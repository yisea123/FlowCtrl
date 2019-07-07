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
** Example of an ADI setup with 11 ADIs covering all data types and structures.
********************************************************************************
********************************************************************************
*/

#include "appl_adi_config.h"

#if ( APPL_ACTIVE_ADI_SETUP == 0xFFFF )

/*******************************************************************************
** Constants
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Access descriptor for the ADIs
**------------------------------------------------------------------------------
*/
#define APPL_READ_MAP_READ_ACCESS_DESC ( ABP_APPD_DESCR_GET_ACCESS |           \
                                         ABP_APPD_DESCR_MAPPABLE_READ_PD )

#define APPL_READ_MAP_WRITE_ACCESS_DESC ( ABP_APPD_DESCR_GET_ACCESS |          \
                                          ABP_APPD_DESCR_SET_ACCESS |          \
                                          ABP_APPD_DESCR_MAPPABLE_READ_PD )

#define APPL_WRITE_MAP_READ_ACCESS_DESC ( ABP_APPD_DESCR_GET_ACCESS |          \
                                          ABP_APPD_DESCR_MAPPABLE_WRITE_PD )

#define APPL_NOT_MAP_READ_ACCESS_DESC ( ABP_APPD_DESCR_GET_ACCESS |            \
                                        ABP_APPD_DESCR_MAPPABLE_WRITE_PD )

#define APPL_NOT_MAP_WRITE_ACCESS_DESC ( ABP_APPD_DESCR_GET_ACCESS |           \
                                         ABP_APPD_DESCR_SET_ACCESS )


#define APPL_ALL_ACCESS_DESC ( ABP_APPD_DESCR_GET_ACCESS |                     \
                               ABP_APPD_DESCR_SET_ACCESS |                     \
                               ABP_APPD_DESCR_MAPPABLE_READ_PD |               \
                               ABP_APPD_DESCR_MAPPABLE_WRITE_PD )

/*******************************************************************************
** Typedefs
********************************************************************************
*/
typedef struct APPL_AdiNotMappable
{
   char  appl_acString[ 20 ];
   UINT8 bOctet[ 10 ];
}
APPL_AdiNotMappableType;

typedef struct APPL_Adi
{
   UINT32  lUint32;
   INT32   lInt32;
   UINT16  iUint16;
   INT16   iInt16;
   UINT16  iBit16;
   UINT8   bUint8;
   INT8    bInt8;
   UINT8   bBit8;
   UINT8   bBitTypes[ 4 ];
   UINT16  iBitTypes[ 2 ];
}
APPL_AdiType;

typedef struct APPL_AdiMinMaxVerif
{
   UINT8   bUint8;
   INT8    bInt8;
   UINT16  iUint16;
   INT16   iInt16;
   UINT32  lUint32;
   INT32   lInt32;
   FLOAT32 rFloat;
   UINT8   bOctet;
   UINT8   eEnum;
}
APPL_AdiMinMaxVerif;


/*******************************************************************************
** Private Globals
********************************************************************************
*/

static UINT32                  appl_lUint32 = 0;
static INT32                   appl_lInt32 = 0;
static UINT16                  appl_iUint16 = 0;
static INT16                   appl_iInt16 = 0;
static UINT16                  appl_iBit16 = 0;
static UINT8                   appl_bUint8 = 0;
static INT8                    appl_bInt8 = 0;
static UINT8                   appl_bBit8 = 0;
static UINT8                   appl_bOctet = 0;
static UINT8                   appl_eEnum = 0;
static UINT8                   appl_bBit2Array = 0;
static FLOAT32                 appl_rFloat = 0;
static AD_ENUMStrType          appl_asEnumStrings[ 3 ] = { { 0, "String0" }, { 1, "String1" }, { 2, "String2" } };
static APPL_AdiType            appl_sAdi1 = { 0, 0, 0, 0, 0, 0, 0, 0, { 0, 0, 0, 0 }, { 0, 0 } };
static APPL_AdiType            appl_sAdi2 = { 0, 0, 0, 0, 0, 0, 0, 0, { 0, 0, 0, 0 }, { 0, 0 } };
static APPL_AdiMinMaxVerif     appl_sAdi4 = { 0, 0, 0, 0, 0, 0, 0.0, 0, 0 };
static APPL_AdiMinMaxVerif     appl_sAdi5 = { 0, 0, 0, 0, 0, 0, 0.0, 0, 0 };
static APPL_AdiMinMaxVerif     appl_sAdi6 = { 0, 0, 0, 0, 0, 0, 0.0, 0, 0 };
static UINT8                   appl_sAdi7[ 2 ] = { 0, 0 };

#ifdef ABCC_SYS_16_BIT_CHAR
static UINT16 appl_acString[] =  { 0x6e41,0x6279,0x7375, 0};
static APPL_AdiNotMappableType appl_sAdi3 = { { 0x6e41,0x6279,0x7375,  0}, { 0x0201, 0x0403, 0x0605, 0x0807, 0x0a09, 0, 0, 0, 0, 0 } };
static UINT8                   appl_bUint8Array[ 2 ] = { 0x3505, 0x1515 };
#else
static char appl_acString[] = "Anybus";
static APPL_AdiNotMappableType appl_sAdi3 = { "Anybus", { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 } };
static UINT8                   appl_bUint8Array[ 4 ] = { 0x05, 0x35, 0x15, 0x15 };
#endif

static AD_UINT32Type  appl_sUint32Prop  = { { 1, 0xFFFFFFF0, 0x12345678 } };
static AD_SINT32Type  appl_sSint32Prop  = { { -0x000000FF, 0x0FFFFFFF, 0x07654321 } };
static AD_UINT16Type  appl_sUint16Prop  = { { 2, 0xFFF0, 0x1234 } };
static AD_SINT16Type  appl_sSint16Prop  = { { -0x00FF, 0x0FFF, 0x0765 } };
static AD_UINT8Type   appl_sUint8Prop   = { { 0x05, 0xF0, 0x15 } };
static AD_SINT8Type   appl_sSint8Prop   = { { -0x0F, 0x7E, 0x62 } };
static AD_FLOAT32Type appl_sFloat32Prop = { { -43.454322814941406, 345.83782958984375, 59.09347152709961 } };
static AD_OctetType   appl_sOctetProp   = { { 0x05, 0xF0, 0x05 } };
static AD_ENUMType    appl_sEnumProp    = { { 2, 7, 5}, 3, appl_asEnumStrings };

/*
** Structured ADI #1 (AD_StructDataType):
**-------------------------------------------------------------------------------------------------
** | 1. pacElementName | 2. bDataType | 3. iNumSubElem | 4. bDesc | 5. bBitOffset | 6. pxValuePtr | 7. pxValuePropPtr |
**-------------------------------------------------------------------------------------------------
*/
static const AD_StructDataType appl_Adi1Struct[] =
{
   /* Index: 0 */  { "A", ABP_UINT32, 1, APPL_ALL_ACCESS_DESC,  0,  { { &appl_sAdi1.lUint32,        NULL } } },
   /* Index: 1 */  { "B", ABP_SINT32, 1, APPL_ALL_ACCESS_DESC,  0,  { { &appl_sAdi1.lInt32,         NULL } } },
   /* Index: 2 */  { "C", ABP_UINT16, 1, APPL_ALL_ACCESS_DESC,  0,  { { &appl_sAdi1.iUint16,        NULL } } },
   /* Index: 3 */  { "D", ABP_SINT16, 1, APPL_ALL_ACCESS_DESC,  0,  { { &appl_sAdi1.iInt16,         NULL } } },
   /* Index: 4 */  { "E", ABP_BITS16, 1, APPL_ALL_ACCESS_DESC,  0,  { { &appl_sAdi1.iBit16,         NULL } } },
   /* Index: 5 */  { "F", ABP_UINT8,  1, APPL_ALL_ACCESS_DESC,  0,  { { &appl_sAdi1.bUint8,         NULL } } },
   /* Index: 6 */  { "G", ABP_SINT8,  1, APPL_ALL_ACCESS_DESC,  0,  { { &appl_sAdi1.bInt8,          NULL } } },
   /* Index: 7 */  { "H", ABP_BITS8,  1, APPL_ALL_ACCESS_DESC,  0,  { { &appl_sAdi1.bBit8,          NULL } } },
   /* Index: 8 */  { "I", ABP_PAD8,   1, APPL_ALL_ACCESS_DESC,  0,  { { NULL,                       NULL } } },
#ifdef ABCC_SYS_16_BIT_CHAR
   /* Index: 9 */  { "J", ABP_BIT1,   1, APPL_ALL_ACCESS_DESC,  0,  { { &appl_sAdi1.iBitTypes[ 0 ], NULL } } },
   /* Index: 10 */ { "K", ABP_BIT2,   1, APPL_ALL_ACCESS_DESC,  1,  { { &appl_sAdi1.iBitTypes[ 0 ], NULL } } },
   /* Index: 11 */ { "L", ABP_BIT3,   1, APPL_ALL_ACCESS_DESC,  3,  { { &appl_sAdi1.iBitTypes[ 0 ], NULL } } },
   /* Index: 12 */ { "M", ABP_BIT4,   1, APPL_ALL_ACCESS_DESC,  6,  { { &appl_sAdi1.iBitTypes[ 0 ], NULL } } },
   /* Index: 13 */ { "N", ABP_BIT5,   1, APPL_ALL_ACCESS_DESC,  10, { { &appl_sAdi1.iBitTypes[ 0 ], NULL } } },
   /* Index: 14 */ { "O", ABP_BIT6,   1, APPL_ALL_ACCESS_DESC,  15, { { &appl_sAdi1.iBitTypes[ 0 ], NULL } } },
   /* Index: 15 */ { "P", ABP_BIT7,   1, APPL_ALL_ACCESS_DESC,  5,  { { &appl_sAdi1.iBitTypes[ 1 ], NULL } } },
   /* Index: 16 */ { "Q", ABP_PAD4,   1, APPL_ALL_ACCESS_DESC,  4,  { { NULL,                       NULL } } }
#else
   /* Index: 9 */  { "J", ABP_BIT1,   1, APPL_ALL_ACCESS_DESC,  0,  { { &appl_sAdi1.bBitTypes[ 0 ], NULL } } },
   /* Index: 10 */ { "K", ABP_BIT2,   1, APPL_ALL_ACCESS_DESC,  1,  { { &appl_sAdi1.bBitTypes[ 0 ], NULL } } },
   /* Index: 11 */ { "L", ABP_BIT3,   1, APPL_ALL_ACCESS_DESC,  3,  { { &appl_sAdi1.bBitTypes[ 0 ], NULL } } },
   /* Index: 12 */ { "M", ABP_BIT4,   1, APPL_ALL_ACCESS_DESC,  6,  { { &appl_sAdi1.bBitTypes[ 0 ], NULL } } },
   /* Index: 13 */ { "N", ABP_BIT5,   1, APPL_ALL_ACCESS_DESC,  2,  { { &appl_sAdi1.bBitTypes[ 1 ], NULL } } },
   /* Index: 14 */ { "O", ABP_BIT6,   1, APPL_ALL_ACCESS_DESC,  7,  { { &appl_sAdi1.bBitTypes[ 1 ], NULL } } },
   /* Index: 15 */ { "P", ABP_BIT7,   1, APPL_ALL_ACCESS_DESC,  5,  { { &appl_sAdi1.bBitTypes[ 2 ], NULL } } },
   /* Index: 16 */ { "Q", ABP_PAD4,   1, APPL_ALL_ACCESS_DESC,  4,  { { NULL ,                      NULL } } }
#endif
};

/*
** Structured ADI #2 (AD_StructDataType):
**-------------------------------------------------------------------------------------------------
** | 1. pacElementName | 2. bDataType | 3. iNumSubElem | 4. bDesc | 5. bBitOffset | 6. pxValuePtr | 7. pxValuePropPtr |
**-------------------------------------------------------------------------------------------------
*/
static const AD_StructDataType appl_Adi2Struct[] =
{
   /* Index: 0 */  { "A", ABP_UINT32, 1, APPL_ALL_ACCESS_DESC, 0,  { { &appl_sAdi2.lUint32,        NULL } } },
   /* Index: 1 */  { "B", ABP_SINT32, 1, APPL_ALL_ACCESS_DESC, 0,  { { &appl_sAdi2.lInt32,         NULL } } },
   /* Index: 2 */  { "C", ABP_UINT16, 1, APPL_ALL_ACCESS_DESC, 0,  { { &appl_sAdi2.iUint16,        NULL } } },
   /* Index: 3 */  { "D", ABP_SINT16, 1, APPL_ALL_ACCESS_DESC, 0,  { { &appl_sAdi2.iInt16,         NULL } } },
   /* Index: 4 */  { "E", ABP_BITS16, 1, APPL_ALL_ACCESS_DESC, 0,  { { &appl_sAdi2.iBit16,         NULL } } },
   /* Index: 5 */  { "F", ABP_UINT8,  1, APPL_ALL_ACCESS_DESC, 0,  { { &appl_sAdi2.bInt8,          NULL } } },
   /* Index: 6 */  { "G", ABP_SINT8,  1, APPL_ALL_ACCESS_DESC, 0,  { { &appl_sAdi2.bUint8,         NULL } } },
   /* Index: 7 */  { "H", ABP_BITS8,  1, APPL_ALL_ACCESS_DESC, 0,  { { &appl_sAdi2.bBit8,          NULL } } },
   /* Index: 8 */  { "I", ABP_PAD8,   1, APPL_ALL_ACCESS_DESC, 0,  { { NULL,                      NULL } } },
#ifdef ABCC_SYS_16_BIT_CHAR
   /* Index: 9 */  { "J", ABP_BIT1,   1, APPL_ALL_ACCESS_DESC, 0,  { { &appl_sAdi2.iBitTypes[ 0 ], NULL } } },
   /* Index: 10 */ { "K", ABP_BIT2,   1, APPL_ALL_ACCESS_DESC, 1,  { { &appl_sAdi2.iBitTypes[ 0 ], NULL } } },
   /* Index: 11 */ { "L", ABP_BIT3,   1, APPL_ALL_ACCESS_DESC, 3,  { { &appl_sAdi2.iBitTypes[ 0 ], NULL } } },
   /* Index: 12 */ { "M", ABP_BIT4,   1, APPL_ALL_ACCESS_DESC, 6,  { { &appl_sAdi2.iBitTypes[ 0 ], NULL } } },
   /* Index: 13 */ { "N", ABP_BIT5,   1, APPL_ALL_ACCESS_DESC, 10, { { &appl_sAdi2.iBitTypes[ 0 ], NULL } } },
   /* Index: 14 */ { "O", ABP_BIT6,   1, APPL_ALL_ACCESS_DESC, 15, { { &appl_sAdi2.iBitTypes[ 0 ], NULL } } },
   /* Index: 15 */ { "P", ABP_BIT7,   1, APPL_ALL_ACCESS_DESC, 5,  { { &appl_sAdi2.iBitTypes[ 1 ], NULL } } },
   /* Index: 16 */ { "Q", ABP_PAD4,   1, APPL_ALL_ACCESS_DESC, 4,  { { NULL,                      NULL } } }
#else
   /* Index: 9 */  { "J", ABP_BIT1,   1, APPL_ALL_ACCESS_DESC, 0,  { { &appl_sAdi2.bBitTypes[ 0 ], NULL } } },
   /* Index: 10 */ { "K", ABP_BIT2,   1, APPL_ALL_ACCESS_DESC, 1,  { { &appl_sAdi2.bBitTypes[ 0 ], NULL } } },
   /* Index: 11 */ { "L", ABP_BIT3,   1, APPL_ALL_ACCESS_DESC, 3,  { { &appl_sAdi2.bBitTypes[ 0 ], NULL } } },
   /* Index: 12 */ { "M", ABP_BIT4,   1, APPL_ALL_ACCESS_DESC, 6,  { { &appl_sAdi2.bBitTypes[ 0 ], NULL } } },
   /* Index: 13 */ { "N", ABP_BIT5,   1, APPL_ALL_ACCESS_DESC, 2,  { { &appl_sAdi2.bBitTypes[ 1 ], NULL } } },
   /* Index: 14 */ { "O", ABP_BIT6,   1, APPL_ALL_ACCESS_DESC, 7,  { { &appl_sAdi2.bBitTypes[ 1 ], NULL } } },
   /* Index: 15 */ { "P", ABP_BIT7,   1, APPL_ALL_ACCESS_DESC, 5,  { { &appl_sAdi2.bBitTypes[ 2 ], NULL } } },
   /* Index: 16 */ { "Q", ABP_PAD4,   1, APPL_ALL_ACCESS_DESC, 4,  { { NULL ,                     NULL } } }
#endif
};

/*
** Structured ADI #3 (AD_StructDataType):
**-------------------------------------------------------------------------------------------------
** | 1. pacElementName | 2. bDataType | 3. iNumSubElem | 4. bDesc | 5. bBitOffset | 6. pxValuePtr | 7. pxValuePropPtr |
**-------------------------------------------------------------------------------------------------
*/
static const AD_StructDataType appl_Adi3Struct[] =
{
   /* Index: 0 */  { "A", ABP_CHAR,  6,  APPL_ALL_ACCESS_DESC, 0,  { { appl_sAdi3.appl_acString,  NULL } } },
   /* Index: 1 */  { "B", ABP_OCTET, 10, APPL_ALL_ACCESS_DESC, 0,  { { &appl_sAdi3.bOctet,      NULL } } }
};

/*
** Structured ADI #4 (AD_StructDataType):
**-------------------------------------------------------------------------------------------------
** | 1. pacElementName | 2. bDataType | 3. iNumSubElem | 4. bDesc | 5. bBitOffset | 6. pxValuePtr | 7. pxValuePropPtr |
**-------------------------------------------------------------------------------------------------
*/
static const AD_StructDataType appl_Adi4Struct[] =
{
   /* Index: 0 */  { "A", ABP_UINT8,  1, APPL_ALL_ACCESS_DESC, 0, { { &appl_sAdi4.bUint8,  &appl_sUint8Prop   } } },
   /* Index: 1 */  { "B", ABP_SINT8,  1, APPL_ALL_ACCESS_DESC, 0, { { &appl_sAdi4.bInt8,   &appl_sSint8Prop   } } },
   /* Index: 2 */  { "C", ABP_UINT16, 1, APPL_ALL_ACCESS_DESC, 0, { { &appl_sAdi4.iUint16, &appl_sUint16Prop  } } },
   /* Index: 3 */  { "D", ABP_SINT16, 1, APPL_ALL_ACCESS_DESC, 0, { { &appl_sAdi4.iInt16,  &appl_sSint16Prop  } } },
   /* Index: 4 */  { "E", ABP_UINT32, 1, APPL_ALL_ACCESS_DESC, 0, { { &appl_sAdi4.lUint32, &appl_sUint32Prop  } } },
   /* Index: 5 */  { "F", ABP_SINT32, 1, APPL_ALL_ACCESS_DESC, 0, { { &appl_sAdi4.lInt32,  &appl_sSint32Prop  } } },
   /* Index: 6 */  { "G", ABP_FLOAT,  1, APPL_ALL_ACCESS_DESC, 0, { { &appl_sAdi4.rFloat,  &appl_sFloat32Prop } } },
   /* Index: 7 */  { "H", ABP_OCTET,  1, APPL_ALL_ACCESS_DESC, 0, { { &appl_sAdi4.bOctet,  &appl_sOctetProp   } } },
   /* Index: 8 */  { "I", ABP_ENUM,   1, APPL_ALL_ACCESS_DESC, 0, { { &appl_sAdi4.eEnum,   &appl_sEnumProp    } } }
};

/*
** Structured ADI #5 (AD_StructDataType):
**-------------------------------------------------------------------------------------------------
** | 1. pacElementName | 2. bDataType | 3. iNumSubElem | 4. bDesc | 5. bBitOffset | 6. pxValuePtr | 7. pxValuePropPtr |
**-------------------------------------------------------------------------------------------------
*/
static const AD_StructDataType appl_Adi5Struct[] =
{
   /* Index: 0 */  { "A",  ABP_UINT8,  1, APPL_ALL_ACCESS_DESC, 0, { { &appl_sAdi5.bUint8,  NULL } } },
   /* Index: 1 */  { "B",  ABP_SINT8,  1, APPL_ALL_ACCESS_DESC, 0, { { &appl_sAdi5.bInt8,   NULL } } },
   /* Index: 2 */  { "",   ABP_UINT16, 1, APPL_ALL_ACCESS_DESC, 0, { { &appl_sAdi5.iUint16, NULL } } },
   /* Index: 3 */  { "D",  ABP_SINT16, 1, APPL_ALL_ACCESS_DESC, 0, { { &appl_sAdi5.iInt16,  NULL } } },
   /* Index: 4 */  { NULL, ABP_UINT32, 1, APPL_ALL_ACCESS_DESC, 0, { { &appl_sAdi5.lUint32, NULL } } },
   /* Index: 5 */  { NULL, ABP_SINT32, 1, APPL_ALL_ACCESS_DESC, 0, { { &appl_sAdi5.lInt32,  NULL } } },
   /* Index: 6 */  { "G",  ABP_FLOAT,  1, APPL_ALL_ACCESS_DESC, 0, { { &appl_sAdi5.rFloat,  NULL } } },
   /* Index: 7 */  { "H",  ABP_OCTET,  1, APPL_ALL_ACCESS_DESC, 0, { { &appl_sAdi5.bOctet,  NULL } } },
   /* Index: 8 */  { "I",  ABP_ENUM,   1, APPL_ALL_ACCESS_DESC, 0, { { &appl_sAdi5.eEnum,   NULL } } }
};

/*
** Structured ADI #6 (AD_StructDataType):
**-------------------------------------------------------------------------------------------------
** | 1. pacElementName | 2. bDataType | 3. iNumSubElem | 4. bDesc | 5. bBitOffset | 6. pxValuePtr | 7. pxValuePropPtr |
**-------------------------------------------------------------------------------------------------
*/
static const AD_StructDataType appl_Adi6Struct[] =
{
   /* Index: 0 */  { "A", ABP_UINT8,  1, APPL_ALL_ACCESS_DESC, 0, { { &appl_sAdi6.bUint8,  &appl_sUint8Prop   } } },
   /* Index: 1 */  { "B", ABP_SINT8,  1, APPL_ALL_ACCESS_DESC, 0, { { &appl_sAdi6.bInt8,   &appl_sSint8Prop   } } },
   /* Index: 2 */  { "C", ABP_UINT16, 1, APPL_ALL_ACCESS_DESC, 0, { { &appl_sAdi6.iUint16, &appl_sUint16Prop  } } },
   /* Index: 3 */  { "D", ABP_SINT16, 1, APPL_ALL_ACCESS_DESC, 0, { { &appl_sAdi6.iInt16,  NULL               } } },
   /* Index: 4 */  { "E", ABP_UINT32, 1, APPL_ALL_ACCESS_DESC, 0, { { &appl_sAdi6.lUint32, &appl_sUint32Prop  } } },
   /* Index: 5 */  { "F", ABP_SINT32, 1, APPL_ALL_ACCESS_DESC, 0, { { &appl_sAdi6.lInt32,  &appl_sSint32Prop  } } },
   /* Index: 6 */  { "G", ABP_FLOAT,  1, APPL_ALL_ACCESS_DESC, 0, { { &appl_sAdi6.rFloat,  NULL               } } },
   /* Index: 7 */  { "H", ABP_OCTET,  1, APPL_ALL_ACCESS_DESC, 0, { { &appl_sAdi6.bOctet,  &appl_sOctetProp   } } },
   /* Index: 8 */  { "I", ABP_ENUM,   1, APPL_ALL_ACCESS_DESC, 0, { { &appl_sAdi6.eEnum,   &appl_sEnumProp    } } }
};

/*
** Structured ADI #7 (AD_StructDataType):
**-------------------------------------------------------------------------------------------------
** | 1. bDataType | 2. iNumSubElem | 3. bDesc | 4. bBitOffset | 5. pxValuePtr | 6. pxValuePropPtr |
**-------------------------------------------------------------------------------------------------
*/
static const AD_StructDataType appl_Adi7Struct[] =
{
   /* Index: 0 */  { NULL, ABP_UINT8, 1, ABP_APPD_DESCR_GET_ACCESS,      0, { { &appl_sAdi7[ 0 ], NULL } } },
   /* Index: 1 */  { NULL, ABP_UINT8, 1, ABP_APPD_DESCR_SET_ACCESS,      0, { { &appl_sAdi7[ 0 ], NULL } } },
   /* Index: 2 */  { NULL, ABP_UINT8, 1, APPL_NOT_MAP_WRITE_ACCESS_DESC, 0, { { &appl_sAdi7[ 1 ], NULL } } }
};

/*******************************************************************************
** Public Globals
********************************************************************************
*/

/*
** The ADI entry list (AD_AdiEntryType):
** ----------------------------------------------------------------------------------------------------------------------------
** | 1. iInstance | 2. pabName | 3. bDataType | 4. bNumOfElements | 5. bDesc | 6.pxValuePtr | 7. pxValuePropPtr | 8. psStruct |
** ----------------------------------------------------------------------------------------------------------------------------
*/
const AD_AdiEntryType APPL_asAdiEntryList[] =
{
   { 0x1,  "ABP_UINT32",     ABP_UINT32,  1,  APPL_ALL_ACCESS_DESC, { { &appl_lUint32,     &appl_sUint32Prop } },     NULL },
   { 0x2,  "ABP_SINT32",     ABP_SINT32,  1,  APPL_ALL_ACCESS_DESC, { { &appl_lInt32,      &appl_sSint32Prop } },     NULL },
   { 0x3,  "ABP_UINT16",     ABP_UINT16,  1,  APPL_ALL_ACCESS_DESC, { { &appl_iUint16,     &appl_sUint16Prop } },     NULL },
   { 0x4,  "ABP_SINT16",     ABP_SINT16,  1,  APPL_ALL_ACCESS_DESC, { { &appl_iInt16,      &appl_sSint16Prop } },     NULL },
   { 0x5,  "ABP_BITS16",     ABP_BITS16,  1,  APPL_ALL_ACCESS_DESC, { { &appl_iBit16,      NULL } },                  NULL },
   { 0x6,  "ABP_UINT8",      ABP_UINT8,   1,  APPL_ALL_ACCESS_DESC, { { &appl_bUint8,      &appl_sUint8Prop } },      NULL },
   { 0x7,  "ABP_SINT8",      ABP_SINT8,   1,  APPL_ALL_ACCESS_DESC, { { &appl_bInt8,       &appl_sSint8Prop } },      NULL },
   { 0x8,  "ABP_UINT8",      ABP_PAD8,    1,  APPL_ALL_ACCESS_DESC, { { NULL,              NULL } },                  NULL },
   { 0x9,  "ABP_BIT7",       ABP_BIT7,    1,  APPL_ALL_ACCESS_DESC, { { &appl_bBit8,       NULL } },                  NULL },
   { 0x60, "Struct1",        DONT_CARE,   17, APPL_ALL_ACCESS_DESC, { { NULL,              NULL } },                  appl_Adi1Struct },
   { 0x70, "Struct2",        DONT_CARE,   17, APPL_ALL_ACCESS_DESC, { { NULL,              NULL } },                  appl_Adi2Struct },
   { 0x80, "Enum"   ,        ABP_ENUM,    1,  APPL_ALL_ACCESS_DESC, { { &appl_eEnum,       &appl_sEnumProp } },       NULL },
   { 0x81, "Octet_no_map" ,  ABP_OCTET,   1,  APPL_ALL_ACCESS_DESC, { { &appl_bOctet,      &appl_sOctetProp } },      NULL },
   { 0x82, "String_no_map",  ABP_CHAR,    6,  APPL_ALL_ACCESS_DESC, { { appl_acString,     NULL } },                  NULL },
   { 0x84, "Struct3_no_map", DONT_CARE,   2,  APPL_ALL_ACCESS_DESC, { { NULL,              NULL } },                  appl_Adi3Struct },
   { 0x85, "Bit2Array",      ABP_BIT2,    4,  APPL_ALL_ACCESS_DESC, { { &appl_bBit2Array,  NULL } },                  NULL },
   { 0x86, "Float"    ,      ABP_FLOAT,   1,  APPL_ALL_ACCESS_DESC, { { &appl_rFloat,      &appl_sFloat32Prop } },    NULL },
   { 0x90, "UINT8Array",     ABP_UINT8,   4,  APPL_ALL_ACCESS_DESC, { { appl_bUint8Array,  &appl_sUint8Prop } },      NULL },
   { 0x91, "Struct4",        DONT_CARE,   9,  APPL_ALL_ACCESS_DESC, { { NULL,              NULL } },                  appl_Adi4Struct },
   { 0x92, "Struct5",        DONT_CARE,   9,  APPL_ALL_ACCESS_DESC, { { NULL,              NULL } },                  appl_Adi5Struct },
   { 0x93, "Struct6",        DONT_CARE,   9,  APPL_ALL_ACCESS_DESC, { { NULL,              NULL } },                  appl_Adi6Struct },
   { 0x94, "Struct7",        DONT_CARE,   3,  APPL_ALL_ACCESS_DESC, { { NULL,              NULL } },                  appl_Adi7Struct }
};


/*
** Default map (AD_DefaultMapType):
**-------------------------------------------------------------
** | 1. AdiIndex | 2. Direction | 3. NumElem  | 4. StartIndex |
**-------------------------------------------------------------
*/
const AD_DefaultMapType APPL_asAdObjDefaultMap[] =
{
   { 0x60,  PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0  },
   { 0x60,  PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0  },
   { 0x70, PD_WRITE, 1,                       8  },
   { 0x70, PD_WRITE, 1,                       7  },
   { 0x70, PD_WRITE, 1,                       6  },
   { 0x70, PD_WRITE, 1,                       5  },
   { 0x70, PD_WRITE, 1,                       4  },
   { 0x70, PD_WRITE, 1,                       3  },
   { 0x70, PD_WRITE, 1,                       2  },
   { 0x70, PD_WRITE, 1,                       1  },
   { 0x70, PD_WRITE, 1,                       0  },
   { 0x70, PD_WRITE, 1,                       9  },
   { 0x70, PD_WRITE, 1,                       11 },
   { 0x70, PD_WRITE, 1,                       13 },
   { 0x70, PD_WRITE, 1,                       15 },
   { 0x70, PD_WRITE, 1,                       10 },
   { 0x70, PD_WRITE, 1,                       12 },
   { 0x70, PD_WRITE, 1,                       14 },
   { 0x70, PD_WRITE, 1,                       16 },
   { 0x70, PD_READ,  1,                       0  },
   { 0x70, PD_READ,  1,                       1  },
   { 0x70, PD_READ,  1,                       2  },
   { 0x70, PD_READ,  1,                       3  },
   { 0x70, PD_READ,  1,                       4  },
   { 0x70, PD_READ,  1,                       5  },
   { 0x70, PD_READ,  1,                       6  },
   { 0x70, PD_READ,  1,                       7  },
   { 0x70, PD_READ,  1,                       8  },
   { 0x70, PD_READ,  1,                       9  },
   { 0x70, PD_READ,  1,                       10 },
   { 0x70, PD_READ,  1,                       11 },
   { 0x70, PD_READ,  1,                       12 },
   { 0x70, PD_READ,  1,                       13 },
   { 0x70, PD_READ,  1,                       14 },
   { 0x70, PD_READ,  1,                       15 },
   { 0x70, PD_READ,  1,                       16 },
   { 9, PD_WRITE,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
   { AD_MAP_PAD_ADI, PD_WRITE, 1,          0 },
   { 8, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
   { 7, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
   { 6, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
   { 5, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
   { 4, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
   { 3, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
   { 2, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
   { 1, PD_WRITE, AD_DEFAULT_MAP_ALL_ELEM, 0 },
   { 1, PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
   { 2, PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
   { 3, PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
   { 4, PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
   { 5, PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
   { 6, PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
   { 7, PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
   { 8, PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
   { 9, PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
   { AD_MAP_PAD_ADI, PD_READ, 1,       0 },

   { 0x85, PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
   { 0x86, PD_READ,  AD_DEFAULT_MAP_ALL_ELEM, 0 },

   { 0x86, PD_WRITE,  AD_DEFAULT_MAP_ALL_ELEM, 0 },
   { 0x85, PD_WRITE,  AD_DEFAULT_MAP_ALL_ELEM, 0 },

/*
** This mapping currently does not work in AD object.
*/
   /*
   { 0x85, PD_READ,  1, 0 },
   { 0x85, PD_READ,  1, 1 },
   { 0x85, PD_READ,  1, 2 },
   { 0x85, PD_READ,  1, 3 },

   { 0x85, PD_WRITE,  1, 3 },
   { 0x85, PD_WRITE,  1, 2 },
   { 0x85, PD_WRITE,  1, 1 },
   { 0x85, PD_WRITE,  1, 0 },
*/
   { AD_DEFAULT_MAP_END_ENTRY }
};

/*******************************************************************************
** Private Services
********************************************************************************
*/

/*******************************************************************************
** Public Services
********************************************************************************
*/

UINT16 APPL_GetNumAdi( void )
{
   return( sizeof( APPL_asAdiEntryList ) / sizeof( AD_AdiEntryType ) );
}

void APPL_CyclicalProcessing( void )
{
   static BOOL fStarted = FALSE;

   /*
   ** Set default values the first time this function is called
   */
   if( !fStarted )
   {
      appl_lUint32 = appl_sUint32Prop.lMinMaxDefault[ AD_DEFAULT_VALUE_INDEX ];
      appl_lInt32 =  appl_sSint32Prop.lMinMaxDefault[ AD_DEFAULT_VALUE_INDEX ];
      appl_iUint16 = appl_sUint16Prop.iMinMaxDefault[ AD_DEFAULT_VALUE_INDEX ];
      appl_iInt16 = appl_sSint16Prop.iMinMaxDefault[ AD_DEFAULT_VALUE_INDEX ];
      appl_bUint8 = appl_sUint8Prop.bMinMaxDefault[ AD_DEFAULT_VALUE_INDEX ];
      appl_bInt8 = appl_sSint8Prop.bMinMaxDefault[ AD_DEFAULT_VALUE_INDEX ];
      appl_bOctet = appl_sOctetProp.bMinMaxDefault[ AD_DEFAULT_VALUE_INDEX ];
      appl_eEnum = appl_sEnumProp.eMinMaxDefault[ AD_DEFAULT_VALUE_INDEX ];
      appl_rFloat = appl_sFloat32Prop.rMinMaxDefault[ AD_DEFAULT_VALUE_INDEX ];
      fStarted = TRUE;
   }
}

/*******************************************************************************
** Tasks
********************************************************************************
*/

#endif
