/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.array-user.conf,v 5.15.2.1 2003/02/27 05:59:41 rstory Exp $
 *
 * $Id$
 *
 * Yes, there is lots of code here that you might not use. But it is much
 * easier to remove code than to add it!
 */
#ifndef SAHPIEVENTTABLE_H
#define SAHPIEVENTTABLE_H

#ifdef __cplusplus
extern          "C" {
#endif


#include <net-snmp/net-snmp-config.h>
#include <net-snmp/library/container.h>
#include <net-snmp/agent/table_array.h>

        /** Index saHpiEventIndex is internal */
        /** Index saHpiEventTimestamp is internal */

    typedef struct saHpiEventTable_context_s {
        netsnmp_index   index;
                         /** THIS MUST BE FIRST!!! */

    /*************************************************************
     * You can store data internally in this structure.
     *
     * TODO: You will probably have to fix a few types here...
     */
        /** UNSIGNED32 = ASN_UNSIGNED */
        unsigned long   saHpiEventIndex;

        /** INTEGER = ASN_INTEGER */
        long            saHpiEventType;

        /** TimeStamp = ASN_TIMETICKS */
        unsigned long   saHpiEventTimestamp;

        /** INTEGER = ASN_INTEGER */
        long            saHpiEventSeverity;

        /** UNSIGNED32 = ASN_UNSIGNED */
        unsigned long   saHpiEventSensorNum;

        /** INTEGER = ASN_INTEGER */
        long            saHpiEventSensorType;

        /** INTEGER = ASN_INTEGER */
        long            saHpiEventCategory;

        /** TruthValue = ASN_INTEGER */
        long            saHpiEventAssertion;

        /** INTEGER = ASN_INTEGER */
        long            saHpiEventStateCategoryUnspecified;

        /** INTEGER = ASN_INTEGER */
        long            saHpiEventStateCategoryThreshold;

        /** INTEGER = ASN_INTEGER */
        long            saHpiEventStateCategoryUsage;

        /** INTEGER = ASN_INTEGER */
        long            saHpiEventStateCategoryState;

        /** INTEGER = ASN_INTEGER */
        long            saHpiEventStateCategoryPredFail;

        /** INTEGER = ASN_INTEGER */
        long            saHpiEventStateCategoryLimit;

        /** INTEGER = ASN_INTEGER */
        long            saHpiEventStateCategoryPerformance;

        /** INTEGER = ASN_INTEGER */
        long            saHpiEventStateCategorySeverity;

        /** INTEGER = ASN_INTEGER */
        long            saHpiEventStateCategoryPresence;

        /** INTEGER = ASN_INTEGER */
        long            saHpiEventStateCategoryEnable;

        /** INTEGER = ASN_INTEGER */
        long            saHpiEventStateCategoryAvailability;

        /** INTEGER = ASN_INTEGER */
        long            saHpiEventStateCategoryRedundancy;

        /** INTEGER = ASN_INTEGER */
        long            saHpiEventStateCategoryUser;

        /** INTEGER = ASN_INTEGER */
        long            saHpiEventStateCategoryGeneric;

        /** UNSIGNED32 = ASN_UNSIGNED */
        unsigned long   saHpiSensorOptionalData;

        /** UNSIGNED32 = ASN_UNSIGNED */
        unsigned long   saHpiEventTriggerReadingType;

        /** UNSIGNED32 = ASN_UNSIGNED */
        unsigned long   saHpiEventTriggerReadingRaw;

        /** INTEGER = ASN_INTEGER */
        long            saHpiEventTriggerReadingInterpretedType;

        /** OCTETSTR = ASN_OCTET_STR */
        unsigned char   saHpiEventTriggerReadingInterpreted[65535];
        long            saHpiEventTriggerReadingInterpreted_len;

        /** OCTETSTR = ASN_OCTET_STR */
        unsigned char   saHpiEventTriggerReadingEventState[65535];
        long            saHpiEventTriggerReadingEventState_len;

        /** UNSIGNED32 = ASN_UNSIGNED */
        unsigned long   saHpiEventTriggerThresholdType;

        /** UNSIGNED32 = ASN_UNSIGNED */
        unsigned long   saHpiEventTriggerThresholdRaw;

        /** INTEGER = ASN_INTEGER */
        long            saHpiEventTriggerThresholdInterpretedType;

        /** OCTETSTR = ASN_OCTET_STR */
        unsigned char   saHpiEventTriggerThresholdInterpreted[65535];
        long            saHpiEventTriggerThresholdInterpreted_len;

        /** OCTETSTR = ASN_OCTET_STR */
        unsigned char   saHpiEventTriggerThresholdEventState[65535];
        long            saHpiEventTriggerThresholdEventState_len;

        /** UNSIGNED32 = ASN_UNSIGNED */
        unsigned long   saHpiEventPreviousState;

        /** UNSIGNED32 = ASN_UNSIGNED */
        unsigned long   saHpiEventOem;

        /** UNSIGNED32 = ASN_UNSIGNED */
        unsigned long   saHpiEventSensorSpecific;

        /** INTEGER = ASN_INTEGER */
        long            saHpiEventHotSwapState;

        /** INTEGER = ASN_INTEGER */
        long            saHpiEventPreviousHotSwapState;

        /** UNSIGNED32 = ASN_UNSIGNED */
        unsigned long   saHpiEventWatchdogNum;

        /** INTEGER = ASN_INTEGER */
        long            saHpiEventWatchdogAction;

        /** INTEGER = ASN_INTEGER */
        long            saHpiEventWatchdogPreTimerAction;

        /** INTEGER = ASN_INTEGER */
        long            saHpiEventWatchdogUse;

        /** UNSIGNED32 = ASN_UNSIGNED */
        unsigned long   saHpiEventOemManufacturerIdT;

        /** OCTETSTR = ASN_OCTET_STR */
        unsigned char   saHpiEventOemEventData[65535];
        long            saHpiEventOemEventData_len;

        /** OCTETSTR = ASN_OCTET_STR */
        unsigned char   saHpiEventUserEventData[65535];
        long            saHpiEventUserEventData_len;


        /*
         * OR
         *
         * Keep a pointer to your data
         */
        void           *data;

        /*
         *add anything else you want here
         */

    } saHpiEventTable_context;

/*************************************************************
 * function declarations
 */
    void            init_saHpiEventTable(void);
    void            initialize_table_saHpiEventTable(void);
    const saHpiEventTable_context *saHpiEventTable_get_by_idx(netsnmp_index
                                                              *);
    const saHpiEventTable_context
        *saHpiEventTable_get_by_idx_rs(netsnmp_index *, int row_status);
    int             saHpiEventTable_get_value(netsnmp_request_info *,
                                              netsnmp_index *,
                                              netsnmp_table_request_info
                                              *);


/*************************************************************
 * oid declarations
 */
    extern oid      saHpiEventTable_oid[];
    extern size_t   saHpiEventTable_oid_len;

#define saHpiEventTable_TABLE_OID 1,3,6,1,3,90,2,1,2

/*************************************************************
 * column number definitions for table saHpiEventTable
 */
#define COLUMN_SAHPIEVENTINDEX 1
#define COLUMN_SAHPIEVENTTYPE 2
#define COLUMN_SAHPIEVENTTIMESTAMP 3
#define COLUMN_SAHPIEVENTSEVERITY 4
#define COLUMN_SAHPIEVENTSENSORNUM 5
#define COLUMN_SAHPIEVENTSENSORTYPE 6
#define COLUMN_SAHPIEVENTCATEGORY 7
#define COLUMN_SAHPIEVENTASSERTION 8
#define COLUMN_SAHPIEVENTSTATECATEGORYUNSPECIFIED 9
#define COLUMN_SAHPIEVENTSTATECATEGORYTHRESHOLD 10
#define COLUMN_SAHPIEVENTSTATECATEGORYUSAGE 11
#define COLUMN_SAHPIEVENTSTATECATEGORYSTATE 12
#define COLUMN_SAHPIEVENTSTATECATEGORYPREDFAIL 13
#define COLUMN_SAHPIEVENTSTATECATEGORYLIMIT 14
#define COLUMN_SAHPIEVENTSTATECATEGORYPERFORMANCE 15
#define COLUMN_SAHPIEVENTSTATECATEGORYSEVERITY 16
#define COLUMN_SAHPIEVENTSTATECATEGORYPRESENCE 17
#define COLUMN_SAHPIEVENTSTATECATEGORYENABLE 18
#define COLUMN_SAHPIEVENTSTATECATEGORYAVAILABILITY 19
#define COLUMN_SAHPIEVENTSTATECATEGORYREDUNDANCY 20
#define COLUMN_SAHPIEVENTSTATECATEGORYUSER 21
#define COLUMN_SAHPIEVENTSTATECATEGORYGENERIC 22
#define COLUMN_SAHPISENSOROPTIONALDATA 23
#define COLUMN_SAHPIEVENTTRIGGERREADINGTYPE 24
#define COLUMN_SAHPIEVENTTRIGGERREADINGRAW 25
#define COLUMN_SAHPIEVENTTRIGGERREADINGINTERPRETEDTYPE 26
#define COLUMN_SAHPIEVENTTRIGGERREADINGINTERPRETED 27
#define COLUMN_SAHPIEVENTTRIGGERREADINGEVENTSTATE 28
#define COLUMN_SAHPIEVENTTRIGGERTHRESHOLDTYPE 29
#define COLUMN_SAHPIEVENTTRIGGERTHRESHOLDRAW 30
#define COLUMN_SAHPIEVENTTRIGGERTHRESHOLDINTERPRETEDTYPE 31
#define COLUMN_SAHPIEVENTTRIGGERTHRESHOLDINTERPRETED 32
#define COLUMN_SAHPIEVENTTRIGGERTHRESHOLDEVENTSTATE 33
#define COLUMN_SAHPIEVENTPREVIOUSSTATE 34
#define COLUMN_SAHPIEVENTOEM 35
#define COLUMN_SAHPIEVENTSENSORSPECIFIC 36
#define COLUMN_SAHPIEVENTHOTSWAPSTATE 37
#define COLUMN_SAHPIEVENTPREVIOUSHOTSWAPSTATE 38
#define COLUMN_SAHPIEVENTWATCHDOGNUM 39
#define COLUMN_SAHPIEVENTWATCHDOGACTION 40
#define COLUMN_SAHPIEVENTWATCHDOGPRETIMERACTION 41
#define COLUMN_SAHPIEVENTWATCHDOGUSE 42
#define COLUMN_SAHPIEVENTOEMMANUFACTURERIDT 43
#define COLUMN_SAHPIEVENTOEMEVENTDATA 44
#define COLUMN_SAHPIEVENTUSEREVENTDATA 45
#define saHpiEventTable_COL_MIN 1
#define saHpiEventTable_COL_MAX 45

    /*
     * comment out the following line if you don't handle SET-REQUEST for saHpiEventTable 
     */
#define saHpiEventTable_SET_HANDLING

    /*
     * comment out the following line if you can't create new rows 
     */
#define saHpiEventTable_ROW_CREATION

    /*
     * comment out the following line if you don't want the secondary index 
     */
#define saHpiEventTable_IDX2

#ifdef saHpiEventTable_SET_HANDLING

    int             saHpiEventTable_extract_index(saHpiEventTable_context *
                                                  ctx,
                                                  netsnmp_index * hdr);

    void            saHpiEventTable_set_reserve1(netsnmp_request_group *);
    void            saHpiEventTable_set_reserve2(netsnmp_request_group *);
    void            saHpiEventTable_set_action(netsnmp_request_group *);
    void            saHpiEventTable_set_commit(netsnmp_request_group *);
    void            saHpiEventTable_set_free(netsnmp_request_group *);
    void            saHpiEventTable_set_undo(netsnmp_request_group *);

    saHpiEventTable_context
        *saHpiEventTable_duplicate_row(saHpiEventTable_context *);
    netsnmp_index  *saHpiEventTable_delete_row(saHpiEventTable_context *);

    int             saHpiEventTable_can_delete(saHpiEventTable_context *
                                               undo_ctx,
                                               saHpiEventTable_context *
                                               row_ctx,
                                               netsnmp_request_group * rg);


#ifdef saHpiEventTable_ROW_CREATION
    saHpiEventTable_context *saHpiEventTable_create_row(netsnmp_index *);
#endif
#endif

#ifdef saHpiEventTable_IDX2
    saHpiEventTable_context *saHpiEventTable_get(const char *name,
                                                 int len);
#endif

#ifdef __cplusplus
};
#endif

#endif /** SAHPIEVENTTABLE_H */
