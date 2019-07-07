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
** User configuration to set the identification parameters of an ABCC module
**
** All configurations in this file should be possible to override if defined
** in abcc_platform_cfg.h. This allows the target platform to adapt the
** configuration to realize its implementation for specific needs.
********************************************************************************
********************************************************************************
** Services:
********************************************************************************
********************************************************************************
*/

#ifndef ABCC_IDENTIFICATION_H_
#define ABCC_IDENTIFICATION_H_

#include "abcc_platform_cfg.h"

#ifndef ABCC_OBJ_CFG_H_
#error Do not include abcc_identification.h, include abcc_obj_cfg.h instead.
#endif

/*******************************************************************************
** Constants
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Ethernet Powerlink (0xE9)
**------------------------------------------------------------------------------
*/
#if EPL_OBJ_ENABLE
/*
** Attribute 1: Vendor ID (UINT32 - 0x00000000-0xFFFFFFFF)
*/
#ifndef EPL_IA_VENDOR_ID_ENABLE
   #define EPL_IA_VENDOR_ID_ENABLE                 TRUE
   #define EPL_IA_VENDOR_ID_VALUE                  0xFFFFFFFF
#endif

/*
** Attribute 2: Product Code type (UINT32 - 0x00000000-0xFFFFFFFF)
*/
#ifndef EPL_IA_PRODUCT_CODE_ENABLE
   #define EPL_IA_PRODUCT_CODE_ENABLE              TRUE
   #define EPL_IA_PRODUCT_CODE_VALUE               0xFFFFFFFF
#endif

/*
** Attribute 3: Revision High Word (UINT16 - 0x0000-0xFFFF)
*/
#ifndef EPL_IA_REVISION_HW_ENABLE
   #define EPL_IA_REVISION_HW_ENABLE               TRUE
   #define EPL_IA_REVISION_HW_VALUE                0xFFFF
#endif

/*
** Attribute 4: Revision Low Word (UINT16 - 0x0000-0xFFFF)
*/
#ifndef EPL_IA_REVISION_LW_ENABLE
   #define EPL_IA_REVISION_LW_ENABLE               TRUE
   #define EPL_IA_REVISION_LW_VALUE                0xFFFF
#endif

/*
** Attribute 5: Serial number (UINT32 - function/variable/0x00000000-0xFFFFFFFF)
*/
#ifndef EPL_IA_SERIAL_NUMBER_ENABLE
   #define EPL_IA_SERIAL_NUMBER_ENABLE             TRUE
   #define EPL_IA_SERIAL_NUMBER_VALUE              0x12345678
#endif

/*
** Attribute 6: Manufacturer device name (Array of CHAR)
*/
#ifndef EPL_IA_MANF_DEVICE_NAME_ENABLE
   #define EPL_IA_MANF_DEVICE_NAME_ENABLE          TRUE
   #define EPL_IA_MANF_DEVICE_NAME_VALUE           "Manufacturer device name"
#endif

/*
** Attribute 7: Manufacturer hardware version (Array of CHAR)
*/
#ifndef EPL_IA_MANF_HW_VERSION_ENABLE
   #define EPL_IA_MANF_HW_VERSION_ENABLE           TRUE
   #define EPL_IA_MANF_HW_VERSION_VALUE            "01"
#endif

/*
** Attribute 8: Manufacturer software version (Array of CHAR)
*/
#ifndef EPL_IA_MANF_SW_VERSION_ENABLE
   #define EPL_IA_MANF_SW_VERSION_ENABLE           TRUE
   #define EPL_IA_MANF_SW_VERSION_VALUE            "01"
#endif

/*
** Attribute 10: Device type (UINT32 - 0x00000000-0xFFFFFFFF)
*/
#ifndef EPL_IA_DEVICE_TYPE_ENABLE
   #define EPL_IA_DEVICE_TYPE_ENABLE               TRUE
   #define EPL_IA_DEVICE_TYPE_VALUE                0xFFFFFFFF
#endif

/*
** Attribute 14: Manufacturer name (Array of CHAR)
*/
#ifndef EPL_IA_MANF_NAME_ENABLE
   #define EPL_IA_MANF_NAME_ENABLE                 TRUE
   #define EPL_IA_MANF_NAME_VALUE                  "Manufacturer name"
#endif

#endif /* #if EPL_OBJ_ENABLE */

/*------------------------------------------------------------------------------
** EtherCAT (0xF5)
**------------------------------------------------------------------------------
*/
#if ECT_OBJ_ENABLE
/*
** Attribute 1: Vendor ID (UINT32 - 0x00000000-0xFFFFFFFF)
*/
#ifndef ECT_IA_VENDOR_ID_ENABLE
   #define ECT_IA_VENDOR_ID_ENABLE                 TRUE
   #define ECT_IA_VENDOR_ID_VALUE                  0xFFFFFFFF
#endif

/*
** Attribute 2: Product Code type (UINT32 - 0x00000000-0xFFFFFFFF)
*/
#ifndef ECT_IA_PRODUCT_CODE_ENABLE
   #define ECT_IA_PRODUCT_CODE_ENABLE              TRUE
   #define ECT_IA_PRODUCT_CODE_VALUE               0xFFFFFFFF
#endif

/*
** Attribute 3: Revision High Word (UINT16 - 0x0000-0xFFFF)
*/
#ifndef ECT_IA_MAJOR_REV_ENABLE
   #define ECT_IA_MAJOR_REV_ENABLE                 TRUE
   #define ECT_IA_REVISION_HW_VALUE                0xFFFF
#endif

/*
** Attribute 4: Revision Low Word (UINT16 - 0x0000-0xFFFF)
*/
#ifndef ECT_IA_MINOR_REV_ENABLE
   #define ECT_IA_MINOR_REV_ENABLE                 TRUE
   #define ECT_IA_REVISION_LW_VALUE                0xFFFF
#endif

/*
** Attribute 5: Serial number (UINT32 - function/variable/0x00000000-0xFFFFFFFF)
*/
#ifndef ECT_IA_SERIAL_NUMBER_ENABLE
   #define ECT_IA_SERIAL_NUMBER_ENABLE             TRUE
   #define ECT_IA_SERIAL_NUMBER_VALUE              0x12345678
#endif

/*
** Attribute 6: Manufacturer device name (Array of CHAR)
*/
#ifndef ECT_IA_MANF_DEVICE_NAME_ENABLE
   #define ECT_IA_MANF_DEVICE_NAME_ENABLE          TRUE
   #define ECT_IA_MANF_DEVICE_NAME_VALUE           "Manufacturer device name"
#endif

/*
** Attribute 7: Manufacturer hardware version (Array of CHAR)
*/
#ifndef ECT_IA_MANF_HW_VERSION_ENABLE
   #define ECT_IA_MANF_HW_VERSION_ENABLE           TRUE
   #define ECT_IA_MANF_HW_VERSION_VALUE            "FF"
#endif

/*
** Attribute 8: Manufacturer software version (Array of CHAR)
*/
#ifndef ECT_IA_MANF_SW_VERSION_ENABLE
   #define ECT_IA_MANF_SW_VERSION_ENABLE           TRUE
   #define ECT_IA_MANF_SW_VERSION_VALUE            "FF"
#endif

/*
** Attribute 10: Device type (UINT32 - 0x00000000-0xFFFFFFFF)
*/
#ifndef ECT_IA_DEVICE_TYPE_ENABLE
   #define ECT_IA_DEVICE_TYPE_ENABLE               TRUE
   #define ECT_IA_DEVICE_TYPE_VALUE                0xFFFFFFFF
#endif

#endif /* #if ECT_OBJ_ENABLE */

/*------------------------------------------------------------------------------
** PROFINET IO Object (0xF6)
**------------------------------------------------------------------------------
*/
#if PRT_OBJ_ENABLE
/*
** Attribute 1: Device ID (UINT16 - 0x0000-0xFFFF)
*/
#ifndef PRT_IA_DEVICE_ID_ENABLE
   #define PRT_IA_DEVICE_ID_ENABLE                 TRUE
   #define PRT_IA_DEVICE_ID_VALUE                  0xFFFF
#endif

/*
** Attribute 2: Vendor ID (UINT16 - 0x0000-0xFFFF)
*/
#ifndef PRT_IA_VENDOR_ID_ENABLE
   #define PRT_IA_VENDOR_ID_ENABLE                 TRUE
   #define PRT_IA_VENDOR_ID_VALUE                  0xFFFF
#endif

/*
** Attribute 3: Station Type (Array of CHAR)
*/
#ifndef PRT_IA_STATION_TYPE_ENABLE
   #define PRT_IA_STATION_TYPE_ENABLE              TRUE
   #define PRT_IA_STATION_TYPE_VALUE               "Station Type"
#endif

/*
** Attribute 8: IM Order ID (Array of CHAR)
*/
#ifndef PRT_IA_IM_ORDER_ID_ENABLE
   #define PRT_IA_IM_ORDER_ID_ENABLE               TRUE
   #define PRT_IA_IM_ORDER_ID_VALUE                "Order ID"
#endif

/*
** Attribute 9: IM Serial Number (Array of CHAR)
*/
#ifndef PRT_IA_IM_SERIAL_NBR_ENABLE
   #define PRT_IA_IM_SERIAL_NBR_ENABLE             TRUE
   #define PRT_IA_IM_SERIAL_NBR_VALUE              "0123456789ABCDEF"
#endif

/*
** Attribute 10: IM Hardware revision (UINT16 - 0x0000-0xFFFF)
*/
#ifndef PRT_IA_IM_HW_REV_ENABLE
   #define PRT_IA_IM_HW_REV_ENABLE                 TRUE
   #define PRT_IA_IM_HW_REV_VALUE                  0x0000
#endif

/*
** Attribute 11: IM Software revision (CHAR, 3 * UINT8)
*/
#ifndef PRT_IA_IM_SW_REV_ENABLE
   #define PRT_IA_IM_SW_REV_ENABLE                 TRUE
   #define PRT_IA_IM_SW_REV_SYMBOL_VALUE           'V'
   #define PRT_IA_IM_SW_REV_MAJOR_VALUE            99
   #define PRT_IA_IM_SW_REV_MINOR_VALUE            99
   #define PRT_IA_IM_SW_REV_BUILD_VALUE            99
#endif

/*
** Attribute 19: System Description (Array of CHAR)
*/
#ifndef PRT_IA_SYSTEM_DESCRIPTION_ENABLE
   #define PRT_IA_SYSTEM_DESCRIPTION_ENABLE        TRUE
   #define PRT_IA_SYSTEM_DESCRIPTION_VALUE         "System Description"
#endif

/*
** Attribute 22: System Contact (Array of CHAR)
*/
#ifndef PRT_IA_SYSTEM_CONTACT_ENABLE
   #define PRT_IA_SYSTEM_CONTACT_ENABLE            TRUE
   #define PRT_IA_SYSTEM_CONTACT_VALUE             "System Contact"
#endif

#endif /* #if PRT_OBJ_ENABLE */

/*------------------------------------------------------------------------------
** EtherNet/IP (0xF8)
**------------------------------------------------------------------------------
*/
#if EIP_OBJ_ENABLE
/*
** Attribute 1: Vendor ID (UINT16 - 0x0000-0xFFFF)
*/
#ifndef EIP_IA_VENDOR_ID_ENABLE
   #define EIP_IA_VENDOR_ID_ENABLE                 TRUE
   #define EIP_IA_VENDOR_ID_VALUE                  0xFFFF
#endif

/*
** Attribute 2: Device type (UINT16 - 0x0000-0xFFFF)
*/
#ifndef EIP_IA_DEVICE_TYPE_ENABLE
   #define EIP_IA_DEVICE_TYPE_ENABLE               TRUE
   #define EIP_IA_DEVICE_TYPE_VALUE                0xFFFF
#endif

/*
** Attribute 3: Product code (UINT16 - 0x0000-0xFFFF)
*/
#ifndef EIP_IA_PRODUCT_CODE_ENABLE
   #define EIP_IA_PRODUCT_CODE_ENABLE              TRUE
   #define EIP_IA_PRODUCT_CODE_VALUE               0xFFFF
#endif

/*
** Attribute 4: Revision (Array of UINT8 - {0x01-0xFF})
*/
#ifndef EIP_IA_REVISION_ENABLE
   #define EIP_IA_REVISION_ENABLE                  TRUE
   #define EIP_IA_REVISION_MAJOR_VALUE             99
   #define EIP_IA_REVISION_MINOR_VALUE             99
#endif

/*
** Attribute 5: Serial number (UINT32 - function/variable/0x00000000-0xFFFFFFFF)
*/
#ifndef EIP_IA_SERIAL_NUMBER_ENABLE
   #define EIP_IA_SERIAL_NUMBER_ENABLE             TRUE
   #define EIP_IA_SERIAL_NUMBER_VALUE              0x12345678
#endif

/*
** Attribute 6: Product name (Array of CHAR)
*/
#ifndef EIP_IA_PRODUCT_NAME_ENABLE
   #define EIP_IA_PRODUCT_NAME_ENABLE              TRUE
   #define EIP_IA_PRODUCT_NAME_VALUE               "Product Name"
#endif

#endif /* #if EIP_OBJ_ENABLE */

/*------------------------------------------------------------------------------
** Ethernet Object (0xF9)
**------------------------------------------------------------------------------
*/
#if ETN_OBJ_ENABLE
/*
** Attribute 1: MAC Address (Array[6] of UINT8)
*/
#ifndef ETN_IA_MAC_ADDRESS_ENABLE
   #define ETN_IA_MAC_ADDRESS_ENABLE               TRUE
   #define ETN_IA_MAC_ADDRESS_VALUE                "\x00\x30\x11\x11\xFF\xFF"
#endif

#endif /* #if ETN_OBJ_ENABLE */

/*------------------------------------------------------------------------------
** Modbus (0xFA)
**------------------------------------------------------------------------------
*/
#if MOD_OBJ_ENABLE
/*
** Attribute 1: Vendor Name (Array of CHAR)
*/
#ifndef MOD_IA_VENDOR_NAME_ENABLE
   #define MOD_IA_VENDOR_NAME_ENABLE               TRUE
   #define MOD_IA_VENDOR_NAME_VALUE                "Vendor Name"
#endif

/*
** Attribute 2: Product Code (Array of CHAR)
*/
#ifndef MOD_IA_PRODUCT_CODE_ENABLE
   #define MOD_IA_PRODUCT_CODE_ENABLE              TRUE
   #define MOD_IA_PRODUCT_CODE_VALUE               "Product Code"
#endif

/*
** Attribute 3: Revision (Array of CHAR)
*/
#ifndef MOD_IA_REVISION_ENABLE
   #define MOD_IA_REVISION_ENABLE                  TRUE
   #define MOD_IA_REVISION_VALUE                   "99.99"
#endif

/*
** Attribute 4: Vendor URL (Array of CHAR)
*/
#ifndef MOD_IA_VENDOR_URL_ENABLE
   #define MOD_IA_VENDOR_URL_ENABLE                TRUE
   #define MOD_IA_VENDOR_URL_VALUE                 "www.vendor.url"
#endif

/*
** Attribute 5: Product Name (Array of CHAR)
*/
#ifndef MOD_IA_PRODUCT_NAME_ENABLE
   #define MOD_IA_PRODUCT_NAME_ENABLE              TRUE
   #define MOD_IA_PRODUCT_NAME_VALUE               "Product Name"
#endif

/*
** Attribute 6: Model Name (Array of CHAR)
*/
#ifndef MOD_IA_MODEL_NAME_ENABLE
   #define MOD_IA_MODEL_NAME_ENABLE                TRUE
   #define MOD_IA_MODEL_NAME_VALUE                 "Model Name"
#endif

/*
** Attribute 7: User Application Name (Array of CHAR)
*/
#ifndef MOD_IA_USER_APP_NAME_ENABLE
   #define MOD_IA_USER_APP_NAME_ENABLE             TRUE
   #define MOD_IA_USER_APP_NAME_VALUE              "User Application Name"
#endif

/*
** Attribute 8: Device ID (Array of UINT8)
*/
#ifndef MOD_IA_DEVICE_ID_ENABLE
   #define MOD_IA_DEVICE_ID_ENABLE                 TRUE
   #define MOD_IA_DEVICE_ID_VALUE                  "\x00\x01\x02\x03"
   #define MOD_IA_DEVICE_ID_ARRAY_SIZE             4
#endif

#endif /* #if MOD_OBJ_ENABLE */

/*------------------------------------------------------------------------------
** COP Object (0xFB)
**------------------------------------------------------------------------------
*/
#if COP_OBJ_ENABLE
/*
** Attribute 1: Vendor ID (UINT32 - 0x00000000-0xFFFFFFFF)
*/
#ifndef COP_IA_VENDOR_ID_ENABLE
   #define COP_IA_VENDOR_ID_ENABLE                 TRUE
   #define COP_IA_VENDOR_ID_VALUE                  0xFFFFFFFF
#endif

/*
** Attribute 2: Product Code (UINT32 - 0x00000000-0xFFFFFFFF)
*/
#ifndef COP_IA_PRODUCT_CODE_ENABLE
   #define COP_IA_PRODUCT_CODE_ENABLE              TRUE
   #define COP_IA_PRODUCT_CODE_VALUE               0xFFFFFFFF
#endif

/*
** Attribute 3: Major Revision (UINT16 - 0x0000-0xFFFF)
*/
#ifndef COP_IA_REV_MAJOR_ENABLE
   #define COP_IA_REV_MAJOR_ENABLE                 TRUE
   #define COP_IA_REV_MAJOR_VALUE                  0xFFFF
#endif

/*
** Attribute 4: Minor Revision (UINT16 - 0x0000-0xFFFF)
*/
#ifndef COP_IA_REV_MINOR_ENABLE
   #define COP_IA_REV_MINOR_ENABLE                 TRUE
   #define COP_IA_REV_MINOR_VALUE                  0xFFFF
#endif

/*
** Attribute 5: Serial Number (UINT32 - 0x00000000-0xFFFFFFFF)
*/
#ifndef COP_IA_SERIAL_NUMBER_ENABLE
   #define COP_IA_SERIAL_NUMBER_ENABLE             TRUE
   #define COP_IA_SERIAL_NUMBER_VALUE              0xFFFFFFFF
#endif

/*
** Attribute 6: Manufacturer Device Name (Array of CHAR)
*/
#ifndef COP_IA_MANF_DEV_NAME_ENABLE
   #define COP_IA_MANF_DEV_NAME_ENABLE             TRUE
   #define COP_IA_MANF_DEV_NAME_VALUE              "Device Name"
#endif

/*
** Attribute 7: Manufacturer Hardware Version (Array of CHAR)
*/
#ifndef COP_IA_MANF_HW_VER_ENABLE
   #define COP_IA_MANF_HW_VER_ENABLE               TRUE
   #define COP_IA_MANF_HW_VER_VALUE                "Hardware Version"
#endif

/*
** Attribute 8: Manufacturer Software Version (Array of CHAR)
*/
#ifndef COP_IA_MANF_SW_VER_ENABLE
   #define COP_IA_MANF_SW_VER_ENABLE               TRUE
   #define COP_IA_MANF_SW_VER_VALUE                "Software Version"
#endif

#endif /* #if COP_OBJ_ENABLE */

/*------------------------------------------------------------------------------
** DeviceNet (0xFC)
**------------------------------------------------------------------------------
*/
#if DEV_OBJ_ENABLE
/*
** Attribute 1: Vendor ID (UINT16 - 0x0000-0xFFFF)
*/
#ifndef DEV_IA_VENDOR_ID_ENABLE
   #define DEV_IA_VENDOR_ID_ENABLE                 TRUE
   #define DEV_IA_VENDOR_ID_VALUE                  0xFFFF
#endif

/*
** Attribute 2: Device type (UINT16 - 0x0000-0xFFFF)
*/
#ifndef DEV_IA_DEVICE_TYPE_ENABLE
   #define DEV_IA_DEVICE_TYPE_ENABLE               TRUE
   #define DEV_IA_DEVICE_TYPE_VALUE                0xFFFF
#endif

/*
** Attribute 3: Product code (UINT16 - 0x0000-0xFFFF)
*/
#ifndef DEV_IA_PRODUCT_CODE_ENABLE
   #define DEV_IA_PRODUCT_CODE_ENABLE              TRUE
   #define DEV_IA_PRODUCT_CODE_VALUE               0xFFFF
#endif

/*
** Attribute 4: Revision (Array of UINT8 - {0x01-0xFF})
*/
#ifndef DEV_IA_REVISION_ENABLE
   #define DEV_IA_REVISION_ENABLE                  TRUE
   #define DEV_IA_REVISION_MAJOR_VALUE             99
   #define DEV_IA_REVISION_MINOR_VALUE             99
#endif

/*
** Attribute 5: Serial number (UINT32 - function/variable/0x00000000-0xFFFFFFFF)
*/
#ifndef DEV_IA_SERIAL_NUMBER_ENABLE
   #define DEV_IA_SERIAL_NUMBER_ENABLE             TRUE
   #define DEV_IA_SERIAL_NUMBER_VALUE              0x12345678
#endif

/*
** Attribute 6: Product name (Array of CHAR)
*/
#ifndef DEV_IA_PRODUCT_NAME_ENABLE
   #define DEV_IA_PRODUCT_NAME_ENABLE              TRUE
   #define DEV_IA_PRODUCT_NAME_VALUE               "Product Name"
#endif

#endif /* #if DEV_OBJ_ENABLE */

/*------------------------------------------------------------------------------
** DPV1 Object (0xFD)
**------------------------------------------------------------------------------
*/
#if DPV1_OBJ_ENABLE
/*
** Attribute 1: PNO Ident number (UINT16 - 0x0000-0xFFFF)
*/
#ifndef DPV1_IA_IDENT_NUMBER_ENABLE
   #define DPV1_IA_IDENT_NUMBER_ENABLE             TRUE
   #define DPV1_IA_IDENT_NUMBER_VALUE              0xFFFF
#endif

/*
** Attribute 8: Manufacturer Ident number (UINT16 - 0x0000-0xFFFF)
*/
#ifndef DPV1_IA_MANUFACTURER_ID_ENABLE
   #define DPV1_IA_MANUFACTURER_ID_ENABLE          TRUE
   #define DPV1_IA_MANUFACTURER_ID_VALUE           0xFFFF
#endif

/*
** Attribute 9: IM Order ID (Array of CHAR)
*/
#ifndef DPV1_IA_ORDER_ID_ENABLE
   #define DPV1_IA_ORDER_ID_ENABLE                 TRUE
   #define DPV1_IA_ORDER_ID_VALUE                  "Order ID"
#endif

/*
** Attribute 10: IM Serial Number (Array of CHAR)
*/
#ifndef DPV1_IA_SERIAL_NO_ENABLE
   #define DPV1_IA_SERIAL_NO_ENABLE                TRUE
   #define DPV1_IA_SERIAL_NO_VALUE                 "0123456789ABCDEF"
#endif

/*
** Attribute 11: IM Hardware revision (UINT16 - 0x0000-0xFFFF)
*/
#ifndef DPV1_IA_HW_REV_ENABLE
   #define DPV1_IA_HW_REV_ENABLE                   TRUE
   #define DPV1_IA_HW_REV_VALUE                    0x0000
#endif

/*
** Attribute 12: IM Software revision (CHAR, 3 * UINT8)
*/
#ifndef DPV1_IA_SW_REV_ENABLE
   #define DPV1_IA_SW_REV_ENABLE                   TRUE
   #define DPV1_IA_SW_REV_SYMBOL_VALUE             'V'
   #define DPV1_IA_SW_REV_MAJOR_VALUE              99
   #define DPV1_IA_SW_REV_MINOR_VALUE              99
   #define DPV1_IA_SW_REV_BUILD_VALUE              99
#endif

#endif  /* DPV1_OBJ_ENABLE */

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

#endif  /* inclusion lock */
