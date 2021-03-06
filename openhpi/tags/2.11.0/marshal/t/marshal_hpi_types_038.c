/*
 * Copyright (c) 2005 by IBM Corporation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     W. David Ashley <dashley@us.ibm.com>
 */

#include "marshal_hpi_types.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


static int
cmp_text_buffer( SaHpiTextBufferT *d1, SaHpiTextBufferT *d2 )
{
  if ( d1->DataType != d2->DataType )
       return 0;

  if ( d1->Language != d2->Language )
       return 0;

  if ( d1->DataLength != d2->DataLength )
       return 0;

  return memcmp( d1->Data, d2->Data, d1->DataLength ) ? 0 : 1;
}


static int
cmp_swevent( SaHpiHpiSwEventT *d1, SaHpiHpiSwEventT *d2 )
{
  if ( d1->MId != d2->MId )
       return 0;

  if ( d1->Type != d2->Type )
       return 0;

  if ( !cmp_text_buffer( &d1->EventData, &d2->EventData ) )
       return 0;

  return 1;
}


typedef struct
{
  tUint8 m_pad1;
  SaHpiHpiSwEventT m_v1;
  tUint8 m_pad2;
  SaHpiHpiSwEventT m_v2;
  SaHpiHpiSwEventT m_v3;
  tUint8 m_pad3;
} cTest;

cMarshalType StructElements[] =
{
  dStructElement( cTest, m_pad1 , Marshal_Uint8Type ),
  dStructElement( cTest, m_v1   , SaHpiHpiSwEventType ),
  dStructElement( cTest, m_pad2 , Marshal_Uint8Type ),
  dStructElement( cTest, m_v2   , SaHpiHpiSwEventType ),
  dStructElement( cTest, m_v3   , SaHpiHpiSwEventType ),
  dStructElement( cTest, m_pad3 , Marshal_Uint8Type ),
  dStructElementEnd()
};

cMarshalType TestType = dStruct( cTest, StructElements );


int
main( int argc, char *argv[] )
{
  cTest value =
  {
    .m_pad1                            = 47,
    .m_v1.MId                          = 1,
    .m_v1.Type                         = SAHPI_HPIE_AUDIT,
    .m_v1.EventData.DataType           = SAHPI_TL_TYPE_BINARY,
    .m_v1.EventData.Language           = SAHPI_LANG_TSONGA,
    .m_v1.EventData.DataLength         = 3,
    .m_v1.EventData.Data               = "AB",
    .m_pad2                            = 48,
    .m_v2.MId                          = 2,
    .m_v2.Type                         = SAHPI_HPIE_STARTUP,
    .m_v2.EventData.DataType           = SAHPI_TL_TYPE_BCDPLUS,
    .m_v2.EventData.Language           = SAHPI_LANG_SANGRO,
    .m_v2.EventData.DataLength         = 21,
    .m_v2.EventData.Data               = "12345678901234567890",
    .m_v3.MId                          = 3,
    .m_v3.Type                         = SAHPI_HPIE_OTHER,
    .m_v3.EventData.DataType           = SAHPI_TL_TYPE_ASCII6,
    .m_v3.EventData.Language           = SAHPI_LANG_TAJIK,
    .m_v3.EventData.DataLength         = 0,
    .m_v3.EventData.Data               = "",
    .m_pad3                            = 49
  };

  unsigned char *buffer = (unsigned char *)malloc(sizeof(value));
  cTest          result;

  unsigned int s1 = Marshal( &TestType, &value, buffer );
  unsigned int s2 = Demarshal( MarshalByteOrder(), &TestType, &result, buffer );

  if ( s1 != s2 )
       return 1;

  if ( value.m_pad1 != result.m_pad1 )
       return 1;

  if ( !cmp_swevent( &value.m_v1, &result.m_v1 ) )
       return 1;

  if ( value.m_pad2 != result.m_pad2 )
       return 1;

  if ( !cmp_swevent( &value.m_v2, &result.m_v2 ) )
       return 1;

  if ( !cmp_swevent( &value.m_v3, &result.m_v3 ) )
       return 1;

  if ( value.m_pad3 != result.m_pad3 )
       return 1;

  return 0;
}
