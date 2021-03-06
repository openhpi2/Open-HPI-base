/*
 * (C) Copyright IBM Corp. 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *   Konrad Rzeszutek <konradr@us.ibm.com>

 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.array-user.conf,v 5.18 2003/11/07 17:03:52 rstory Exp $
 *
 * $Id$
 *
 */
#ifndef SAHPISENSORREADINGNOMINALTABLE_H
#define SAHPISENSORREADINGNOMINALTABLE_H

#ifdef __cplusplus
extern "C"
{
#endif


#include <net-snmp/net-snmp-config.h>
#include <net-snmp/library/container.h>
#include <net-snmp/agent/table_array.h>
#include <SaHpi.h>
#include <hpiSubagent.h>


	/** Index saHpiDomainID is external */
	/** Index saHpiResourceID is external */
	/** Index saHpiSensorIndex is external */

  typedef struct saHpiSensorReadingNominalTable_context_s
  {
    netsnmp_index index;
	/** INTEGER = ASN_INTEGER */
    long saHpiSensorReadingNominalValuesPresent;

	/** UNSIGNED32 = ASN_UNSIGNED */
    unsigned long saHpiSensorReadingNominalRaw;

	/** OCTETSTR = ASN_OCTET_STR */
    unsigned char
      saHpiSensorReadingNominalInterpreted[SENSOR_READING_INTER_MAX];
    size_t saHpiSensorReadingNominalInterpreted_len;

	/** INTEGER = ASN_INTEGER */
    long saHpiSensorReadingNominalStatus;

	/** OCTETSTR = ASN_OCTET_STR */
    unsigned char
      saHpiSensorReadingNominalEventStatus[SENSOR_READING_EVENT_MAX];
    size_t saHpiSensorReadingNominalEventStatus_len;

    long sensor_id;
    long resource_id;
    long domain_id;
    long hash;


  } saHpiSensorReadingNominalTable_context;

/*************************************************************
 * function declarations
 */

  void initialize_table_saHpiSensorReadingNominalTable (void);

  int
    delete_ReadingNominal_row (SaHpiDomainIdT domain_id,
			       SaHpiResourceIdT resource_id,
			       SaHpiSensorNumT num);

  int populate_ReadingNominal (SaHpiDomainIdT domain_id,
			       SaHpiResourceIdT resource_id,
			       SaHpiSensorNumT sensor_id,
			       SaHpiEventCategoryT sensor_category,
			       SaHpiSensorReadingT * reading);


  int
    saHpiSensorReadingNominalTable_get_value (netsnmp_request_info *,
					      netsnmp_index *,
					      netsnmp_table_request_info *);


/*************************************************************
 * oid declarations
 */
  extern oid saHpiSensorReadingNominalTable_oid[];
  extern size_t saHpiSensorReadingNominalTable_oid_len;

#define saHpiSensorReadingNominalTable_TABLE_OID hpiSensor_OID,4

/*************************************************************
 * column number definitions for table saHpiSensorReadingNominalTable
 */
#define COLUMN_SAHPISENSORREADINGNOMINALVALUESPRESENT 1
#define COLUMN_SAHPISENSORREADINGNOMINALRAW 2
#define COLUMN_SAHPISENSORREADINGNOMINALINTERPRETED 3
#define COLUMN_SAHPISENSORREADINGNOMINALSTATUS 4
#define COLUMN_SAHPISENSORREADINGNOMINALEVENTSTATUS 5
#define saHpiSensorReadingNominalTable_COL_MIN 1
#define saHpiSensorReadingNominalTable_COL_MAX 5

  int
    saHpiSensorReadingNominalTable_extract_index
    (saHpiSensorReadingNominalTable_context * ctx, netsnmp_index * hdr);

  void saHpiSensorReadingNominalTable_set_reserve1 (netsnmp_request_group *);
  void saHpiSensorReadingNominalTable_set_reserve2 (netsnmp_request_group *);
  void saHpiSensorReadingNominalTable_set_action (netsnmp_request_group *);
  void saHpiSensorReadingNominalTable_set_commit (netsnmp_request_group *);
  void saHpiSensorReadingNominalTable_set_free (netsnmp_request_group *);
  void saHpiSensorReadingNominalTable_set_undo (netsnmp_request_group *);

    saHpiSensorReadingNominalTable_context
    * saHpiSensorReadingNominalTable_duplicate_row
    (saHpiSensorReadingNominalTable_context *);
    netsnmp_index
    * saHpiSensorReadingNominalTable_delete_row
    (saHpiSensorReadingNominalTable_context *);

  int
    saHpiSensorReadingNominalTable_can_delete
    (saHpiSensorReadingNominalTable_context * undo_ctx,
     saHpiSensorReadingNominalTable_context * row_ctx,
     netsnmp_request_group * rg);



    saHpiSensorReadingNominalTable_context
    * saHpiSensorReadingNominalTable_create_row (netsnmp_index *);

#ifdef __cplusplus
};
#endif

#endif /** SAHPISENSORREADINGNOMINALTABLE_H */
