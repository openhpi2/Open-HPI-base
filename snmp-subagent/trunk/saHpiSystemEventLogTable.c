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
 *       : mib2c.array-user.conf,v 5.15.2.1 2003/02/27 05:59:41 rstory Exp $
 *
 * $Id$
 * 
 */
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

#include <net-snmp/library/snmp_assert.h>

#include "saHpiSystemEventLogTable.h"
#include <saHpiEventTable.h>

static netsnmp_handler_registration *my_handler = NULL;
static netsnmp_table_array_callbacks cb;

static oid             saHpiSystemEventLogTable_oid[] =
    { saHpiSystemEventLogTable_TABLE_OID };
static size_t          saHpiSystemEventLogTable_oid_len =
OID_LENGTH(saHpiSystemEventLogTable_oid);

static oid      saHpiSystemEventLogEntries_oid[] =
  { 1, 3, 6, 1, 3, 90, 2, 2, 1, 0 };
static oid      saHpiSystemEventLogSize_oid[] =
  { 1, 3, 6, 1, 3, 90, 2, 2, 2, 0 }; 
static oid      saHpiSystemEventLogUpdateTimestamp_oid[] =
  { 1, 3, 6, 1, 3, 90, 2, 2, 3, 0 };
static oid      saHpiSystemEventLogCurrentTime_oid[] =
  { 1, 3, 6, 1, 3, 90, 2, 2, 4, 0 };
static oid      saHpiSystemEventLogEnabled_oid[] =
  { 1, 3, 6, 1, 3, 90, 2, 2, 5, 0 };
static oid      saHpiSystemEventLogOverflowFlag_oid[] =
  { 1, 3, 6, 1, 3, 90, 2, 2, 6, 0 };
static oid      saHpiSystemEventLogOverflowAction_oid[] =
  { 1, 3, 6, 1, 3, 90, 2, 2, 7, 0 };
static oid      saHpiSystemEventLogDeleteEntrySupported_oid[] =
  { 1, 3, 6, 1, 3, 90, 2, 2, 8, 0 };
static u_long event_log_entries = 0;
static u_long event_log_size = 0;
static long long event_log_update_timestamp = SAHPI_TIME_UNSPECIFIED;
static long long event_log_current_timestamp = SAHPI_TIME_UNSPECIFIED;
static int event_log_enabled = 0;
static int event_log_overflow_flag = 0;
static int event_log_overflow_action = 0;
static int event_log_delete_entry_supported = 0;

int populate_sel(SaHpiRptEntryT *rpt_entry){
  
  SaErrorT     err;
  SaHpiSessionIdT session_id;
  SaHpiRdrT     rdr_entry;
  SaHpiSelEntryIdT entry_id;
  SaHpiSelEntryIdT next_entry_id;
  SaHpiSelEntryIdT prev_entry_id;
  SaHpiSelInfoT info;
  SaHpiSelEntryT sel;
  oid sel_oid[3];
  oid child_oid[MAX_OID_LEN];
  size_t child_oid_len = 0;
  int rc;
  netsnmp_index sel_index;
  saHpiSystemEventLogTable_context *sel_context;
  //  long backup_count = event_log_entries;

  DEBUGMSGTL((AGENT,"\t--- populate_sel: Entry\n"));
  if (rpt_entry) {
    rc = getSaHpiSession(&session_id);
    if (rc != AGENT_ERR_NOERROR) 
      return rc;
    
    err = saHpiEventLogInfoGet( session_id, rpt_entry->ResourceId, &info);
    if (err != SA_OK) 
      return AGENT_ERR_OPERATION;

    //event_log_entries = info.Entries;
    event_log_size = info.Size;
    event_log_update_timestamp = info.UpdateTimestamp;
    event_log_current_timestamp = info.CurrentTime;
    event_log_enabled = (info.Enabled == SAHPI_TRUE) ? MIB_TRUE : MIB_FALSE;
    event_log_overflow_flag = (info.OverflowFlag == SAHPI_TRUE) ? MIB_TRUE : MIB_FALSE;
    event_log_overflow_action = info.OverflowAction + 1;
    event_log_delete_entry_supported = info.DeleteEntrySupported;
    // Fill the data

    entry_id = SAHPI_OLDEST_ENTRY;
    while ((err == SA_OK) && (entry_id != SAHPI_NO_MORE_ENTRIES)) {

      err = saHpiEventLogEntryGet(session_id,
				  rpt_entry->ResourceId,
				  entry_id,
				  &prev_entry_id,
				  &next_entry_id,
				  &sel,
				  &rdr_entry,
				  NULL);

      if (err == SA_OK) {
	// The MIB containst the order of indexes
	sel_oid[0] = rpt_entry->DomainId;
	sel_oid[1] = rpt_entry->ResourceId;
	sel_oid[2] = sel.EntryId;
	DEBUGMSGTL((AGENT,"Request for %d, %d, %d\n",
		    sel_oid[0], sel_oid[1], sel_oid[2]));

	sel_index.oids = (oid *) &sel_oid;
	sel_index.len = 3;
	sel_context = NULL;
	sel_context = CONTAINER_FIND(cb.container, &sel_index);

	if (!sel_context) {
	  // New entry. Add it
	  sel_context = saHpiSystemEventLogTable_create_row(&sel_index);
	}
	// Now create the 'event' and get its OID to be copied to our
	// SystemEventLogged OID

	populate_event(sel.EntryId,
		       &sel.Event, 
		       rpt_entry, 
		       &rdr_entry, 
		       child_oid, &child_oid_len);
	
	if (saHpiSystemEventLogTable_modify_context(&sel,
						    rpt_entry,
						    child_oid, child_oid_len,
						    sel_context)
	    == AGENT_NEW_ENTRY) {
	  
	  CONTAINER_INSERT(cb.container, sel_context);
	  event_log_entries = CONTAINER_SIZE(cb.container);
	  // traps
	}

	prev_entry_id = entry_id;
	entry_id = next_entry_id;
      }
				  
    }
  }
  DEBUGMSGTL((AGENT,"\t--- populate_sel: Exit\n"));
  return AGENT_ERR_NOERROR;
}


int
saHpiSystemEventLogTable_modify_context(SaHpiSelEntryT *sel,
					SaHpiRptEntryT *rpt,
					oid *event_entry, 
					size_t event_entry_oid_len,
					saHpiSystemEventLogTable_context *ctx) {
  long hash;

  if (sel && ctx) {
    hash = calculate_hash_value(sel, sizeof(SaHpiSelEntryT));

    DEBUGMSGTL((AGENT," Hash value: %d, in ctx: %d\n", hash, ctx->hash));

    if (ctx->hash != 0) {
      // Only do the check if the hash value is something else than zero.
      // 'zero' value is only for newly created records, and in some
      // rare instances when the hash has rolled to zero - in which
      // case we will just consider the worst-case scenario and update
      // the record and not trust the hash value.
      if (hash == ctx->hash) {
	// The same data. No need to change.
	return AGENT_ENTRY_EXIST;
      }
    }

    ctx->hash = hash;
    ctx->resource_id = rpt->ResourceId;
    ctx->domain_id = rpt->DomainId;
    ctx->saHpiSystemEventLogEntryId = sel->EntryId;
    ctx->saHpiSystemEventLogTimestamp = sel->Timestamp;
    ctx->saHpiSystemEventLogged_len = event_entry_oid_len * sizeof(oid);
    memcpy(ctx->saHpiSystemEventLogged, event_entry, ctx->saHpiSystemEventLogged_len);

    return AGENT_NEW_ENTRY;
  }
  return AGENT_ERR_NULL_DATA;
}
/************************************************************
 * keep binary tree to find context by name
 */
static int      saHpiSystemEventLogTable_cmp(const void *lhs,
                                             const void *rhs);

/************************************************************
 * compare two context pointers here. Return -1 if lhs < rhs,
 * 0 if lhs == rhs, and 1 if lhs > rhs.
 */
static int
saHpiSystemEventLogTable_cmp(const void *lhs, const void *rhs)
{
    saHpiSystemEventLogTable_context *context_l =
        (saHpiSystemEventLogTable_context *) lhs;
    saHpiSystemEventLogTable_context *context_r =
        (saHpiSystemEventLogTable_context *) rhs;
    int rc;
    DEBUGMSGTL((AGENT,"saHpiSystemEventLogTable_cmp: Called\n"));

    if (context_l->domain_id < context_r->domain_id)
      return -1;
    rc = (context_l->domain_id == context_r->domain_id) ? 0: 1;
    
    if (rc != 0) 
	return rc;

    if (context_l->resource_id < context_r->resource_id)
	return -1;

    rc =(context_l->resource_id == context_r->resource_id) ? 0: 1;

    if (rc != 0)
      return rc;

    if (context_l->saHpiSystemEventLogEntryId < context_r->saHpiSystemEventLogEntryId)
	return -1;
    return (context_l->saHpiSystemEventLogEntryId == context_r->saHpiSystemEventLogEntryId) ? 0: 1;
}



/************************************************************
 * the *_row_copy routine
 */
static int
saHpiSystemEventLogTable_row_copy(saHpiSystemEventLogTable_context * dst,
                                  saHpiSystemEventLogTable_context * src)
{
    if (!dst || !src)
        return 1;

    /*
     * copy index, if provided
     */
    if (dst->index.oids)
        free(dst->index.oids);
    if (snmp_clone_mem((void *) &dst->index.oids, src->index.oids,
                       src->index.len * sizeof(oid))) {
        dst->index.oids = NULL;
        return 1;
    }
    dst->index.len = src->index.len;


    /*
     * copy components into the context structure
     */
    dst->saHpiSystemEventLogEntryId = src->saHpiSystemEventLogEntryId;

    dst->saHpiSystemEventLogTimestamp = src->saHpiSystemEventLogTimestamp;

    memcpy(src->saHpiSystemEventLogged, dst->saHpiSystemEventLogged,
           src->saHpiSystemEventLogged_len);
    dst->saHpiSystemEventLogged_len = src->saHpiSystemEventLogged_len;

    dst->resource_id = src->resource_id;
    dst->hash = src->hash;
    dst->domain_id = src->domain_id;
    return 0;
}


/*
 * the *_extract_index routine
 */
int
saHpiSystemEventLogTable_extract_index(saHpiSystemEventLogTable_context *
                                       ctx, netsnmp_index * hdr)
{
    /*
     * temporary local storage for extracting oid index
     */
    netsnmp_variable_list var_saHpiDomainID;
    netsnmp_variable_list var_saHpiResourceID;
    netsnmp_variable_list var_saHpiSystemEventLogEntryId;
    int             err;

    /*
     * copy index, if provided
     */
    if (hdr) {
        netsnmp_assert(ctx->index.oids == NULL);
        if (snmp_clone_mem((void *) &ctx->index.oids, hdr->oids,
                           hdr->len * sizeof(oid))) {
            return -1;
        }
        ctx->index.len = hdr->len;
    }


    memset(&var_saHpiDomainID, 0x00, sizeof(var_saHpiDomainID));
    var_saHpiDomainID.type = ASN_UNSIGNED;
    var_saHpiDomainID.next_variable = &var_saHpiResourceID;

    memset(&var_saHpiResourceID, 0x00, sizeof(var_saHpiResourceID));
    var_saHpiResourceID.type = ASN_UNSIGNED;
    var_saHpiResourceID.next_variable = &var_saHpiSystemEventLogEntryId;

    memset(&var_saHpiSystemEventLogEntryId, 0x00,
           sizeof(var_saHpiSystemEventLogEntryId));
    var_saHpiSystemEventLogEntryId.type = ASN_UNSIGNED;
    var_saHpiSystemEventLogEntryId.next_variable = NULL;


    /*
     * parse the oid into the individual components
     */
    err = parse_oid_indexes(hdr->oids, hdr->len, &var_saHpiDomainID);

    if (err == SNMP_ERR_NOERROR) {
       
              /** skipping external index saHpiDomainID */

              /** skipping external index saHpiResourceID */

        ctx->saHpiSystemEventLogEntryId =
            *var_saHpiSystemEventLogEntryId.val.integer;

    }

    /*
     * parsing may have allocated memory. free it.
     */
    snmp_reset_var_buffers(&var_saHpiDomainID);

    return err;
}

/************************************************************
 * the *_can_delete routine is called to determine if a row
 * can be deleted.
 *
 * return 1 if the row can be deleted
 * return 0 if the row cannot be deleted
 */
int
saHpiSystemEventLogTable_can_delete(saHpiSystemEventLogTable_context *
                                    undo_ctx,
                                    saHpiSystemEventLogTable_context *
                                    row_ctx, netsnmp_request_group * rg)
{

    /*
     * TODO: check for other deletion requirements here
     */
    return 1;
}

/************************************************************
 * the *_create_row routine is called by the table handler
 * to create a new row for a given index. If you need more
 * information (such as column values) to make a decision
 * on creating rows, you must create an initial row here
 * (to hold the column values), and you can examine the
 * situation in more detail in the *_set_reserve1 or later
 * states of set processing. Simple check for a NULL undo_ctx
 * in those states and do detailed creation checking there.
 *
 * returns a newly allocated saHpiSystemEventLogTable_context
 *   structure if the specified indexes are not illegal
 * returns NULL for errors or illegal index values.
 */
saHpiSystemEventLogTable_context *
saHpiSystemEventLogTable_create_row(netsnmp_index * hdr)
{
    saHpiSystemEventLogTable_context *ctx =
        SNMP_MALLOC_TYPEDEF(saHpiSystemEventLogTable_context);

    if (!ctx)
        return NULL;

    if (saHpiSystemEventLogTable_extract_index(ctx, hdr)) {
        free(ctx->index.oids);
        free(ctx);
        return NULL;
    }

    ctx->hash =0;

    return ctx;
}


/************************************************************
 * the *_duplicate row routine
 */
saHpiSystemEventLogTable_context *
saHpiSystemEventLogTable_duplicate_row(saHpiSystemEventLogTable_context *
                                       row_ctx)
{
    saHpiSystemEventLogTable_context *dup;

    if (!row_ctx)
        return NULL;

    dup = SNMP_MALLOC_TYPEDEF(saHpiSystemEventLogTable_context);
    if (!dup)
        return NULL;

    if (saHpiSystemEventLogTable_row_copy(dup, row_ctx)) {
        free(dup);
        dup = NULL;
    }

    return dup;
}

/************************************************************
 * the *_delete_row method is called to delete a row.
 */
netsnmp_index  *
saHpiSystemEventLogTable_delete_row(saHpiSystemEventLogTable_context * ctx)
{
    /*
     * netsnmp_mutex_destroy(ctx->lock); 
     */

    if (ctx->index.oids)
        free(ctx->index.oids);

    /*
     * TODO: release any memory you allocated here...
     */

    /*
     * release header
     */
    free(ctx);

    return NULL;
}


/************************************************************
 * RESERVE is used to check the syntax of all the variables
 * provided, that the values being set are sensible and consistent,
 * and to allocate any resources required for performing the SET.
 * After this stage, the expectation is that the set ought to
 * succeed, though this is not guaranteed. (In fact, with the UCD
 * agent, this is done in two passes - RESERVE1, and
 * RESERVE2, to allow for dependancies between variables).
 *
 * BEFORE calling this routine, the agent will call duplicate_row
 * to create a copy of the row (unless this is a new row; i.e.
 * row_created == 1).
 *
 * next state -> SET_RESERVE2 || SET_FREE
 */
void
saHpiSystemEventLogTable_set_reserve1(netsnmp_request_group * rg)
{
  
}

void
saHpiSystemEventLogTable_set_reserve2(netsnmp_request_group * rg)
{
  
}

/************************************************************
 * Assuming that the RESERVE phases were successful, the next
 * stage is indicated by the action value ACTION. This is used
 * to actually implement the set operation. However, this must
 * either be done into temporary (persistent) storage, or the
 * previous value stored similarly, in case any of the subsequent
 * ACTION calls fail.
 *
 * In your case, changes should be made to row_ctx. A copy of
 * the original row is in undo_ctx.
 */
void
saHpiSystemEventLogTable_set_action(netsnmp_request_group * rg)
{
  
}

/************************************************************
 * Only once the ACTION phase has completed successfully, can
 * the final COMMIT phase be run. This is used to complete any
 * writes that were done into temporary storage, and then release
 * any allocated resources. Note that all the code in this phase
 * should be "safe" code that cannot possibly fail (cue
 * hysterical laughter). The whole intent of the ACTION/COMMIT
 * division is that all of the fallible code should be done in
 * the ACTION phase, so that it can be backed out if necessary.
 *
 * BEFORE calling this routine, the agent will update the
 * container (inserting a row if row_created == 1, or removing
 * the row if row_deleted == 1).
 *
 * AFTER calling this routine, the agent will delete the
 * undo_info.
 */
void
saHpiSystemEventLogTable_set_commit(netsnmp_request_group * rg)
{
  
}

/************************************************************
 * If either of the RESERVE calls fail, the write routines
 * are called again with the FREE action, to release any resources
 * that have been allocated. The agent will then return a failure
 * response to the requesting application.
 *
 * AFTER calling this routine, the agent will delete undo_info.
 */
void
saHpiSystemEventLogTable_set_free(netsnmp_request_group * rg)
{
 
}

/************************************************************
 * If the ACTION phase does fail (for example due to an apparently
 * valid, but unacceptable value, or an unforeseen problem), then
 * the list of write routines are called again, with the UNDO
 * action. This requires the routine to reset the value that was
 * changed to its previous value (assuming it was actually changed),
 * and then to release any resources that had been allocated. As
 * with the FREE phase, the agent will then return an indication
 * of the error to the requesting application.
 *
 * BEFORE calling this routine, the agent will update the container
 * (remove any newly inserted row, re-insert any removed row).
 *
 * AFTER calling this routing, the agent will call row_copy
 * to restore the data in existing_row from the date in undo_info.
 * Then undo_info will be deleted (or existing row, if row_created
 * == 1).
 */
void
saHpiSystemEventLogTable_set_undo(netsnmp_request_group * rg)
{
  
}




/************************************************************
 *
 * Initialize the saHpiSystemEventLogTable table by defining its contents and how it's structured
 */
void
initialize_table_saHpiSystemEventLogTable(void)
{
    netsnmp_table_registration_info *table_info;

    if (my_handler) {
        snmp_log(LOG_ERR,
                 "initialize_table_saHpiSystemEventLogTable_handler called again\n");
        return;
    }

    memset(&cb, 0x00, sizeof(cb));

    /** create the table structure itself */
    table_info = SNMP_MALLOC_TYPEDEF(netsnmp_table_registration_info);

    /*
     * if your table is read only, it's easiest to change the
     * HANDLER_CAN_RWRITE definition below to HANDLER_CAN_RONLY 
     */
    my_handler =
        netsnmp_create_handler_registration("saHpiSystemEventLogTable",
                                            netsnmp_table_array_helper_handler,
                                            saHpiSystemEventLogTable_oid,
                                            saHpiSystemEventLogTable_oid_len,
                                            HANDLER_CAN_RONLY);

    if (!my_handler || !table_info) {
        snmp_log(LOG_ERR, "malloc failed in "
                 "initialize_table_saHpiSystemEventLogTable_handler\n");
        return; /** mallocs failed */
    }

    /***************************************************
     * Setting up the table's definition
     */
    /*
     * TODO: add any external indexes here.
     */

    /*
     * internal indexes
     */
        /** index: saHpiDomainID */
    netsnmp_table_helper_add_index(table_info, ASN_UNSIGNED);
        /** index: saHpiResourceID */
    netsnmp_table_helper_add_index(table_info, ASN_UNSIGNED);
        /** index: saHpiSystemEventLogEntryId */
    netsnmp_table_helper_add_index(table_info, ASN_UNSIGNED);

    table_info->min_column = saHpiSystemEventLogTable_COL_MIN;
    table_info->max_column = saHpiSystemEventLogTable_COL_MAX;

    /***************************************************
     * registering the table with the master agent
     */
    cb.get_value = saHpiSystemEventLogTable_get_value;
    cb.container =
        netsnmp_container_find("saHpiSystemEventLogTable_primary:"
                               "saHpiSystemEventLogTable:"
                               "table_container");

    netsnmp_container_add_index(cb.container,
                                netsnmp_container_find
                                ("saHpiSystemEventLogTable_secondary:"
                                 "saHpiSystemEventLogTable:"
                                 "table_container"));
    cb.container->next->compare = saHpiSystemEventLogTable_cmp;




    cb.create_row = (UserRowMethod *) saHpiSystemEventLogTable_create_row;
    /*
    cb.duplicate_row =
        (UserRowMethod *) saHpiSystemEventLogTable_duplicate_row;
    cb.delete_row = (UserRowMethod *) saHpiSystemEventLogTable_delete_row;
    cb.row_copy =
        (Netsnmp_User_Row_Operation *) saHpiSystemEventLogTable_row_copy;

    cb.can_delete =
        (Netsnmp_User_Row_Action *) saHpiSystemEventLogTable_can_delete;

    cb.set_reserve1 = saHpiSystemEventLogTable_set_reserve1;
    cb.set_reserve2 = saHpiSystemEventLogTable_set_reserve2;
    cb.set_action = saHpiSystemEventLogTable_set_action;
    cb.set_commit = saHpiSystemEventLogTable_set_commit;
    cb.set_free = saHpiSystemEventLogTable_set_free;
    cb.set_undo = saHpiSystemEventLogTable_set_undo;
    */

    DEBUGMSGTL(("initialize_table_saHpiSystemEventLogTable",
                "Registering table saHpiSystemEventLogTable "
                "as a table array\n"));
    netsnmp_table_container_register(my_handler, table_info, &cb,
                                     cb.container, 1);



    // Add the rest of objects 1-8
    netsnmp_register_read_only_ulong_instance("event_log_entries",
					      saHpiSystemEventLogEntries_oid,
					      OID_LENGTH(saHpiSystemEventLogEntries_oid),
					      &event_log_entries,
					      NULL);
    
    netsnmp_register_read_only_ulong_instance("event_log_size",
					      saHpiSystemEventLogSize_oid,
					      OID_LENGTH(saHpiSystemEventLogSize_oid),
					      &event_log_size,
					      NULL);
    
    netsnmp_register_read_only_instance(netsnmp_create_handler_registration
					("event_log_update_timestamp",
					 event_log_update_timestamp_handler,
					 saHpiSystemEventLogUpdateTimestamp_oid,
					 OID_LENGTH(saHpiSystemEventLogUpdateTimestamp_oid),
					 HANDLER_CAN_RONLY));
    
    netsnmp_register_read_only_instance(netsnmp_create_handler_registration
					("event_log_current_timestamp",
					 event_log_current_timestamp_handler,
					 saHpiSystemEventLogCurrentTime_oid,
					 OID_LENGTH(saHpiSystemEventLogCurrentTime_oid),
					 HANDLER_CAN_RONLY));
    
    netsnmp_register_read_only_int_instance("event_log_enabled",
					      saHpiSystemEventLogEnabled_oid,
					      OID_LENGTH(saHpiSystemEventLogEnabled_oid),
					      
					      &event_log_enabled,
					      NULL);
    
    netsnmp_register_read_only_int_instance("event_log_overflow_flag",
					      saHpiSystemEventLogOverflowFlag_oid,
					      OID_LENGTH(saHpiSystemEventLogEnabled_oid),
					      &event_log_overflow_flag,
					      NULL);
    
    netsnmp_register_read_only_int_instance("event_log_overflow_action",
						  saHpiSystemEventLogOverflowAction_oid,
						  OID_LENGTH(saHpiSystemEventLogOverflowAction_oid),
						  &event_log_overflow_action,
					       NULL);
    
   netsnmp_register_read_only_int_instance("event_log_delete_entry_supported",
					     saHpiSystemEventLogDeleteEntrySupported_oid,
					     OID_LENGTH(saHpiSystemEventLogDeleteEntrySupported_oid),
					     &event_log_delete_entry_supported,
					     NULL);
}

/************************************************************
 * saHpiSystemEventLogTable_get_value
 */
int
saHpiSystemEventLogTable_get_value(netsnmp_request_info *request,
                                   netsnmp_index * item,
                                   netsnmp_table_request_info *table_info)
{
    netsnmp_variable_list *var = request->requestvb;
    saHpiSystemEventLogTable_context *context =
        (saHpiSystemEventLogTable_context *) item;

    DEBUGMSGTL((AGENT,"get_value\n"));
    switch (table_info->colnum) {

    case COLUMN_SAHPISYSTEMEVENTLOGENTRYID:
            /** UNSIGNED32 = ASN_UNSIGNED */
        snmp_set_var_typed_value(var, ASN_UNSIGNED,
                                 (char *) &context->
                                 saHpiSystemEventLogEntryId,
                                 sizeof(context->
                                        saHpiSystemEventLogEntryId));
        break;

    case COLUMN_SAHPISYSTEMEVENTLOGTIMESTAMP:
            /** TimeStamp = ASN_TIMETICKS */
        snmp_set_var_typed_value(var, ASN_TIMETICKS,
                                 (char *) &context->
                                 saHpiSystemEventLogTimestamp,
                                 sizeof(context->
                                        saHpiSystemEventLogTimestamp));
        break;

    case COLUMN_SAHPISYSTEMEVENTLOGGED:
            /** RowPointer = ASN_OBJECT_ID */
        snmp_set_var_typed_value(var, ASN_OBJECT_ID,
                                 (char *) &context->saHpiSystemEventLogged,
                                 context->saHpiSystemEventLogged_len);
        break;

    default:/** We shouldn't get here */
        snmp_log(LOG_ERR, "unknown column in "
                 "saHpiSystemEventLogTable_get_value\n");
        return SNMP_ERR_GENERR;
    }
    return SNMP_ERR_NOERROR;
}

int
event_log_update_timestamp_handler(netsnmp_mib_handler *handler,
			 netsnmp_handler_registration *reginfo,
			 netsnmp_agent_request_info *reqinfo,
			 netsnmp_request_info *requests) {

  DEBUGMSGTL((AGENT,"--- event_log_update_timestamp_handler: Entry\n"));  
  DEBUGMSGTL((AGENT," TimeStamp is %d ", event_log_update_timestamp));
  if (reqinfo->mode == MODE_GETNEXT) {
	DEBUGMSGTL((AGENT," GETNEXT "));
  }

  switch (reqinfo->mode) {
  case MODE_GET:
    snmp_set_var_typed_value(requests->requestvb, ASN_TIMETICKS,
			     (u_char *) &event_log_update_timestamp,
			     sizeof(event_log_update_timestamp));
    
  }
  
  DEBUGMSGTL((AGENT,"--- event_log_upate_timestamp_handler: Exit\n"));
  return SNMP_ERR_NOERROR;
}

int
event_log_current_timestamp_handler(netsnmp_mib_handler *handler,
			 netsnmp_handler_registration *reginfo,
			 netsnmp_agent_request_info *reqinfo,
			 netsnmp_request_info *requests) {

  DEBUGMSGTL((AGENT,"--- event_log_current_timestamp_handler: Entry\n"));  
  DEBUGMSGTL((AGENT," TimeStamp is %d ", event_log_current_timestamp));
  if (reqinfo->mode == MODE_GETNEXT) {
	DEBUGMSGTL((AGENT," GETNEXT "));
  }

  switch (reqinfo->mode) {
  case MODE_GET:
    snmp_set_var_typed_value(requests->requestvb, ASN_TIMETICKS,
			     (u_char *) &event_log_current_timestamp,
			     sizeof(event_log_current_timestamp));
  }
  
  DEBUGMSGTL((AGENT,"--- event_log_current_timestamp_handler: Exit\n"));
  return SNMP_ERR_NOERROR;
}
