/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.array-user.conf,v 5.18.2.2 2004/02/09 18:21:47 rstory Exp $
 *
 * $Id$
 *
 * Yes, there is lots of code here that you might not use. But it is much
 * easier to remove code than to add it!
 */
#ifndef SAHPISENSOREVENTTABLE_H
#define SAHPISENSOREVENTTABLE_H

#ifdef __cplusplus
extern "C" {
#endif

    
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/library/container.h>
#include <net-snmp/agent/table_array.h>

        /** Index saHpiDomainId is external */
        /** Index saHpiResourceId is external */
        /** Index saHpiSensorNum is external */
        /** Index saHpiEventSeverity is external */
        /** Index saHpiSensorEventEntryId is internal */

typedef struct saHpiSensorEventTable_context_s {
    netsnmp_index index; /** THIS MUST BE FIRST!!! */

    /*************************************************************
     * You can store data internally in this structure.
     *
     * TODO: You will probably have to fix a few types here...
     */
    /** TODO: add storage for external index(s)! */
        /** SaHpiEntryId = ASN_UNSIGNED */
            unsigned long saHpiSensorEventEntryId;

        /** SaHpiTime = ASN_COUNTER64 */
    /** TODO: Is this type correct? */
            long saHpiSensorEventTimestamp;

        /** SaHpiSensorType = ASN_INTEGER */
            long saHpiSensorEventType;

        /** SaHpiEventCategory = ASN_INTEGER */
            long saHpiSensorEventCategory;

        /** TruthValue = ASN_INTEGER */
            long saHpiSensorEventAssertion;

        /** SaHpiEventState = ASN_OCTET_STR */
            unsigned char saHpiSensorEventState[65535];
            long saHpiSensorEventState_len;

        /** SaHpiOptionalData = ASN_OCTET_STR */
            unsigned char saHpiSensorEventOptionalData[65535];
            long saHpiSensorEventOptionalData_len;

        /** SaHpiSensorReadingType = ASN_INTEGER */
            long saHpiSensorEventTriggerReadingType;

        /** SaHpiSensorReadingValue = ASN_OCTET_STR */
            unsigned char saHpiSensorEventTriggerReading[65535];
            long saHpiSensorEventTriggerReading_len;

        /** SaHpiSensorReadingType = ASN_INTEGER */
            long saHpiSensorEventTriggerThresholdType;

        /** SaHpiSensorReadingValue = ASN_OCTET_STR */
            unsigned char saHpiSensorEventTriggerThreshold[65535];
            long saHpiSensorEventTriggerThreshold_len;

        /** SaHpiEventState = ASN_OCTET_STR */
            unsigned char saHpiSensorEventPreviousState[65535];
            long saHpiSensorEventPreviousState_len;

        /** SaHpiEventState = ASN_OCTET_STR */
            unsigned char saHpiSensorEventCurrentState[65535];
            long saHpiSensorEventCurrentState_len;

        /** UNSIGNED32 = ASN_UNSIGNED */
            unsigned long saHpiSensorEventOem;

        /** UNSIGNED32 = ASN_UNSIGNED */
            unsigned long saHpiSensorEventSpecific;


    /*
     * OR
     *
     * Keep a pointer to your data
     */
    void * data;

    /*
     *add anything else you want here
     */

} saHpiSensorEventTable_context;

/*************************************************************
 * function declarations
 */
void init_saHpiSensorEventTable(void);
void initialize_table_saHpiSensorEventTable(void);
const saHpiSensorEventTable_context * saHpiSensorEventTable_get_by_idx(netsnmp_index *);
const saHpiSensorEventTable_context * saHpiSensorEventTable_get_by_idx_rs(netsnmp_index *,
                                        int row_status);
int saHpiSensorEventTable_get_value(netsnmp_request_info *, netsnmp_index *, netsnmp_table_request_info *);


/*************************************************************
 * oid declarations
 */
extern oid saHpiSensorEventTable_oid[];
extern size_t saHpiSensorEventTable_oid_len;

#define saHpiSensorEventTable_TABLE_OID 1,3,6,1,4,1,18568,2,1,1,3,1,12
    
/*************************************************************
 * column number definitions for table saHpiSensorEventTable
 */
#define COLUMN_SAHPISENSOREVENTENTRYID 1
#define COLUMN_SAHPISENSOREVENTTIMESTAMP 2
#define COLUMN_SAHPISENSOREVENTTYPE 3
#define COLUMN_SAHPISENSOREVENTCATEGORY 4
#define COLUMN_SAHPISENSOREVENTASSERTION 5
#define COLUMN_SAHPISENSOREVENTSTATE 6
#define COLUMN_SAHPISENSOREVENTOPTIONALDATA 7
#define COLUMN_SAHPISENSOREVENTTRIGGERREADINGTYPE 8
#define COLUMN_SAHPISENSOREVENTTRIGGERREADING 9
#define COLUMN_SAHPISENSOREVENTTRIGGERTHRESHOLDTYPE 10
#define COLUMN_SAHPISENSOREVENTTRIGGERTHRESHOLD 11
#define COLUMN_SAHPISENSOREVENTPREVIOUSSTATE 12
#define COLUMN_SAHPISENSOREVENTCURRENTSTATE 13
#define COLUMN_SAHPISENSOREVENTOEM 14
#define COLUMN_SAHPISENSOREVENTSPECIFIC 15
#define saHpiSensorEventTable_COL_MIN 2
#define saHpiSensorEventTable_COL_MAX 15

/* comment out the following line if you don't handle SET-REQUEST for saHpiSensorEventTable */
#define saHpiSensorEventTable_SET_HANDLING

/* comment out the following line if you can't create new rows */
#define saHpiSensorEventTable_ROW_CREATION

/* comment out the following line if you don't want the secondary index */
#define saHpiSensorEventTable_IDX2

/* uncommend the following line if you allow modifications to an
 * active row */
/** define saHpiSensorEventTable_CAN_MODIFY_ACTIVE_ROW */

#ifdef saHpiSensorEventTable_SET_HANDLING

int saHpiSensorEventTable_extract_index( saHpiSensorEventTable_context * ctx, netsnmp_index * hdr );

void saHpiSensorEventTable_set_reserve1( netsnmp_request_group * );
void saHpiSensorEventTable_set_reserve2( netsnmp_request_group * );
void saHpiSensorEventTable_set_action( netsnmp_request_group * );
void saHpiSensorEventTable_set_commit( netsnmp_request_group * );
void saHpiSensorEventTable_set_free( netsnmp_request_group * );
void saHpiSensorEventTable_set_undo( netsnmp_request_group * );

saHpiSensorEventTable_context * saHpiSensorEventTable_duplicate_row( saHpiSensorEventTable_context* );
netsnmp_index * saHpiSensorEventTable_delete_row( saHpiSensorEventTable_context* );

int saHpiSensorEventTable_can_activate(saHpiSensorEventTable_context *undo_ctx,
                      saHpiSensorEventTable_context *row_ctx,
                      netsnmp_request_group * rg);
int saHpiSensorEventTable_can_deactivate(saHpiSensorEventTable_context *undo_ctx,
                        saHpiSensorEventTable_context *row_ctx,
                        netsnmp_request_group * rg);
int saHpiSensorEventTable_can_delete(saHpiSensorEventTable_context *undo_ctx,
                    saHpiSensorEventTable_context *row_ctx,
                    netsnmp_request_group * rg);
    
    
#ifdef saHpiSensorEventTable_ROW_CREATION
saHpiSensorEventTable_context * saHpiSensorEventTable_create_row( netsnmp_index* );
#endif
#endif

#ifdef saHpiSensorEventTable_IDX2
saHpiSensorEventTable_context * saHpiSensorEventTable_get( const char *name, int len );
#endif

#ifdef __cplusplus
};
#endif

#endif /** SAHPISENSOREVENTTABLE_H */
