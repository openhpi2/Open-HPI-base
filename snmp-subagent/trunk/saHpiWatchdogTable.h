/*
 * (C) Copyright IBM Corp. 2003
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
 *
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.array-user.conf,v 5.15.2.1 2003/02/27 05:59:41 rstory Exp $
 *
 * $Id$
 *
 */
#ifndef SAHPIWATCHDOGTABLE_H
#define SAHPIWATCHDOGTABLE_H

#ifdef __cplusplus
extern          "C" {
#endif


#include <net-snmp/net-snmp-config.h>
#include <net-snmp/library/container.h>
#include <net-snmp/agent/table_array.h>
#include <SaHpi.h>
  /*
   * Number of index values in this table.
   * Consult the HPI-MIB
   *
   * If this number changes, look in the src code for this 
   * define, and make sure to add/remove the new index value(s).
   */
#define WATCHDOG_INDEX_NR 3

    typedef struct saHpiWatchdogTable_context_s {
        netsnmp_index   index;

        /** UNSIGNED32 = ASN_UNSIGNED */
        unsigned long   saHpiWatchdogNum;

        /** TruthValue = ASN_INTEGER */
        int            saHpiWatchdogLog;

        /** TruthValue = ASN_INTEGER */
        int            saHpiWatchdogRunning;

        /** INTEGER = ASN_INTEGER */
        long            saHpiWatchdogTimerUse;

        /** INTEGER = ASN_INTEGER */
        long            saHpiWatchdogTimerAction;

        /** INTEGER = ASN_INTEGER */
        long            saHpiWatchdogPretimerInterrupt;

        /** UNSIGNED32 = ASN_UNSIGNED */
        unsigned long   saHpiWatchdogPreTimeoutInterval;

        /** UNSIGNED32 = ASN_UNSIGNED */
        unsigned long   saHpiWatchdogTimerUseExpFlags;

        /** UNSIGNED32 = ASN_UNSIGNED */
        unsigned long   saHpiWatchdogTimerInitialCount;

        /** UNSIGNED32 = ASN_UNSIGNED */
        unsigned long   saHpiWatchdogTimerPresentCount;

      /** TruthValue = ASN_INTEGER */
      long            saHpiWatchdogTimerReset;
        /** UNSIGNED32 = ASN_UNSIGNED */
        unsigned long   saHpiWatchdogOem;

      /** RowPointer = ASN_OBJECT_ID */
        oid             saHpiWatchdogRDR[MAX_OID_LEN];
        long            saHpiWatchdogRDR_len;

        long resource_id;
        long domain_id;
	long hash;
    } saHpiWatchdogTable_context;

/*************************************************************
 * function declarations
 */
   
    void            initialize_table_saHpiWatchdogTable(void);
 
    int             saHpiWatchdogTable_get_value(netsnmp_request_info *,
                                                 netsnmp_index *,
                                                 netsnmp_table_request_info
                                                 *);


  int  populate_watchdog(SaHpiWatchdogRecT *watchdog, 
			 SaHpiRptEntryT *rpt_entry,
			 oid *rdr_oid, size_t rdr_oid_len,
			 oid *watchdog_oid, 
			 size_t *watchdog_oid_len);
  

  int
  delete_watchdog(SaHpiDomainIdT domain_id,
		  SaHpiResourceIdT resource_id,
		  SaHpiWatchdogNumT num);

  int set_watchdog(saHpiWatchdogTable_context *ctx);
  
 
 

/*************************************************************
 * oid declarations
 */
  //  extern oid      saHpiWatchdogTable_oid[];
  // extern size_t   saHpiWatchdogTable_oid_len;
//1,3,6,1,3,90,3,10
#define saHpiWatchdogTable_TABLE_OID hpiResources_OID, 10


/***********************************
**************************
 * column number definitions for table saHpiWatchdogTable
 */

#define COLUMN_SAHPIWATCHDOGNUM 1
#define COLUMN_SAHPIWATCHDOGLOG 2
#define COLUMN_SAHPIWATCHDOGRUNNING 3
#define COLUMN_SAHPIWATCHDOGTIMERUSE 4
#define COLUMN_SAHPIWATCHDOGTIMERACTION 5
#define COLUMN_SAHPIWATCHDOGPRETIMERINTERRUPT 6
#define COLUMN_SAHPIWATCHDOGPRETIMEOUTINTERVAL 7
#define COLUMN_SAHPIWATCHDOGTIMERUSEEXPFLAGS 8
#define COLUMN_SAHPIWATCHDOGTIMERINITIALCOUNT 9
#define COLUMN_SAHPIWATCHDOGTIMERPRESENTCOUNT 10
#define COLUMN_SAHPIWATCHDOGTIMERRESET 11
#define COLUMN_SAHPIWATCHDOGOEM 12
#define COLUMN_SAHPIWATCHDOGRDR 13
#define saHpiWatchdogTable_COL_MIN 1
#define saHpiWatchdogTable_COL_MAX 13


    int            
        saHpiWatchdogTable_extract_index(saHpiWatchdogTable_context * ctx,
                                         netsnmp_index * hdr);

    void            saHpiWatchdogTable_set_reserve1(netsnmp_request_group
                                                    *);
    void            saHpiWatchdogTable_set_reserve2(netsnmp_request_group
                                                    *);
    void            saHpiWatchdogTable_set_action(netsnmp_request_group *);
    void            saHpiWatchdogTable_set_commit(netsnmp_request_group *);
    void            saHpiWatchdogTable_set_free(netsnmp_request_group *);
    void            saHpiWatchdogTable_set_undo(netsnmp_request_group *);

    saHpiWatchdogTable_context
        *saHpiWatchdogTable_duplicate_row(saHpiWatchdogTable_context *);
    netsnmp_index 
        *saHpiWatchdogTable_delete_row(saHpiWatchdogTable_context *);

    int            
        saHpiWatchdogTable_can_delete(saHpiWatchdogTable_context *
                                      undo_ctx,
                                      saHpiWatchdogTable_context * row_ctx,
                                      netsnmp_request_group * rg);



    saHpiWatchdogTable_context *saHpiWatchdogTable_create_row(netsnmp_index
                                                              *);



 

#ifdef __cplusplus
};
#endif

#endif /** SAHPIWATCHDOGTABLE_H */
