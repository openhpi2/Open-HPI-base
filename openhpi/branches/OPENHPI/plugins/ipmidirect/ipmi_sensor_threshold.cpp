/*
 * ipmi_sensor_threshold.cpp
 *
 * Copyright (c) 2004 by FORCE Computers
 *
 * Note that this file is based on parts of OpenIPMI
 * written by Corey Minyard <minyard@mvista.com>
 * of MontaVista Software. Corey's code was helpful
 * and many thanks go to him. He gave the permission
 * to use this code in OpenHPI under BSD license.
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

#include "ipmi_sensor_threshold.h"
#include "ipmi_log.h"
#include "ipmi_domain.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <glib.h>


static const char *hysteresis_support_types[] =
{
    "none",
    "readable",
    "settable",
    "fixed",
};


const char *
IpmiHysteresisSupportToString( tIpmiHysteresisSupport val )
{
  if ( val > eIpmiHysteresisSupportFixed )
       return "invalid";
 
  return hysteresis_support_types[val];
}


static const char *threshold_access_support_types[] =
{
    "none",
    "readable",
    "settable",
    "fixed",
};


const char *
IpmiThresholdAccessSupportToString( tIpmiThresholdAccessSuport val )
{
  if ( val > eIpmiThresholdAccessSupportFixed )
       return "invalid";

  return threshold_access_support_types[val];
}


void
IpmiThresholdEventMaskToString( unsigned short em, char *str )
{
  *str = 0;

  if ( em & dIpmiEventLowerNonCriticalLow )
       strcat( str, " lower-non-critical-low" );

  if ( em & dIpmiEventLowerNonCriticalHigh )
       strcat( str, " lower-non-critical-high" );

  if ( em & dIpmiEventLowerCriticalLow )
       strcat( str, " lower-critical-low" );

  if ( em & dIpmiEventLowerCriticalHigh )
       strcat( str, " lower-critical-high" );

  if ( em & dIpmiEventLowerNonRecoverableLow )
       strcat( str, " lower-non-recoverable-low" );

  if ( em & dIpmiEventLowerNonRecoverableHigh )
       strcat( str, " lower-non-recoverable-high" );

  if ( em & dIpmiEventUpperNonCriticalLow )
       strcat( str, " upper-non-critical-low" );

  if ( em & dIpmiEventUpperCriticalHigh )
       strcat( str, " upper-critical-high" );

  if ( em & dIpmiEventUpperNonRecoverableLow )
       strcat( str, " upper-non-recoverable-low" );

  if ( em & dIpmiEventUpperNonRecoverableHigh )
       strcat( str, " upper-non-recoverable-high" );
}


static const char *rate_unit[] =
{
  "None",
  "Us"
  "ms"
  "s",
  "minute",
  "hour",
  "day"
};

#define dNumRateUnit (sizeof(rate_unit)/sizeof(char *))

const char *
IpmiRateUnitToString( tIpmiRateUnit val )
{
  if ( val > (int)dNumRateUnit )
       return "Invalid";

  return rate_unit[val];  
}


static const char *unit_types[] =
{
  "unspecified",
  "C",
  "F",
  "K",
  "volts",
  "amps",
  "watts",
  "joules",
  "coulombs",
  "VA",
  "nits",
  "lumens",
  "lux",
  "candela",
  "kpa",
  "PSI",
  "newtons",
  "CFM",
  "RPM",
  "HZ",
  "useconds",
  "mseconds",
  "seconds",
  "minute",
  "hour",
  "day",
  "week",
  "mil",
  "inches",
  "feet",
  "cubic inchs",
  "cubic feet",
  "millimeters",
  "centimeters",
  "meters",
  "cubic centimeters"
  "cubic meters",
  "liters",
  "fluid ounces",
  "radians",
  "seradians",
  "revolutions",
  "cycles",
  "gravities",
  "ounces",
  "pounds",
  "foot pounds",
  "ounce inches",
  "gauss",
  "gilberts",
  "henries",
  "mhenries",
  "farads",
  "ufarads",
  "ohms",
  "siemens",
  "moles",
  "becquerels",
  "PPM",
  "unspecified",
  "decibels",
  "DbA",
  "DbC",
  "grays",
  "sieverts",
  "color temp deg K",
  "bits",
  "kbits",
  "mbits",
  "gbits",
  "bytes",
  "kbytes",
  "mbytes",
  "gbytes",
  "words",
  "dwords",
  "qwords",
  "lines",
  "hits",
  "misses",
  "retries",
  "resets",
  "overruns",
  "underruns",
  "collisions",
  "packets",
  "messages",
  "characters",
  "errors",
  "correctable_errors",
  "uncorrectable_errors"
};

#define dNumUnitTypes (sizeof(unit_types)/sizeof(char *))


const char *
IpmiUnitTypeToString( tIpmiUnitType val )
{
  if ( val > (int)dNumUnitTypes )
       return "invalid";

  return unit_types[val];
}


cIpmiSensorThreshold::cIpmiSensorThreshold( cIpmiMc *mc )
  : cIpmiSensor( mc ), 
    m_sensor_init_thresholds( false ),
    m_sensor_init_hysteresis( false ),
    m_hysteresis_support( eIpmiHysteresisSupportNone ),
    m_threshold_access( eIpmiThresholdAccessSupportNone ),
    m_assertion_event_mask( 0 ),
    m_deassertion_event_mask( 0 ),
    m_reading_mask( 0 ),
    m_threshold_readable( 0 ),
    m_threshold_settable( 0 ),
    m_rate_unit( eIpmRateUnitNone ),
    m_modifier_unit_use( eIpmiModifierUnitNone ),
    m_percentage( false ),
    m_base_unit( eIpmiUnitTypeUnspecified ),
    m_modifier_unit( eIpmiUnitTypeUnspecified ),
    m_normal_min_specified( false ),
    m_normal_max_specified( false ),
    m_nominal_reading_specified( false ),
    m_nominal_reading( 0 ),
    m_normal_max( 0 ),
    m_normal_min( 0 ),
    m_sensor_max( 0 ),
    m_sensor_min( 0 ),
    m_upper_non_recoverable_threshold( 0 ),
    m_upper_critical_threshold( 0 ),
    m_upper_non_critical_threshold( 0 ),
    m_lower_non_recoverable_threshold( 0 ),
    m_lower_critical_threshold( 0 ),
    m_lower_non_critical_threshold( 0 ),
    m_positive_going_threshold_hysteresis( 0 ),
    m_negative_going_threshold_hysteresis( 0 ),
    m_sensor_factors( 0 )
{
}


cIpmiSensorThreshold::~cIpmiSensorThreshold()
{
  if ( m_sensor_factors )
       delete m_sensor_factors;
}


bool
cIpmiSensorThreshold::GetDataFromSdr( cIpmiMc *mc, cIpmiSdr *sdr )
{
  if ( !cIpmiSensor::GetDataFromSdr( mc, sdr ) )
       return false;

  m_sensor_init_thresholds  = (sdr->m_data[10] >> 4) & 1;
  m_sensor_init_hysteresis  = (sdr->m_data[10] >> 3) & 1;

  m_hysteresis_support      = (tIpmiHysteresisSupport)((sdr->m_data[11] >> 4) & 3);
  m_threshold_access        = (tIpmiThresholdAccessSuport)((sdr->m_data[11] >> 2) & 3);

  // assertion
  unsigned int val = IpmiGetUint16( sdr->m_data + 14 );

  m_assertion_event_mask = val & dIpmiEventMask;
  m_reading_mask = (val >> 12) & 7;
  
  // deassertion
  val = IpmiGetUint16( sdr->m_data + 16 );

  m_deassertion_event_mask = val & dIpmiEventMask;
  m_reading_mask |= ((val >> 12) & 7 ) << 3;

  val = IpmiGetUint16( sdr->m_data + 18 );
  m_threshold_readable = val & 0x3f;
  m_threshold_settable  = (val >> 8) & 0x3f;

  m_rate_unit          = (tIpmiRateUnit)((sdr->m_data[20] >> 3) & 7);
  m_modifier_unit_use  = (tIpmiModifierUnit)((sdr->m_data[20] >> 1) & 3);
  m_percentage         = sdr->m_data[20] & 1;
  m_base_unit          = (tIpmiUnitType)sdr->m_data[21];
  m_modifier_unit      = (tIpmiUnitType)sdr->m_data[22];

  m_sensor_factors = CreateSensorFactors( mc, sdr );

  if ( !m_sensor_factors )
       return false;

  m_normal_min_specified = (sdr->m_data[30] >> 2) & 1;
  m_normal_max_specified = (sdr->m_data[30] >> 1) & 1;
  m_nominal_reading_specified = sdr->m_data[30] & 1;
  m_nominal_reading      = sdr->m_data[31];
  m_normal_max           = sdr->m_data[32];
  m_normal_min           = sdr->m_data[33];
  m_sensor_max           = sdr->m_data[34];
  m_sensor_min           = sdr->m_data[35];
  m_upper_non_recoverable_threshold = sdr->m_data[36];
  m_upper_critical_threshold = sdr->m_data[37];
  m_upper_non_critical_threshold = sdr->m_data[38];
  m_lower_non_recoverable_threshold = sdr->m_data[39];
  m_lower_critical_threshold = sdr->m_data[40];
  m_lower_non_critical_threshold = sdr->m_data[41];
  m_positive_going_threshold_hysteresis = sdr->m_data[42];
  m_negative_going_threshold_hysteresis = sdr->m_data[43];

  return true;
}


cIpmiSensorFactors *
cIpmiSensorThreshold::CreateSensorFactors( cIpmiMc *mc, cIpmiSdr *sdr )
{
  cIpmiSensorFactors *f= new cIpmiSensorFactors;

  if ( !f->GetDataFromSdr( sdr ) )
     {
       delete f;
       return 0;
     }

  return f;
}


void
cIpmiSensorThreshold::HandleNew( cIpmiDomain *domain )
{
  m_rate_unit_string          = IpmiRateUnitToString( m_rate_unit );
  m_base_unit_string          = IpmiUnitTypeToString( m_base_unit );
  m_modifier_unit_string      = IpmiUnitTypeToString( m_modifier_unit );

  cIpmiSensor::HandleNew( domain );
}


bool
cIpmiSensorThreshold::Cmp( const cIpmiSensor &s2 ) const
{
  if ( cIpmiSensor::Cmp( s2 ) == false )
       return false;

  const cIpmiSensorThreshold *t = dynamic_cast<const cIpmiSensorThreshold *>( &s2 );

  if ( !t )
       return false;

  if ( m_sensor_init_thresholds  != t->m_sensor_init_thresholds )
       return false;

  if ( m_sensor_init_hysteresis  != t->m_sensor_init_hysteresis )
       return false;

  if ( m_hysteresis_support      != t->m_hysteresis_support )
       return false;

  if ( m_threshold_access        != t->m_threshold_access )
       return false;

  if ( m_assertion_event_mask    != t->m_assertion_event_mask )
       return false;
       
  if ( m_deassertion_event_mask  != t->m_deassertion_event_mask )
       return false;

  if ( m_reading_mask            != t->m_reading_mask )
       return false;

  if ( m_threshold_readable      != t->m_threshold_readable )
       return false;

  if ( m_threshold_settable      != t->m_threshold_settable )
       return false;

  if ( m_rate_unit          != t->m_rate_unit )
       return false;

  if ( m_modifier_unit_use  != t->m_modifier_unit_use )
       return false;

  if ( m_percentage         != t->m_percentage )
       return false;

  if ( m_base_unit          != t->m_base_unit )
       return false;

  if ( m_modifier_unit      != t->m_modifier_unit )
       return false;

  if ( (bool)m_sensor_factors != (bool)t->m_sensor_factors )
       return false;

  if ( m_sensor_factors )
       if ( m_sensor_factors->Cmp( *t->m_sensor_factors ) == false )
            return false;

  if ( m_normal_min_specified != t->m_normal_min_specified )
       return false;

  if ( m_normal_max_specified != t->m_normal_max_specified )
       return false;

  if ( m_nominal_reading_specified != t->m_nominal_reading_specified )
       return false;

  if ( m_nominal_reading != t->m_nominal_reading )
       return false;

  if ( m_normal_max != t->m_normal_max )
       return false;

  if ( m_normal_min != t->m_normal_min )
       return false;

  if ( m_sensor_max != t->m_sensor_max )
       return false;

  if ( m_sensor_min != t->m_sensor_min )
       return false;
  if (    m_upper_non_recoverable_threshold
       != t->m_upper_non_recoverable_threshold )
       return false;

  if ( m_upper_critical_threshold != t->m_upper_critical_threshold )
       return false;

  if (    m_upper_non_critical_threshold
       != t->m_upper_non_critical_threshold )
       return false;

  if (    m_lower_non_recoverable_threshold
       != t->m_lower_non_recoverable_threshold )
       return false;

  if ( m_lower_critical_threshold != t->m_lower_critical_threshold )
       return false;

  if ( m_lower_non_critical_threshold
      != t->m_lower_non_critical_threshold )
       return false;

  if (    m_positive_going_threshold_hysteresis
       != t->m_positive_going_threshold_hysteresis )
       return false;

  if (    m_negative_going_threshold_hysteresis
       != t->m_negative_going_threshold_hysteresis )
       return false;

  return true;
}


bool
cIpmiSensorThreshold::IsThresholdReadable( tIpmiThresh event )
{
  return m_threshold_readable & ( 1 << event );
}


bool
cIpmiSensorThreshold::IsThresholdSettable( tIpmiThresh event )
{
  return m_threshold_settable & ( 1 << event );
}


SaErrorT
cIpmiSensorThreshold::CreateEvent( cIpmiEvent *event, SaHpiEventT &h )
{
  SaErrorT rv = cIpmiSensor::CreateEvent( event, h );

  if ( rv != SA_OK )
       return rv;

  // sensor event
  SaHpiSensorEventT &se = h.EventDataUnion.SensorEvent;

  bool assertion = !(event->m_data[9] >> 7);
  bool high      = (event->m_data[10] & 1);

  if (    (  assertion &&  high  )
       || ( !assertion && !high ) )
       se.Assertion = SAHPI_TRUE;
  else if ( (  assertion && !high )
       ||   ( !assertion &&  high ) )
       se.Assertion = SAHPI_FALSE;

  tIpmiThresh threshold = (tIpmiThresh)((event->m_data[10] >> 1) & 0x07);
  se.PreviousState = EventState();

  switch( threshold )
     {
       case eIpmiLowerNonCritical:
            se.EventState = SAHPI_ES_LOWER_MINOR;
            h.Severity    = SAHPI_MINOR;
            break;

       case eIpmiLowerCritical:
            se.EventState = SAHPI_ES_LOWER_MAJOR;
            h.Severity    = SAHPI_MAJOR;
            break;

       case eIpmiLowerNonRecoverable:
            se.EventState = SAHPI_ES_LOWER_CRIT;
            h.Severity    = SAHPI_CRITICAL;
            break;

       case eIpmiUpperNonCritical:
            se.EventState = SAHPI_ES_UPPER_MINOR;
            h.Severity    = SAHPI_MINOR;
            break;

       case eIpmiUpperCritical:
            se.EventState = SAHPI_ES_UPPER_MAJOR;
            h.Severity    = SAHPI_MAJOR;
            break;

       case eIpmiUpperNonRecoverable:
            se.EventState = SAHPI_ES_UPPER_CRIT;
            h.Severity = SAHPI_CRITICAL;
            break;

       default:
            IpmiLog( "Invalid threshold giving");
            se.EventState = SAHPI_ES_UNSPECIFIED;
     }

  EventState() = se.EventState;

  // byte 2
  tIpmiEventType type = (tIpmiEventType)(event->m_data[10] >> 6);

  if ( type == eIpmiEventData1 )
       ConvertToInterpreted( event->m_data[11],  se.TriggerReading );
  else if ( type == eIpmiEventData2 )
       se.Oem = (SaHpiUint32T)event->m_data[11]; 
  else if ( type == eIpmiEventData3 )
       se.SensorSpecific = (SaHpiUint32T)event->m_data[11]; 

  // byte 3
  type = (tIpmiEventType)((event->m_data[10] & 0x30) >> 4);

  if ( type == eIpmiEventData1 )
       ConvertToInterpreted( event->m_data[11], se.TriggerThreshold );
  else if ( type == eIpmiEventData2 )
       se.Oem = (SaHpiUint32T)event->m_data[12];
  else if ( type == eIpmiEventData3 )
       se.SensorSpecific = (SaHpiUint32T)event->m_data[12];

  return SA_OK;
}


void 
cIpmiSensorThreshold::Log()
{
  cIpmiSensor::Log();
  
  IpmiLog( "\tthreshold_access %s, hysteresis_support %s\n",
           IpmiThresholdAccessSupportToString( m_threshold_access ),
           IpmiHysteresisSupportToString( m_hysteresis_support ) );
}


unsigned short
cIpmiSensorThreshold::GetEventMask()
{
  // convert ipmi event mask to hpi event mask
  unsigned short amask = m_assertion_event_mask;
  unsigned short dmask = m_deassertion_event_mask;
  unsigned short mask  = 0;

  for( int i = 0; i < 12; i++ )
       if ( amask & (1 <<i ) || dmask & (1 <<i ) )
            mask |= (1 << (i/2));

  return mask;
}


SaErrorT
cIpmiSensorThreshold::ConvertFromInterpreted( const SaHpiSensorReadingT r,
                                              unsigned char &v )
{
  if ( (r.ValuesPresent & SAHPI_SRF_INTERPRETED) == 0 )
       return SA_OK;

  if ( r.Interpreted.Type != SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32 )
       return SA_ERR_HPI_INVALID_DATA;

  unsigned int raw;

  if ( !m_sensor_factors->ConvertToRaw( cIpmiSensorFactors::eRoundNormal, 
                                        (double)r.Interpreted.Value.SensorFloat32,
                                        raw ) )
       return false;

  v = (unsigned char)raw;

  return SA_OK;
}


void
cIpmiSensorThreshold::ConvertToInterpreted( unsigned int v, SaHpiSensorReadingT &r )
{
  memset( &r, 0, sizeof( SaHpiSensorReadingT ) );

  r.ValuesPresent = SAHPI_SRF_RAW;
  r.Raw = (SaHpiUint32T)v;

  double d;

  if ( m_sensor_factors->ConvertFromRaw( v, d ) )
     {
       r.ValuesPresent |= SAHPI_SRF_INTERPRETED;
       r.Interpreted.Type = SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32;
       r.Interpreted.Value.SensorFloat32 = (SaHpiFloat32T)d;
     }
}


bool
cIpmiSensorThreshold::CreateRdr( SaHpiRptEntryT &resource,
                                 SaHpiRdrT &rdr )
{
  if ( cIpmiSensor::CreateRdr( resource, rdr ) == false )
       return false;

  SaHpiSensorRecT &rec = rdr.RdrTypeUnion.SensorRec;
  rec.Events = GetEventMask();

  // data format
  rec.DataFormat.BaseUnits     = (SaHpiSensorUnitsT)BaseUnit();
  rec.DataFormat.ModifierUnits = (SaHpiSensorUnitsT)ModifierUnit();
  rec.DataFormat.ModifierUse  = (SaHpiSensorModUnitUseT)ModifierUnitUse();

  if ( m_sensor_factors->CreateDataFormat( rec.DataFormat ) == false )
       return false;

  rec.DataFormat.Percentage = (SaHpiBoolT)Percentage();

  rec.DataFormat.Range.Flags = SAHPI_SRF_MAX | SAHPI_SRF_MIN;
  ConvertToInterpreted( SensorMax(), rec.DataFormat.Range.Max );
  ConvertToInterpreted( SensorMin(), rec.DataFormat.Range.Min );

  if ( NominalReadingSpecified() )
     {
       rec.DataFormat.Range.Flags |= SAHPI_SRF_NOMINAL;
       ConvertToInterpreted( NominalReading(), rec.DataFormat.Range.Nominal );
     }

  if ( NormalMaxSpecified() )
     {
       rec.DataFormat.Range.Flags |= SAHPI_SRF_NORMAL_MAX;
       ConvertToInterpreted( NormalMax(), rec.DataFormat.Range.NormalMax );
     }

  if ( NormalMinSpecified() )
     {
       rec.DataFormat.Range.Flags |= SAHPI_SRF_NORMAL_MIN;
       ConvertToInterpreted( NormalMin(), rec.DataFormat.Range.NormalMin );
     }

  // thresholds
  unsigned int        acc = ThresholdAccess();

  if ( acc >= eIpmiThresholdAccessSupportReadable )
     {
       rec.ThresholdDefn.IsThreshold = SAHPI_TRUE;
       rec.ThresholdDefn.TholdCapabilities = SAHPI_SRF_RAW | SAHPI_SRF_INTERPRETED;

       SaHpiSensorThdMaskT temp = 0;

       int val = IsThresholdReadable( eIpmiLowerNonCritical );
       if ( val )
            temp |= SAHPI_STM_LOW_MINOR;

       val = IsThresholdReadable( eIpmiLowerCritical );

       if ( val )
            temp |= SAHPI_STM_LOW_MAJOR;

       val = IsThresholdReadable( eIpmiLowerNonRecoverable );
       if ( val )
            temp |= SAHPI_STM_LOW_CRIT;
			
       val = IsThresholdReadable( eIpmiUpperNonCritical );
       if ( val )
            temp |= SAHPI_STM_UP_MINOR;
			
       val = IsThresholdReadable( eIpmiUpperCritical );
       if ( val )
            temp |= SAHPI_STM_UP_MAJOR;
			
       val = IsThresholdReadable( eIpmiUpperNonRecoverable );
       if ( val )
            temp |= SAHPI_STM_UP_CRIT;

       if (    HysteresisSupport() == eIpmiHysteresisSupportReadable 
            || HysteresisSupport() == eIpmiHysteresisSupportSettable ) 
            temp |=   SAHPI_STM_UP_HYSTERESIS
                    | SAHPI_STM_LOW_HYSTERESIS;

       rec.ThresholdDefn.ReadThold = temp;
     }

  if ( acc == eIpmiThresholdAccessSupportSettable )
     {
       SaHpiSensorThdMaskT temp = 0;
       int val = IsThresholdSettable( eIpmiLowerNonCritical );

       if ( val )
            temp |= SAHPI_STM_LOW_MINOR;

       val = IsThresholdSettable( eIpmiLowerCritical );
       if ( val )
            temp |= SAHPI_STM_LOW_MAJOR;

       val = IsThresholdSettable( eIpmiLowerNonRecoverable );
       if ( val )
            temp |= SAHPI_STM_LOW_CRIT;

       val = IsThresholdSettable( eIpmiUpperNonCritical );
       if ( val )
            temp |= SAHPI_STM_UP_MINOR;

       val = IsThresholdSettable( eIpmiUpperCritical );
       if ( val )
            temp |= SAHPI_STM_UP_MAJOR;

       val = IsThresholdSettable( eIpmiUpperNonRecoverable );
       if ( val )
            temp |= SAHPI_STM_UP_CRIT;

       if ( HysteresisSupport() == eIpmiHysteresisSupportSettable )
            temp |=   SAHPI_STM_UP_HYSTERESIS
                    | SAHPI_STM_LOW_HYSTERESIS;

       rec.ThresholdDefn.WriteThold = temp;
     }

  rec.ThresholdDefn.FixedThold = 0;

  if ( HysteresisSupport() == eIpmiHysteresisSupportFixed )
       rec.ThresholdDefn.FixedThold |=   SAHPI_STM_UP_HYSTERESIS
                                       | SAHPI_STM_LOW_HYSTERESIS;

  return true;
}


SaErrorT
cIpmiSensorThreshold::GetData( SaHpiSensorReadingT &data )
{
  if ( Ignore() )
     {
       dbg("sensor is ignored");
       return SA_ERR_HPI_NOT_PRESENT;
     }

  cIpmiMsg rsp;
  SaErrorT rv = GetSensorReading( rsp );

  if ( rv != SA_OK )
       return rv;

  ConvertToInterpreted( rsp.m_data[1], data );

  return SA_OK;
}


SaErrorT
cIpmiSensorThreshold::GetDefaultThresholds( SaHpiSensorThresholdsT &thres )
{
  if ( IsThresholdReadable( eIpmiLowerNonRecoverable ) )
       ConvertToInterpreted( m_lower_non_recoverable_threshold, thres.LowCritical );

  if ( IsThresholdReadable( eIpmiLowerCritical ) )
       ConvertToInterpreted( m_lower_critical_threshold, thres.LowMajor );

  if ( IsThresholdReadable( eIpmiLowerNonCritical ) )
       ConvertToInterpreted( m_lower_non_critical_threshold, thres.LowMinor );

  if ( IsThresholdReadable( eIpmiUpperNonRecoverable ) )
       ConvertToInterpreted( m_upper_non_recoverable_threshold, thres.UpCritical );

  if ( IsThresholdReadable( eIpmiUpperCritical ) )
       ConvertToInterpreted( m_upper_critical_threshold, thres.UpMajor );
  
  if ( IsThresholdReadable( eIpmiUpperNonCritical ) )
       ConvertToInterpreted( m_upper_non_critical_threshold, thres.UpMinor );

  return SA_OK;
}


SaErrorT
cIpmiSensorThreshold::GetThresholds( SaHpiSensorThresholdsT &thres )
{
  cIpmiEntity *ent = GetEntity();

  IpmiLog( "%d.%d sensor %d (%s) get thresholds.\n",
           ent->EntityId(), ent->EntityInstance(),
           m_num, m_id );

  if ( m_threshold_access == eIpmiThresholdAccessSupportFixed )
       // Thresholds are fixed, pull them from the SDR.
       return GetDefaultThresholds( thres );

  cIpmiMsg msg( eIpmiNetfnSensorEvent, eIpmiCmdGetSensorThreshold );
  cIpmiMsg rsp;

  msg.m_data_len = 1;
  msg.m_data[0]  = m_num;

  int rv = m_mc->SendCommand( msg, rsp, m_lun );

  if ( rv )
     {
       IpmiLog( "Error getting thresholds: %x !\n", rv );

       return SA_ERR_HPI_INVALID_DATA;
     }

  if ( rsp.m_data[0] )
     {
       IpmiLog( "IPMI error getting thresholds: %x !\n", rsp.m_data[0] );

       return SA_ERR_HPI_INVALID_DATA;
    }

  if ( rsp.m_data[1] & (1 << eIpmiLowerNonRecoverable) )
       ConvertToInterpreted( rsp.m_data[4], thres.LowCritical );

  if ( rsp.m_data[1] & (1 << eIpmiLowerCritical ) )
       ConvertToInterpreted( rsp.m_data[3], thres.LowMajor );

  if ( rsp.m_data[1] & (1 << eIpmiLowerNonCritical ) )
       ConvertToInterpreted( rsp.m_data[2], thres.LowMinor );

  if ( rsp.m_data[1] & (1 << eIpmiUpperNonRecoverable ) )
       ConvertToInterpreted( rsp.m_data[7], thres.UpCritical );

  if ( rsp.m_data[1] & (1 << eIpmiUpperCritical ) )
       ConvertToInterpreted( rsp.m_data[6], thres.UpMajor );

  if ( rsp.m_data[1] & (1 << eIpmiUpperNonCritical ) )
       ConvertToInterpreted( rsp.m_data[5], thres.UpMinor );

  return SA_OK;
}


SaErrorT
cIpmiSensorThreshold::GetHysteresis( SaHpiSensorThresholdsT &thres )
{
  cIpmiEntity *ent = GetEntity();

  IpmiLog( "%d.%d sensor %d (%s) get hysteresis.\n",
           ent->EntityId(), ent->EntityInstance(),
           m_num, m_id );

  if (    m_hysteresis_support != eIpmiHysteresisSupportReadable
       && m_hysteresis_support != eIpmiHysteresisSupportSettable)
       return SA_OK;

  cIpmiMsg msg( eIpmiNetfnSensorEvent, eIpmiCmdGetSensorHysteresis );
  cIpmiMsg rsp;

  msg.m_data_len = 2;
  msg.m_data[0]  = m_num;
  msg.m_data[1]  = 0xff;

  int rv = m_mc->SendCommand( msg, rsp, m_lun );

  if ( rv )
     {
       IpmiLog( "Error sending hysteresis get command: %x !\n", rv );

       return SA_ERR_HPI_INVALID_CMD;
     }

  if ( rsp.m_data[0] || rsp.m_data_len < 3 )
     {
       IpmiLog( "IPMI error getting hysteresis: %x !\n",
                rsp.m_data[0] );

       return SA_ERR_HPI_INVALID_CMD;
    }

  ConvertToInterpreted( rsp.m_data[1], thres.PosThdHysteresis );
  ConvertToInterpreted( rsp.m_data[2], thres.NegThdHysteresis );

  return 0;
}


SaErrorT
cIpmiSensorThreshold::GetThresholdsAndHysteresis( SaHpiSensorThresholdsT &thres )
{
  SaErrorT rv;

  if ( Ignore() )
     {
       IpmiLog( "sensor is ignored !\n");
       return SA_ERR_HPI_NOT_PRESENT;
     }

  memset( &thres, 0, sizeof( SaHpiSensorThresholdsT ) );

  bool found = false;

  if ( ThresholdAccess() == eIpmiThresholdAccessSupportNone )
       IpmiLog( "sensor doesn't support threshold read !\n" );
  else
     {
       rv = GetThresholds( thres );

       if ( rv != SA_OK )
            return rv;

       found = true;
     }

  if (    HysteresisSupport() == eIpmiHysteresisSupportReadable
       || HysteresisSupport() == eIpmiHysteresisSupportSettable )
     {
       rv = GetHysteresis( thres );

       if ( rv != SA_OK )
            return rv;

       found = true;
     }
  else
       IpmiLog( "sensor doesn't support hysteresis read !\n");

  if ( !found )
       return SA_ERR_HPI_INVALID_CMD;

  return SA_OK;
}


SaErrorT 
cIpmiSensorThreshold::ConvertThreshold( const SaHpiSensorReadingT &r, 
                                        tIpmiThresh event,
                                        unsigned char &data,
                                        unsigned char &mask )
{
  SaErrorT rv = ConvertFromInterpreted( r, data );

  if ( rv != SA_OK )
       return rv;
  
  if ( r.ValuesPresent & SAHPI_SRF_INTERPRETED )
       mask |= (1 << event);

  return SA_OK;
}


SaErrorT
cIpmiSensorThreshold::SetThresholds( const SaHpiSensorThresholdsT &thres )
{
  cIpmiMsg msg( eIpmiNetfnSensorEvent, eIpmiCmdSetSensorThreshold );
  msg.m_data_len = 8;
  msg.m_data[0]  = m_num;
  msg.m_data[1]  = 0;

  SaErrorT rv;

  rv = ConvertThreshold( thres.LowMinor, eIpmiLowerNonCritical,
                         msg.m_data[2], msg.m_data[0] );

  if ( rv != SA_OK )
       return rv;
  
  rv = ConvertThreshold( thres.LowMajor, eIpmiLowerCritical,
                         msg.m_data[3], msg.m_data[0] );

  if ( rv != SA_OK )
       return rv;
  
  rv = ConvertThreshold( thres.LowCritical, eIpmiLowerNonRecoverable,
                         msg.m_data[4], msg.m_data[0] );

  if ( rv != SA_OK )
       return rv;
  
  rv = ConvertThreshold( thres.UpMinor, eIpmiUpperNonCritical,
                         msg.m_data[5], msg.m_data[0] );

  if ( rv != SA_OK )
       return rv;
  
  rv = ConvertThreshold( thres.UpMajor, eIpmiUpperCritical,
                         msg.m_data[6], msg.m_data[0] );

  if ( rv != SA_OK )
       return rv;
  
  rv = ConvertThreshold( thres.UpCritical, eIpmiUpperNonRecoverable,
                         msg.m_data[7], msg.m_data[0] );

  if ( rv != SA_OK )
       return rv;

  // nothing to do
  if ( msg.m_data[1] == 0 )
       return SA_OK;

  // settable ?
  if ( m_threshold_access != eIpmiThresholdAccessSupportSettable )
       return SA_ERR_HPI_INVALID_CMD;

  if ( (m_threshold_settable | msg.m_data[1]) != m_threshold_settable )
       return SA_ERR_HPI_INVALID_CMD;

  // set thresholds
  cIpmiMsg rsp;

  int r = m_mc->SendCommand( msg, rsp, m_lun );

  if ( r )
     {
       IpmiLog( "Error sending thresholds set command: %x !\n", r );
       return SA_ERR_HPI_INVALID_CMD;
     }

  if ( rsp.m_data[0] )
     {
       IpmiLog( "IPMI error setting thresholds: %x !\n",
                rsp.m_data[0] );
       return SA_ERR_HPI_INVALID_CMD;
    }

  return SA_OK;
}


SaErrorT
cIpmiSensorThreshold::SetHysteresis( const SaHpiSensorThresholdsT &thres )
{
  // nothing to do
  if (    !(thres.PosThdHysteresis.ValuesPresent & SAHPI_SRF_INTERPRETED)
       && !(thres.NegThdHysteresis.ValuesPresent & SAHPI_SRF_INTERPRETED) )
       return SA_OK;

  if ( m_hysteresis_support != eIpmiHysteresisSupportSettable )
       return SA_ERR_HPI_INVALID_CMD;

  // cannot write only one hysteresis
  if (    !(thres.PosThdHysteresis.ValuesPresent & SAHPI_SRF_INTERPRETED)
       || !(thres.NegThdHysteresis.ValuesPresent & SAHPI_SRF_INTERPRETED) )
       return SA_ERR_HPI_UNKNOWN;

  cIpmiMsg  msg( eIpmiNetfnSensorEvent, eIpmiCmdSetSensorHysteresis );
  cIpmiMsg  rsp;

  msg.m_data_len = 4;
  msg.m_data[0]  = m_num;
  msg.m_data[1]  = 0xff;

  SaErrorT rv = ConvertFromInterpreted( thres.PosThdHysteresis, msg.m_data[2] );
  
  if ( rv != SA_OK )
       return rv;
  
  rv = ConvertFromInterpreted( thres.NegThdHysteresis, msg.m_data[3] );
  
  if ( rv != SA_OK )
       return rv;

  int r = m_mc->SendCommand( msg, rsp, m_lun );

  if ( r )
     {
       IpmiLog( "Error sending hysteresis set command: %x !\n", r );

       return SA_ERR_HPI_INVALID_CMD;
     }

  if ( rsp.m_data[0] )
     {
       IpmiLog( "IPMI error setting hysteresis: %x !\n", rsp.m_data[0] );
       return SA_ERR_HPI_INVALID_CMD;
     }

  return SA_OK;
}


SaErrorT
cIpmiSensorThreshold::SetThresholdsAndHysteresis( const SaHpiSensorThresholdsT &thres )
{
  SaErrorT rv;

  if ( Ignore() )
     {
       IpmiLog( "sensor is ignored !\n");
       return SA_ERR_HPI_NOT_PRESENT;
     }

  if ( ThresholdAccess() == eIpmiThresholdAccessSupportSettable )
     {
       rv = SetThresholds( thres );

       if ( rv != SA_OK )
            return rv;
     }
  else
       IpmiLog( "sensor doesn't support threshold set !\n" );

  if ( HysteresisSupport() == eIpmiHysteresisSupportSettable )
     { 
       rv = SetHysteresis( thres );

       if ( rv != SA_OK )
            return rv;
     }
  else
       IpmiLog( "sensor doesn't support hysteresis set !\n");

  return SA_OK;
}


SaErrorT
cIpmiSensorThreshold::GetEventEnables( SaHpiSensorEvtEnablesT &enables )
{
  cIpmiMsg rsp;
  SaErrorT rv = cIpmiSensor::GetEventEnables( enables, rsp );

  if ( rv != SA_OK )
       return rv;

  unsigned int amask = IpmiGetUint16( rsp.m_data + 2 );
  unsigned int dmask = IpmiGetUint16( rsp.m_data + 4 );

  for( int i = 0; i < 6; i++ )
     {
       unsigned int b1 = 1 << (2*i);
       unsigned int b2 = 1 << (2*i + 1);

       if ( (amask & b1) || (amask & b2) )
            enables.AssertEvents |= (1 << i);
       
       if ( (dmask & b1) || (dmask & b2) )
            enables.DeassertEvents |= (1 << i);
     }

  return SA_OK;
}


SaErrorT
cIpmiSensorThreshold::SetEventEnables( const SaHpiSensorEvtEnablesT &enables )
{
  // create de/assertion event mask
  unsigned int amask = 0;
  unsigned int dmask = 0;

  for( int i = 0; i < 6; i++ )
     {
       unsigned int b1 = 1 << (2*i);
       unsigned int b2 = 1 << (2*i + 1);
       unsigned int b  = b1 | b2; // this is 3 << (2*i)

       if ( enables.AssertEvents & ( 1 << i ) )
          {
            if ( (m_assertion_event_mask & b) == 0 )
               {
                 // this event is not allowed
                 IpmiLog( "SetEventEnables: assertion event %s not allowed !\n",
                          IpmiThresToString( (tIpmiThresh)i ) );

                 return SA_ERR_HPI_INVALID_CMD; 
               }

            amask |= (m_assertion_event_mask & b);
          }

       if ( enables.DeassertEvents & ( 1 << i ) )
          {
            if ( (m_deassertion_event_mask & b) == 0 )
               {
                 // this event is not allowed
                 IpmiLog( "SetEventEnables: deassertion event %s not allowed !\n",
                          IpmiThresToString( (tIpmiThresh)i ) );

                 return SA_ERR_HPI_INVALID_CMD; 
               }

            dmask |= (m_deassertion_event_mask & b);
          }
     }

  cIpmiMsg msg;
  IpmiSetUint16( msg.m_data + 2, amask );
  IpmiSetUint16( msg.m_data + 4, dmask );

  SaErrorT rv = cIpmiSensor::SetEventEnables( enables, msg );

  return rv;
}
