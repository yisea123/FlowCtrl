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
** Defines system specific interface for SPI
********************************************************************************
********************************************************************************
** Services:
** ABCC_SYS_SpiRegDataReceived() - MISO received
** ABCC_SYS_SpiSendReceive()          - Start transaction
********************************************************************************
********************************************************************************
*/

#ifndef ABCC_SYS_ADAPT_SPI_
#define ABCC_SYS_ADAPT_SPI_
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
** Function pointer type for MISO frame ready callback.
** ------------------------------------------------------------------------------
*/
typedef void ( *ABCC_SYS_SpiDataReceivedCbfType )( void );

/*******************************************************************************
** Public Globals
********************************************************************************
*/

/*******************************************************************************
** Public Services()
********************************************************************************
*/

/*------------------------------------------------------------------------------
** ABCC_SYS_SpiRegDataReceived()
** As soon as the MISO frame is received by the low level SPI hardware driver,
** this registered function shall be called to indicate to the SPI driver that
** the MISO frame could be processed.
**------------------------------------------------------------------------------
** Arguments:
**    pnDataReceived Function to call when MISO frame is received.
**
** Returns:
**
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_SYS_SpiRegDataReceived( ABCC_SYS_SpiDataReceivedCbfType pnDataReceived );

/*------------------------------------------------------------------------------
** ABCC_SYS_SpiSendReceive()
** This function is called by the SPI driver when the MOSI frame shall be sent.
** Two buffers are provided, one with MOSI data frame to be sent and one buffer
** to store the received MISO frame.
** If the callback registered by ABCC_SYS_SpiRegDataReceived() is invoked within
** this function, indicating that the MISO frame is already received ,the MISO
** frame will be processed by the SPI driver on return of this function.
** Otherwise the MISO frame will be processed the next time the driver is polled
** (ABCC_RunDriver() ) after the MISO frame received callback is invoked.
**------------------------------------------------------------------------------
** Arguments:
**             pxSendDataBuffer     Pointer to MOSI Buffer to send
**             pxReceiveDataBuffer  Pointer to MISO Buffer.
**             iLength              Length of SPI frame ( in bytes )
** Returns:
**          None.
**------------------------------------------------------------------------------
*/
EXTFUNC void ABCC_SYS_SpiSendReceive( void* pxSendDataBuffer, void* pxReceiveDataBuffer, UINT16 iLength );

#endif  /* inclusion lock */
