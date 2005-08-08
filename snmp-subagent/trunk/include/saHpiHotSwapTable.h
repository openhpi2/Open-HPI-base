/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.array-user.conf,v 5.18.2.2 2004/02/09 18:21:47 rstory Exp $
 *
 * $Id$
 *
 * Yes, there is lots of code here that you might not use. But it is much
 * easier to remove code than to add it!
 */
#ifndef SAHPIHOTSWAPTABLE_H
#define SAHPIHOTSWAPTABLE_H

#ifdef __cplusplus
extern "C" {
#endif

    
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/library/container.h>
#include <net-snmp/agent/table_array.h>

        /** Index saHpiDomainId is external */
        /** Index saHpiResourceId is external */
        /** Index saHpiResourceIsHistorical is external */

typedef struct saHpiHotSwapTable_context_s {
    netsnmp_index index; /** THIS MUST BE FIRST!!! */

    /*************************************************************
     * You can store data internally in this structure.
     *
     * TODO: You will probably have to fix a few types here...
     */
    /** TODO: add storage for external index(s)! */
        /** INTEGER = ASN_INTEGER */
            long saHpiHotSwapIndicator;

        /** SaHpiHotSwapState = ASN_INTEGER */
            long saHpiHotSwapState;

        /** SaHpiTime = ASN_COUNTER64 */
    /** TODO: Is this type correct? */
            long saHpiHotSwapExtractTimeout;

        /** INTEGER = ASN_INTEGER */
            long saHpiHotSwapActionRequest;

        /** INTEGER = ASN_INTEGER */
            long saHpiHotSwapPolicyCancel;

        /** INTEGER = ASN_INTEGER */
            long saHpiHotSwapResourceRequest;


    /*
     * OR
     *
     * Keep a pointer to your data
     */
    void * data;

    /*
     *add anything else you want here
     */

} saHpiHotSwapTable_context;

/*************************************************************
 * function declarations
 */
void init_saHpiHotSwapTable(void);
void initialize_table_saHpiHotSwapTable(void);
const saHpiHotSwapTable_context * saHpiHotSwapTable_get_by_idx(netsnmp_index *);
const saHpiHotSwapTable_context * saHpiHotSwapTable_get_by_idx_rs(netsnmp_index *,
                                        int row_status);
int saHpiHotSwapTable_get_value(netsnmp_request_info *, netsnmp_index *, netsnmp_table_request_info *);

/*************************************************************
 * set funtions
 */
int hot_swap_indicator_set (saHpiHotSwapTable_context *row_ctx);
int hot_swap_state_set (saHpiHotSwapTable_context *row_ctx);

/*************************************************************
 * function declarations
 */
SaErrorT populate_hotswap(SaHpiSessionIdT sessionid, 
                           SaHpiRptEntryT *rpt_entry,
                           oid *child_oid, size_t child_oid_len); 


/*************************************************************
 * oid declarations
 */
extern oid saHpiHotSwapTable_oid[];
extern size_t saHpiHotSwapTable_oid_len;

#define saHpiHotSwapTable_TABLE_OID 1,3,6,1,4,1,18568,2,1,1,2,11,3
    
/*************************************************************
 * column number definitions for table saHpiHotSwapTable
 */

/* Number of table Indexes */
#define HOTSWAP_INDEX_NR 3
#define saHpiDomainId_INDEX 0
#define saHpiResourceId_INDEX 1
#define saHpiResourceIsHistorical_INDEX 2
 
#define COLUMN_SAHPIHOTSWAPINDICATOR 1
#define COLUMN_SAHPIHOTSWAPSTATE 2
#define COLUMN_SAHPIHOTSWAPEXTRACTTIMEOUT 3
#define COLUMN_SAHPIHOTSWAPACTIONREQUEST 4
#define COLUMN_SAHPIHOTSWAPPOLICYCANCEL 5
#define COLUMN_SAHPIHOTSWAPRESOURCEREQUEST 6
#define saHpiHotSwapTable_COL_MIN 1
#define saHpiHotSwapTable_COL_MAX 6

int saHpiHotSwapTable_extract_index( saHpiHotSwapTable_context * ctx, netsnmp_index * hdr );

void saHpiHotSwapTable_set_reserve1( netsnmp_request_group * );
void saHpiHotSwapTable_set_reserve2( netsnmp_request_group * );
void saHpiHotSwapTable_set_action( netsnmp_request_group * );
void saHpiHotSwapTable_set_commit( netsnmp_request_group * );
void saHpiHotSwapTable_set_free( netsnmp_request_group * );
void saHpiHotSwapTable_set_undo( netsnmp_request_group * );

saHpiHotSwapTable_context * saHpiHotSwapTable_duplicate_row( saHpiHotSwapTable_context* );
netsnmp_index * saHpiHotSwapTable_delete_row( saHpiHotSwapTable_context* );

int saHpiHotSwapTable_can_activate(saHpiHotSwapTable_context *undo_ctx,
                      saHpiHotSwapTable_context *row_ctx,
                      netsnmp_request_group * rg);
int saHpiHotSwapTable_can_deactivate(saHpiHotSwapTable_context *undo_ctx,
                        saHpiHotSwapTable_context *row_ctx,
                        netsnmp_request_group * rg);
int saHpiHotSwapTable_can_delete(saHpiHotSwapTable_context *undo_ctx,
                    saHpiHotSwapTable_context *row_ctx,
                    netsnmp_request_group * rg);
    
    
saHpiHotSwapTable_context * saHpiHotSwapTable_create_row( netsnmp_index* );

saHpiHotSwapTable_context * saHpiHotSwapTable_get( const char *name, int len );

#ifdef __cplusplus
};
#endif

#endif /** SAHPIHOTSWAPTABLE_H */
