/*
 * Copyright (c) 2004 by FORCE Computers.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Thomas Kanngieser <thomas.kanngieser@fci.com>
 */

#include "marshal.h"
#include <string.h>


#define dModU8  0
#define dModU16 1
#define dModU32 2
#define dModU64 3
#define dModI8  4
#define dModI16 5
#define dModI32 6
#define dModI64 7
#define dModF32 8
#define dModF64 9


typedef union
{
  tUint8   m_u8;
  tUint16  m_u16;
  tUint32  m_u32;
  tUint64  m_u64;
  tInt8    m_i8;
  tInt16   m_i16;
  tInt32   m_i32;
  tInt64   m_i64;
  tFloat32 m_f32;
  tFloat64 m_f64;
} cUnion;


typedef struct
{
  tUint8 m_pad1;
  tUint8 m_mod;
  tUint8 m_pad2;
  cUnion m_union;
  tUint8 m_pad3;
} cTest;


cMarshalType UnionElements[] =
{
  dUnionElement( dModU8 , Uint8Type ),
  dUnionElement( dModU16, Uint16Type ),
  dUnionElement( dModU32, Uint32Type ),
  dUnionElement( dModU64, Uint64Type ),
  dUnionElement( dModI8 , Int8Type ),
  dUnionElement( dModI16, Int16Type ),
  dUnionElement( dModI32, Int32Type ),
  dUnionElement( dModI64, Int64Type ),
  dUnionElement( dModF32, Float32Type ),
  dUnionElement( dModF64, Float64Type ),
  dUnionElementEnd()
};


cMarshalType TestUnionType = dUnion( 1, cUnion, UnionElements );


cMarshalType StructElements[] =
{
  dStructElement( cTest, m_pad1 , Uint8Type ),
  dStructElement( cTest, m_mod  , Uint8Type ),
  dStructElement( cTest, m_pad2 , Uint8Type ),
  dStructElement( cTest, m_union, TestUnionType ),
  dStructElement( cTest, m_pad3 , Uint8Type ),
  dStructElementEnd()
};


cMarshalType TestType = dStruct( cTest, StructElements );


int
main( int argc, char *argv[] )
{
  cTest value =
  {
    .m_pad1 = 47,
    .m_mod  = dModU64,
    .m_pad2 = 48,
    .m_union.m_u64 = 0xabcd123456780123LL,
    .m_pad3 = 49
  };

  unsigned char  buffer[256];
  cTest          result;

  unsigned int s1 = Marshal( &TestType, &value, buffer );
  unsigned int s2 = Demarshal( MarshalByteOrder(), &TestType, &result, buffer );

  if ( s1 != s2 )
       return 1;

  if ( value.m_pad1 != result.m_pad1 )
       return 1;

  if ( value.m_mod != result.m_mod )
       return 1;

  if ( value.m_pad2 != result.m_pad2 )
       return 1;

  if ( value.m_pad3 != result.m_pad3 )
       return 1;

  if ( value.m_union.m_u64 != result.m_union.m_u64 )
       return 1;

  return 0;
}
