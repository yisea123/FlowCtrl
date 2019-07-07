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
** This is the public header file for the CRC calculation routines.
********************************************************************************
********************************************************************************
** Services:
**
**    CRC_Crc32()                   - CRC32 checksum calculation function.
********************************************************************************
********************************************************************************
*/

#ifndef CRC32_H
#define CRC32_H

#include "abcc_drv_cfg.h"
#include "abcc_td.h"

/*******************************************************************************
** Typedefs
********************************************************************************
*/

/*******************************************************************************
** Public Services
********************************************************************************
*/

/*------------------------------------------------------------------------------
** CRC_Crc32()
**
** Calculates a CRC32 checksum on the indicated bytes including transforming
** input bytes from LSB to MSB for the SPI use.
**------------------------------------------------------------------------------
** Inputs:
**    pbBufferStart            - Where to start the calculation.
**    iLength                  - The amount of bytes to include.
**
** Outputs:
**    Returns                  - The calculated CRC32 checksum for the SPI.
**
** Usage:
**    iCrc = CRC_Crc16( pbStart, 20 );
**------------------------------------------------------------------------------
*/

EXTFUNC UINT32 CRC_Crc32( UINT16* piBufferStart, UINT16 iLength );



#endif  /* inclusion lock */

/*******************************************************************************
** End of abcc_crc32.h
********************************************************************************
*/
