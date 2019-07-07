/*******************************************************************************
********************************************************************************
**                                                                            **
** ABCC Starter Kit version 3.02.02 (2016-11-10)                              **
**                                                                            **
** Delivered with:                                                            **
**    ABP            7.31.01 (2016-09-16)                                     **
**    ABCC Driver    5.02.01 (2016-11-02)                                     **
**                                                                            */
/*******************************************************************************
********************************************************************************
** COPYRIGHT NOTIFICATION (c) 2015 HMS Industrial Networks AB                 **
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
** Application Data Object 0xFE (ADI) - Public interfaces
********************************************************************************
********************************************************************************
** Services :
** AD_Init()                  - Initiate AD object.
** AD_ProcObjectRequest()     - Object request handling.
** AD_RemapDone()             - Remap finished.
** AD_UpdatePdReadData()      - Update of read process data.
** AD_UpdatePdWriteData()     - Update of write process data.
********************************************************************************
********************************************************************************
*/
#ifndef AD_OBJ_H
#define AD_OBJ_H

#include "abcc_td.h"
#include "abp.h"
#include "abcc_ad_if.h"
#include "appl_abcc_handler.h"

/*******************************************************************************
** Typedefs
********************************************************************************
*/

/*******************************************************************************
** Public Services
********************************************************************************
*/
/*------------------------------------------------------------------------------
**  Initiates the AD object.
**------------------------------------------------------------------------------
** Arguments:
**    psAdiEntry   - Pointer to used ADI entry table.
**    iNumAdi      - Number of ADI:s in ADI entry table.
**    psDefaultMap - Pointer to default map. Set to NULL if no default map
**                   shall be used.
**
** Returns:
**    APPL_ErrCodeType
**------------------------------------------------------------------------------
*/
EXTFUNC APPL_ErrCodeType AD_Init( const AD_AdiEntryType* psAdiEntry,
                                  UINT16 iNumAdi,
                                  const AD_DefaultMapType* psDefaultMap );

/*------------------------------------------------------------------------------
**  Retrieve default mapping information from AD object. The AD objects also
**  prepares for network endian conversion if needed. The function may only be
**  called when requested by driver through the callback ABCC_CbfAdiMappingReq()
**------------------------------------------------------------------------------
** Arguments:
**     ppsAdiEntry    - Pointer to retrieve ADI entry table.
**                      NULL is returned if no default map is available.
**     ppsDefaultMap  - Pointer to retrieve default map information.
**                      NULL is returned if no default map is available.
**
** Returns:
**    Number of ADI:s in psAdiEntry table.
**------------------------------------------------------------------------------
*/
EXTFUNC UINT16 AD_AdiMappingReq( const AD_AdiEntryType** ppsAdiEntry,
                                 const AD_DefaultMapType** ppsDefaultMap );

/*------------------------------------------------------------------------------
** Indicate to AD object that the remap is finished
**------------------------------------------------------------------------------
** Arguments:
**    None
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
#if( ABCC_CFG_REMAP_SUPPORT_ENABLED )
EXTFUNC void AD_RemapDone( void );
#endif

/*------------------------------------------------------------------------------
** Process an object request against the Application Data Object.
**------------------------------------------------------------------------------
** Arguments:
**    psMsgBuffer      - Pointer to ABCC command message.
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
EXTFUNC void AD_ProcObjectRequest( ABP_MsgType* psMsgBuffer );

/*------------------------------------------------------------------------------
** Update AD object with new read process data received from the ABCC.
**------------------------------------------------------------------------------
** Arguments:
**    pxPdDataBuf      - Pointer read process data buffer.
**
** Returns:
**    None
**------------------------------------------------------------------------------
*/
EXTFUNC void AD_UpdatePdReadData( void* pxPdDataBuf );

/*------------------------------------------------------------------------------
** Fetch write process data from AD object.
**------------------------------------------------------------------------------
** Arguments:
**    pxPdDataBuf       - Pointer write process data buffer.
**
** Returns:
**    TRUE  -  Write process data buffer is updated.
**    FALSE -  No update was made.
**
**------------------------------------------------------------------------------
*/
EXTFUNC BOOL AD_UpdatePdWriteData( void* pxPdDataBuf );
#endif  /* inclusion lock */
