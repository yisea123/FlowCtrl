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
** file_description
********************************************************************************
********************************************************************************
** Services:
** ABCC_SYS_SerRegDataReceived() - Indicate to driver that RX data is received
** ABCC_SYS_SerSendReceive()          - Send TX telegram and prepare RX receive.
** ABCC_SYS_SerRestart()              - Restart serial driver
********************************************************************************
********************************************************************************
*/

#ifndef ABCC_SYS_ADAPT_SER_
#define ABCC_SYS_ADAPT_SER_
#include "abcc_drv_cfg.h"
#include "abcc_td.h"
/*******************************************************************************
** Constants
********************************************************************************
*/

/*******************************************************************************
** Typedefs
********************************************************************************
*/
/*------------------------------------------------------------------------------
** Function pointer type for RX telegram ready callback.
** **------------------------------------------------------------------------------
*/
typedef void ( *ABCC_SYS_SerDataReceivedCbfType )( void );

/*******************************************************************************
** Public Globals
********************************************************************************
*/

/*******************************************************************************
** Public Services()
********************************************************************************
*/

/*------------------------------------------------------------------------------
** ABCC_SYS_SerRegDataReceived()
** Registers a callback function that indicates that new data has been received
** on the serial channel. The received data shall be located in the buffer
** provided when ABCC_SYS_SerSendRecieved() was called.
** The data in the buffer is valid until next time ABCC_SYS_SerSendReceive() is
** called.
**------------------------------------------------------------------------------
** Arguments:
**    pnDataReceived: Function to call when RX telegram (pong) is received.
**
** Returns:
**          -
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_SYS_SerRegDataReceived( ABCC_SYS_SerDataReceivedCbfType pnDataReceived );

/*------------------------------------------------------------------------------
** Send TX telegram and prepare for RX telegram reception. The the RX telegram
** will be stored in the pbRxDataBuffer.
** Note the data pbRxDataBuffer is not valid upon return of this function. When
** valid data RX data is available the callback function registered in
** ABCC_SYS_SerRegDataReceived() will be called.
**------------------------------------------------------------------------------
** Arguments:
**             pxTxDataBuffer       Pointer to TX telegram
**             pxRxDataBuffer       Pointer to RX telegram.
**             iTxSize              Length of TX telegram
**             iRxSize              Expected length of RX telegram (in bytes)
** Returns:
**          None
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_SYS_SerSendReceive( void* pxTxDataBuffer, void* pxRxDataBuffer,
                                      UINT16 iTxSize, UINT16 iRxSize );


/*------------------------------------------------------------------------------
** ABCC_SYS_SerRestart()
** Restart the serial driver. Typically used when telegram has timed out.
** Flush all buffers and restart communication and start waiting for RX telegram
** with length of latest provided RX telegram length
** (provided in ABCC_SYS_SerSendReceive()
**------------------------------------------------------------------------------
** Arguments:
**          None
** Returns:
**          None
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_SYS_SerRestart( void );


#endif  /* inclusion lock */
