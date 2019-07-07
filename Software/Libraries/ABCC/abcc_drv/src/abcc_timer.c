/*******************************************************************************
********************************************************************************
**                                                                            **
** ABCC Driver version 5.02.01 (2016-11-02)                                   **
**                                                                            **
** Delivered with:                                                            **
**    ABP            7.31.01 (2016-09-16)                                     **
**                                                                            */
/*******************************************************************************
********************************************************************************
** COPYRIGHT NOTIFICATION (c) 2013 HMS Industrial Networks AB                 **
**                                                                            **
** This code is the property of HMS Industrial Networks AB.                   **
** The source code may not be reproduced, distributed, or used without        **
** permission. When used together with a product from HMS, permission is      **
** granted to modify, reproduce and distribute the code in binary form        **
** without any restrictions.                                                  **
**                                                                            **
** THE CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND. HMS DOES NOT    **
** WARRANT THAT THE FUNCTIONS OF THE CODE WILL MEET YOUR REQUIREMENTS, OR     **
** THAT THE OPERATION OF THE CODE WILL BE UNINTERRUPTED OR ERROR-FREE, OR     **
** THAT DEFECTS IN IT CAN BE CORRECTED.                                       **
********************************************************************************
********************************************************************************
** File Description:
** Timer implementation.
********************************************************************************
********************************************************************************
*/

#include "abcc_timer.h"
#include "abcc_debug_err.h"
#include "abcc_port.h"

/*******************************************************************************
** Constants
********************************************************************************
*/


/*******************************************************************************
** Typedefs
********************************************************************************
*/
/*
 * Timer resource structure
 */
typedef struct ABCC_TimerTimeoutType
{
   BOOL  fActive;
   BOOL  fTmoOccured;
   INT32 lTimeLeft;
   ABCC_TimerTimeoutCallbackType pnHandleTimeout;
}
ABCC_TimerTimeoutType;


/*******************************************************************************
** Public Globals
********************************************************************************
*/


/*******************************************************************************
** Private Globals
********************************************************************************
*/
#define MAX_NUM_TIMERS 3
static ABCC_TimerTimeoutType sTimer[ MAX_NUM_TIMERS ];
static BOOL fTimerEnabled = FALSE;

/*******************************************************************************
** Private Services
********************************************************************************
*/


/*******************************************************************************
** Public Services
********************************************************************************
*/
void ABCC_TimerInit( void )
{
   ABCC_TimerHandle xHandle;

   for ( xHandle = 0; xHandle < MAX_NUM_TIMERS; xHandle++ )
   {
      sTimer[ xHandle ].pnHandleTimeout = NULL;
   }
   fTimerEnabled = TRUE;
}


/*
** ABCC_TimerCreateTimer()
*/
ABCC_TimerHandle ABCC_TimerCreate( ABCC_TimerTimeoutCallbackType pnHandleTimeout )
{
   ABCC_TimerHandle xHandle = ABCC_TIMER_NO_HANDLE;
   ABCC_PORT_UseCritical();

   ABCC_PORT_EnterCritical();

   for ( xHandle = 0; xHandle < MAX_NUM_TIMERS; xHandle++ )
   {
      if ( sTimer[ xHandle ].pnHandleTimeout == NULL )
      {
         sTimer[ xHandle ].fActive = FALSE;
         sTimer[ xHandle ].fTmoOccured = FALSE;
         sTimer[ xHandle ].pnHandleTimeout = pnHandleTimeout;
         break;
      }
   }
   ABCC_PORT_ExitCritical();

   if ( xHandle >=  MAX_NUM_TIMERS )
   {
      xHandle = ABCC_TIMER_NO_HANDLE;
   }
   return( xHandle );
}

BOOL ABCC_TimerStart( ABCC_TimerHandle xHandle,
                      UINT32 lTimeoutMs )
{
   BOOL fTmo;
   ABCC_PORT_UseCritical();

   ABCC_ASSERT( sTimer[ xHandle ].pnHandleTimeout );

   ABCC_PORT_EnterCritical();
   fTmo = sTimer[ xHandle ].fTmoOccured;
   sTimer[ xHandle ].lTimeLeft = (INT32)lTimeoutMs;
   sTimer[ xHandle ].fTmoOccured = FALSE;
   sTimer[ xHandle ].fActive = TRUE;

   ABCC_PORT_ExitCritical();
   return( fTmo );
}


/*
** ABCC_TimerStopTimer()
*/
BOOL ABCC_TimerStop( ABCC_TimerHandle xHandle )
{
   BOOL fTmo;
   ABCC_PORT_UseCritical();

   ABCC_PORT_EnterCritical();
   fTmo = sTimer[ xHandle ].fTmoOccured;

   sTimer[ xHandle ].fActive = FALSE;
   sTimer[ xHandle ].fTmoOccured = FALSE;

   ABCC_PORT_ExitCritical();
   return( fTmo );
}

/*
** ABCC_TimerTick()
*/
void ABCC_TimerTick(const INT16 iDeltaTimeMs)
{
   ABCC_TimerHandle xHandle;
   ABCC_PORT_UseCritical();

   if( !fTimerEnabled )
   {
      return;
   }

   ABCC_PORT_EnterCritical();

   for ( xHandle = 0; xHandle < MAX_NUM_TIMERS; xHandle++ )
   {
       if ( ( sTimer[ xHandle ].pnHandleTimeout != NULL ) &&
             ( sTimer[ xHandle ].fActive == TRUE ) )
       {
          sTimer[ xHandle ].lTimeLeft -= (INT32)iDeltaTimeMs;
          if( sTimer[ xHandle ].lTimeLeft <= 0 )
          {
             sTimer[ xHandle ].fTmoOccured = TRUE;
             sTimer[ xHandle ].fActive = FALSE;
             sTimer[ xHandle ].pnHandleTimeout();
          }
       }
   }

   ABCC_PORT_ExitCritical();
}

void ABCC_TimerDisable( void )
{
   fTimerEnabled = FALSE;
}

/*******************************************************************************
** Tasks
********************************************************************************
*/
