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
** permission. When used together with a product from HMS, this code can be   **
** modified, reproduced and distributed in binary form without any            **
** restrictions.                                                              **
**                                                                            **
** THE CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND. HMS DOES NOT    **
** WARRANT THAT THE FUNCTIONS OF THE CODE WILL MEET YOUR REQUIREMENTS, OR     **
** THAT THE OPERATION OF THE CODE WILL BE UNINTERRUPTED OR ERROR-FREE, OR     **
** THAT DEFECTS IN IT CAN BE CORRECTED.                                       **
********************************************************************************
********************************************************************************
** Defines the generic driver interface implemented by each specific driver.
********************************************************************************
********************************************************************************
** Driver services:
**       ABCC_DrvPar30Init()	                    - Initialize driver privates and states to default values.
**       ABCC_DrvPar30ISR()                       - Calls in the interrupt context to acknowledge received interrupts.
**       ABCC_DrvPar30Execute()                   - Drives the internal send/receive process if applicable
**       ABCC_DrvPar30PrepareWriteMessage()       - Copy message data
**       ABCC_DrvPar30WriteMessage()              - Writes a message.
**       ABCC_DrvPar30WriteProcessData()          - Writes current process data.
**       ABCC_DrvPar30IsReadyForWriteMessage()    - Checks if the driver is ready to send a new write message.
**       ABCC_DrvPar30IsReadyForCmd()             - Checks if the Anybus is ready to receive a new command message.
**       ABCC_DrvPar30SetNbrOfCmds()              - Sets the number of simultaneous commands that is supported by the application.
**       ABCC_DrvPar30SetAppStatus()              - Sets the current application status.
**       ABCC_DrvPar30SetPdSize()                 - Sets the current process data size.
**       ABCC_DrvPar30SetMsgReceiverBuffer()      - Sets the message receiver buffer.
**       ABCC_DrvPar30SetIntMask()                - Set interrupt mask
**       ABCC_DrvPar30GetWrPdBuffer()             - Get wrpd buffer
**       ABCC_DrvPar30GetModCap()                 - Read module capability
**       ABCC_DrvPar30GetLedStat()                - Read led status
**       ABCC_DrvPar30GetIntStatus()              - Get current interrupt status
**       ABCC_DrvPar30GetAnybusState()            - Get current Anybus state
**       ABCC_DrvPar30ReadProcessData()           - Get read process data if any.
**       ABCC_DrvPar30ReadMessage()               - Get read message if any.
**       ABCC_DrvPar30RegisterWd()                - Function to call when Wd timeOuts
**       ABCC_DrvPar30IsSuperviced()              - Is the network supervised
********************************************************************************
********************************************************************************
*/

#ifndef PHY_DRV_PAR30_IF_H_
#define PHY_DRV_PAR30_IF_H_

#include "abcc_drv_cfg.h"
#include "abcc_td.h"
#include "abp.h"

#if( ABCC_CFG_DRV_PARALLEL_30 )
/*******************************************************************************
** Constants
********************************************************************************
*/

/*******************************************************************************
** Typedefs
********************************************************************************
*/

/*******************************************************************************
** Public Globals
********************************************************************************
*/

/*******************************************************************************
** Public Services
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Initializes the driver to default values.
** Must be called before the driver is used.
**------------------------------------------------------------------------------
** Arguments:
**       None.
**
** Returns:
**       None.
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_DrvPar30Init( UINT8 bOpmode );


/*------------------------------------------------------------------------------
** Calls in the interrupt context to acknowledge received interrupts.
**
** Remarks:
**       The ISR routine will clear all pending interrupts.
**------------------------------------------------------------------------------
** Arguments:
**       None.
**
** Returns:
**       Acknowledged interrupts.
**------------------------------------------------------------------------------
*/
EXTFUNC UINT16  ABCC_DrvPar30ISR( void );


/*------------------------------------------------------------------------------
** Drives the internal send process.
**------------------------------------------------------------------------------
** Arguments:
**       None.
**
** Returns:
**       None.
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_DrvPar30RunDriverTx( void );

/*------------------------------------------------------------------------------
** Drives the internal receive process.
**------------------------------------------------------------------------------
** Arguments:
**       None.
**
** Returns:
**       Pointer to successfully sent write message.
**------------------------------------------------------------------------------
*/
EXTFUNC ABP_MsgType* ABCC_DrvPar30RunDriverRx( void );

/*------------------------------------------------------------------------------
** Copies the message data to the ABCC memory.
** The actual write trigger is done by ABCC_DrvParWriteMessage.
** Note! It is only allowed to call this functions if the driver is ready to
** handle a new message. Use the following functions to ensure this:
** ABCC_DrvParIsReadyForWriteMessage()
** ABCC_DrvParIsReadyForCmd()
**------------------------------------------------------------------------------
** Arguments:
**       psWriteMsg:    Pointer to message.
**
** Returns:
**       None
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_DrvPar30PrepareWriteMessage( ABP_MsgType* psWriteMsg );

/*------------------------------------------------------------------------------
** Writes a message to the driver.
**------------------------------------------------------------------------------
** Arguments:
**       psWriteMsg:    Pointer to message.
**
** Returns:
**       True:          Message was successfully written and can be deallocated
**                      immediately.
**       False:         Message was not yet written and cannot be deallocated.
**                      The psWriteMsg pointer is owned by the driver until the
**                      message is written and the pointer is returned in the
**                      driver execution response.
**------------------------------------------------------------------------------
*/
EXTFUNC BOOL ABCC_DrvPar30WriteMessage( ABP_MsgType* psWriteMsg );


/*------------------------------------------------------------------------------
** Writes current process data.
** The data is copied before returning from the method.
**------------------------------------------------------------------------------
** Arguments:
**       pxProcessData: Pointer to process data to be sent.
**
** Returns:
**       None.
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_DrvPar30WriteProcessData( void* pxProcessData );

/*------------------------------------------------------------------------------
** Checks if the driver is in the correct state for writing process data to the anybus
**------------------------------------------------------------------------------
** Arguments:
**       None.
**
** Returns:
**       True:          Driver is in correct state to send WrPd
**       False:         Driver is not in correct state to send Wrpd
**------------------------------------------------------------------------------
*/
EXTFUNC BOOL ABCC_DrvPar30IsReadyForWrPd( void );


/*------------------------------------------------------------------------------
** Checks if the driver is ready to send a new write message.
**------------------------------------------------------------------------------
** Arguments:
**       None.
**
** Returns:
**       True:          Driver is ready to send a new write message.
**       False:         Driver is not ready to send a new write message.
**------------------------------------------------------------------------------
*/
EXTFUNC BOOL ABCC_DrvPar30IsReadyForWriteMessage( void );


/*------------------------------------------------------------------------------
** The host application checks if the Anybus is ready to receive a new command
** message.
**------------------------------------------------------------------------------
** Arguments:
**       None.
**
** Returns:
**       True:          OK to send new command.
**       False:         NOK to send new command.
**------------------------------------------------------------------------------
*/
EXTFUNC BOOL ABCC_DrvPar30IsReadyForCmd( void );


/*------------------------------------------------------------------------------
** Sets the number of simultaneous commands that is supported by the application.
**------------------------------------------------------------------------------
** Arguments:
**       bNbrOfCmds:    Number of commands that the application is ready to receive.
**
** Returns:
**       None.
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_DrvPar30SetNbrOfCmds( UINT8 bNbrOfCmds );


/*------------------------------------------------------------------------------
**  Sets the current application status.
**  Note! This information is not supported by all protocols.
**------------------------------------------------------------------------------
** Arguments:
**       eAppStatus:    Current application status.
**
** Returns:
**       None.
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_DrvPar30SetAppStatus( ABP_AppStatusType eAppStatus );


/*------------------------------------------------------------------------------
** Sets the current process data size.
**------------------------------------------------------------------------------
** Arguments:
**       iReadPdSize:   Size of read process data (bytes)
**       iWritePdSize:  Size of write process data (bytes)
**
** Returns:
**       None.
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_DrvPar30SetPdSize( const UINT16 iReadPdSize, const UINT16 iWritePdSize );


/*------------------------------------------------------------------------------
** Sets the receiver buffer, to be used for the next read message.
**------------------------------------------------------------------------------
** Arguments:
**       psReadMsg:     Pointer where next read message will be put.
**                      psReadMsg is not allowed to contain a NULL value.
**
** Returns:
**       None.
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_DrvPar30SetMsgReceiverBuffer( ABP_MsgType* const psReadMsg );


/*------------------------------------------------------------------------------
** Sets Interrupt mask according to h_aci.h.
**------------------------------------------------------------------------------
** Arguments:
**       iIntMask:      Interrupt mask set according to h_aci.h.
**
** Returns:
**       None.
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_DrvPar30SetIntMask( const UINT16 iIntMask );


/*------------------------------------------------------------------------------
** Get WrpdBuffer for the user to update.
**------------------------------------------------------------------------------
** Arguments:
**       None.
**
** Returns:
**       Pointer to WrPd buffer.
**------------------------------------------------------------------------------
*/
EXTFUNC void* ABCC_DrvPar30GetWrPdBuffer( void );


/*------------------------------------------------------------------------------
** Read module capabillity
**------------------------------------------------------------------------------
** Arguments:
**       None.
**
** Returns:
**       Module capability.
**------------------------------------------------------------------------------
*/
EXTFUNC UINT16 ABCC_DrvPar30GetModCap( void );


/*------------------------------------------------------------------------------
** Read module capability
**------------------------------------------------------------------------------
** Arguments:
**       None.
**
** Returns:
**       Module capability.
**------------------------------------------------------------------------------
*/
EXTFUNC UINT16 ABCC_DrvPar30GetLedStatus( void );


/*------------------------------------------------------------------------------
** Gets the Anybus interrupt status.
**
** Remarks:
**       The ABCC_DrvISR() function will clear all pending interrupts. This
**       function must be called before ABCC_DrvISR() or it will always return 0.
**------------------------------------------------------------------------------
** Arguments:
**       None.
**
** Returns:
**       The Anybus interrupt status.
**------------------------------------------------------------------------------
*/
EXTFUNC UINT16 ABCC_DrvPar30GetIntStatus( void );


/*------------------------------------------------------------------------------
** Gets the Anybus state.
**------------------------------------------------------------------------------
** Arguments:
**       None.
**
** Returns:
**       The Anybus state.
**------------------------------------------------------------------------------
*/
EXTFUNC UINT8 ABCC_DrvPar30GetAnybusState( void );


/*------------------------------------------------------------------------------
** Reads the read process data.
**------------------------------------------------------------------------------
** Arguments:
**       None.
**
** Returns:
**       A pointer to the read process data; or NULL if no process data to read
**       was available.
**------------------------------------------------------------------------------
*/
EXTFUNC void* ABCC_DrvPar30ReadProcessData( void );


/*------------------------------------------------------------------------------
** Reads the read message.
**------------------------------------------------------------------------------
** Arguments:
**       None.
**
** Returns:
**       A pointer to the read message; or NULL if no message is available.
**       The pointer, if not NULL, will point to the buffer previously set by
**       calling ABCC_DrvSetMsgReceiverBuffer().
**------------------------------------------------------------------------------
*/
EXTFUNC ABP_MsgType* ABCC_DrvPar30ReadMessage( void );

/*------------------------------------------------------------------------------
**  Returns supervision bit in status register.
**------------------------------------------------------------------------------
** Arguments:
**          -
**
** Returns:
**          TRUE: The device is supervised by another network device.
**------------------------------------------------------------------------------
*/
EXTFUNC BOOL ABCC_DrvPar30IsSupervised( void );


/*------------------------------------------------------------------------------
**  Returns anybus status register.
**------------------------------------------------------------------------------
** Arguments:
**          -
**
** Returns:
**          Anybus status register
**------------------------------------------------------------------------------
*/
EXTFUNC UINT8 ABCC_DrvPar30GetAnbStatus( void );


#endif  /* ABCC_CFG_DRV_PARALLEL_30 */

#endif  /* inclusion lock */

/*******************************************************************************
** End of drv_if.h
********************************************************************************
*/
