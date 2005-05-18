/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.array-user.conf,v 5.18.2.2 2004/02/09 18:21:47 rstory Exp $
 *
 * $Id$
 *
 * Yes, there is lots of code here that you might not use. But it is much
 * easier to remove code than to add it!
 */
#ifndef SAHPIAREATABLE_H
#define SAHPIAREATABLE_H

#ifdef __cplusplus
extern "C" {
#endif

    
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/library/container.h>
#include <net-snmp/agent/table_array.h>

        /** Index saHpiDomainId is external */
        /** Index saHpiResourceId is external */
        /** Index saHpiResourceIsHistorical is external */
        /** Index saHpiInventoryId is external */
        /** Index saHpiAreaId is internal */

typedef struct saHpiAreaTable_context_s {
    netsnmp_index index; /** THIS MUST BE FIRST!!! */

    /*************************************************************
     * You can store data internally in this structure.
     *
     * TODO: You will probably have to fix a few types here...
     */
    /** TODO: add storage for external index(s)! */
        /** SaHpiInstrumentId = ASN_UNSIGNED */
            unsigned long saHpiAreaId;

        /** SaHpiInstrumentId = ASN_UNSIGNED */
            unsigned long saHpiAreaIdIndex;

        /** INTEGER = ASN_INTEGER */
            long saHpiAreaType;

        /** TruthValue = ASN_INTEGER */
            long saHpiAreaIsReadOnly;

        /** RowStatus = ASN_INTEGER */
            long saHpiAreaRowStatus;

        /** UNSIGNED32 = ASN_UNSIGNED */
            unsigned long saHpiAreaNumDataFields;


    /*
     * OR
     *
     * Keep a pointer to your data
     */
    void * data;

    /*
     *add anything else you want here
     */

} saHpiAreaTable_context;


/*************************************************************
 * function declarations
 */
SaErrorT populate_area (SaHpiSessionIdT sessionid, 
                        SaHpiRdrT *rdr_entry,
                        SaHpiRptEntryT *rpt_entry);
int set_table_area_type (saHpiAreaTable_context *row_ctx);
int set_table_area_delete (saHpiAreaTable_context *row_ctx);

/*************************************************************
 * function declarations
 */
void init_saHpiAreaTable(void);
void initialize_table_saHpiAreaTable(void);
const saHpiAreaTable_context * saHpiAreaTable_get_by_idx(netsnmp_index *);
const saHpiAreaTable_context * saHpiAreaTable_get_by_idx_rs(netsnmp_index *,
                                        int row_status);
int saHpiAreaTable_get_value(netsnmp_request_info *, netsnmp_index *, netsnmp_table_request_info *);


/*************************************************************
 * oid declarations
 */
extern oid saHpiAreaTable_oid[];
extern size_t saHpiAreaTable_oid_len;

#define saHpiAreaTable_TABLE_OID 1,3,6,1,4,1,18568,2,1,1,4,8,4

/* Number of table Indexes */
#define AREA_INDEX_NR 5 
#define saHpiDomainId_INDEX 0
#define saHpiResourceEntryId_INDEX 1
#define saHpiResourceIsHistorical_INDEX 2
#define saHpiInventoryId_INDEX 3
#define saHpiAreaId_INDEX 4

    
/*************************************************************
 * column number definitions for table saHpiAreaTable
 */
#define COLUMN_SAHPIAREAID 1
#define COLUMN_SAHPIAREAIDINDEX 2
#define COLUMN_SAHPIAREATYPE 3
#define COLUMN_SAHPIAREAISREADONLY 4
#define COLUMN_SAHPIAREAROWSTATUS 5
#define COLUMN_SAHPIAREANUMDATAFIELDS 6
#define saHpiAreaTable_COL_MIN 3
#define saHpiAreaTable_COL_MAX 6

/* comment out the following line if you don't handle SET-REQUEST for saHpiAreaTable */
#define saHpiAreaTable_SET_HANDLING

/* comment out the following line if you can't create new rows */
#define saHpiAreaTable_ROW_CREATION

/* comment out the following line if you don't want the secondary index */
#define saHpiAreaTable_IDX2

/* uncommend the following line if you allow modifications to an
 * active row */
/** define saHpiAreaTable_CAN_MODIFY_ACTIVE_ROW */

#ifdef saHpiAreaTable_SET_HANDLING

int saHpiAreaTable_extract_index( saHpiAreaTable_context * ctx, netsnmp_index * hdr );

void saHpiAreaTable_set_reserve1( netsnmp_request_group * );
void saHpiAreaTable_set_reserve2( netsnmp_request_group * );
void saHpiAreaTable_set_action( netsnmp_request_group * );
void saHpiAreaTable_set_commit( netsnmp_request_group * );
void saHpiAreaTable_set_free( netsnmp_request_group * );
void saHpiAreaTable_set_undo( netsnmp_request_group * );

saHpiAreaTable_context * saHpiAreaTable_duplicate_row( saHpiAreaTable_context* );
netsnmp_index * saHpiAreaTable_delete_row( saHpiAreaTable_context* );

int saHpiAreaTable_can_activate(saHpiAreaTable_context *undo_ctx,
                      saHpiAreaTable_context *row_ctx,
                      netsnmp_request_group * rg);
int saHpiAreaTable_can_deactivate(saHpiAreaTable_context *undo_ctx,
                        saHpiAreaTable_context *row_ctx,
                        netsnmp_request_group * rg);
int saHpiAreaTable_can_delete(saHpiAreaTable_context *undo_ctx,
                    saHpiAreaTable_context *row_ctx,
                    netsnmp_request_group * rg);
    
    
#ifdef saHpiAreaTable_ROW_CREATION
saHpiAreaTable_context * saHpiAreaTable_create_row( netsnmp_index* );
#endif
#endif

#ifdef saHpiAreaTable_IDX2
saHpiAreaTable_context * saHpiAreaTable_get( const char *name, int len );
#endif

#ifdef __cplusplus
};
#endif

#endif /** SAHPIAREATABLE_H */
