/*
 * Note: this file originally auto-generated by mib2c using
 *       : mib2c.array-user.conf,v 5.18.2.2 2004/02/09 18:21:47 rstory Exp $
 *
 * $Id$
 *
 *
 * For help understanding NET-SNMP in general, please check the 
 *     documentation and FAQ at:
 *
 *     http://www.net-snmp.org/
 *
 *
 * For help understanding this code, the agent and how it processes
 *     requests, please check the following references.
 *
 *     http://www.net-snmp.org/tutorial-5/
 *
 *
 * You can also join the #net-snmp channel on irc.freenode.net
 *     and ask for help there.
 *
 *
 * And if all else fails, send a detailed message to the developers
 *     describing the problem you are having to:
 *
 *    net-snmp-coders@lists.sourceforge.net
 *
 *
 * Yes, there is lots of code here that you might not use. But it is much
 * easier to remove code than to add it!
 */
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

#include <net-snmp/library/snmp_assert.h>

#include <SaHpi.h>
#include "saHpiCtrlTextTable.h"
#include <hpiSubagent.h>
#include <hpiCheckIndice.h>
#include <saHpiResourceTable.h>
#include <session_info.h>

#include <oh_utils.h>


static     netsnmp_handler_registration *my_handler = NULL;
static     netsnmp_table_array_callbacks cb;

oid saHpiCtrlTextTable_oid[] = { saHpiCtrlTextTable_TABLE_OID };
size_t saHpiCtrlTextTable_oid_len = OID_LENGTH(saHpiCtrlTextTable_oid);

/************************************************************/
/************************************************************/
/************************************************************/
/************************************************************/

/*************************************************************
 * objects for hash table
 */
static int initialized = FALSE;               
static GHashTable *dr_table;

/*************************************************************
 * oid and fucntion declarations scalars
 */
static u_long ctrl_text_entry_count = 0;
static oid saHpiCtrlTextEntryCount_oid[] = { 1,3,6,1,4,1,18568,2,1,1,4,7,10 };
int handle_saHpiCtrlTextEntryCount(netsnmp_mib_handler *handler,
				   netsnmp_handler_registration *reginfo,
				   netsnmp_agent_request_info   *reqinfo,
				   netsnmp_request_info         *requests);
int initialize_table_saHpiCtrlTextEntryCount(void);

/*
 * SaErrorT populate_ctrl_text()
 */
SaErrorT populate_ctrl_text(SaHpiSessionIdT sessionid, 
			    SaHpiRdrT *rdr_entry,
			    SaHpiRptEntryT *rpt_entry,
			    oid *full_oid, size_t full_oid_len,
			    oid *child_oid, size_t *child_oid_len)
{
	return 0;
}

/*
 * SaErrorT set_table_ctrl_text()
 */
int set_table_ctrl_text (saHpiCtrlTextTable_context *row_ctx) 
{
	return 0;
}

/*
 * SaErrorT handle_saHpiCtrlTextEntryCount()
 */
int handle_saHpiCtrlTextEntryCount(netsnmp_mib_handler *handler,
				   netsnmp_handler_registration *reginfo,
				   netsnmp_agent_request_info   *reqinfo,
				   netsnmp_request_info         *requests) 
{
	return 0;
}

/*
 * SaErrorT initialize_table_saHpiCtrlTextEntryCount()
 */
int initialize_table_saHpiCtrlTextEntryCount(void)
{
	return 0;
}

/************************************************************/
/************************************************************/
/************************************************************/
/************************************************************/



 /************************************************************
 * keep binary tree to find context by name
 */
static int saHpiCtrlTextTable_cmp( const void *lhs, const void *rhs );

/************************************************************
 * compare two context pointers here. Return -1 if lhs < rhs,
 * 0 if lhs == rhs, and 1 if lhs > rhs.
 */
static int
saHpiCtrlTextTable_cmp( const void *lhs, const void *rhs )
{
    saHpiCtrlTextTable_context *context_l =
        (saHpiCtrlTextTable_context *)lhs;
    saHpiCtrlTextTable_context *context_r =
        (saHpiCtrlTextTable_context *)rhs;

    /*
     * check primary key, then secondary. Add your own code if
     * there are more than 2 indexes
     */
    int rc;

	DEBUGMSGTL ((AGENT, "saHpiCtrlTextTable_cmp, called\n"));

	/* check for NULL pointers */
	if (lhs == NULL || rhs == NULL ) {
		DEBUGMSGTL((AGENT,"saHpiCtrlDigitalTable_cmp() NULL pointer ERROR\n" ));
		return 0;
	}
	/* CHECK FIRST INDEX,  saHpiDomainId */
	if ( context_l->index.oids[0] < context_r->index.oids[0])
		return -1;

	if ( context_l->index.oids[0] > context_r->index.oids[0])
		return 1;

	if ( context_l->index.oids[0] == context_r->index.oids[0]) {
		/* If saHpiDomainId index is equal sort by second index */
		/* CHECK SECOND INDEX,  saHpiResourceEntryId */
		if ( context_l->index.oids[1] < context_r->index.oids[1])
			return -1;

		if ( context_l->index.oids[1] > context_r->index.oids[1])
			return 1;

		if ( context_l->index.oids[1] == context_r->index.oids[1]) {
			/* If saHpiResourceEntryId index is equal sort by third index */
			/* CHECK THIRD INDEX,  saHpiResourceIsHistorical */
			if ( context_l->index.oids[2] < context_r->index.oids[2])
				return -1;

			if ( context_l->index.oids[2] > context_r->index.oids[2])
				return 1;

			if ( context_l->index.oids[2] == context_r->index.oids[2]) {
				/* If saHpiResourceIsHistorical index is equal sort by forth index */
				/* CHECK FORTH INDEX,  saHpiCtrlTextEntryId */
				if ( context_l->index.oids[3] < context_r->index.oids[3])
					return -1;

				if ( context_l->index.oids[3] > context_r->index.oids[3])
					return 1;

				if ( context_l->index.oids[3] == context_r->index.oids[3])
					return 0;
			}
		}
	}

	return 0;
}

/************************************************************
 * Initializes the saHpiCtrlTextTable module
 */
void
init_saHpiCtrlTextTable(void)
{
    initialize_table_saHpiCtrlTextTable();

    initialize_table_saHpiCtrlTextEntryCount();

    domain_resource_pair_initialize(&initialized, &dr_table);

}

/************************************************************
 * the *_row_copy routine
 */
static int saHpiCtrlTextTable_row_copy(saHpiCtrlTextTable_context * dst,
                         saHpiCtrlTextTable_context * src)
{
    if(!dst||!src)
        return 1;
        
    /*
     * copy index, if provided
     */
    if(dst->index.oids)
        free(dst->index.oids);
    if(snmp_clone_mem( (void*)&dst->index.oids, src->index.oids,
                           src->index.len * sizeof(oid) )) {
        dst->index.oids = NULL;
        return 1;
    }
    dst->index.len = src->index.len;
    

    /*
     * copy components into the context structure
     */
    /** TODO: add code for external index(s)! */
    dst->saHpiCtrlTextEntryId = src->saHpiCtrlTextEntryId;

    dst->saHpiCtrlTextNum = src->saHpiCtrlTextNum;

    dst->saHpiCtrlTextOutputType = src->saHpiCtrlTextOutputType;

    dst->saHpiCtrlTextDefaultMode = src->saHpiCtrlTextDefaultMode;

    dst->saHpiCtrlTextMode = src->saHpiCtrlTextMode;

    dst->saHpiCtrlTextIsReadOnly = src->saHpiCtrlTextIsReadOnly;

    dst->saHpiCtrlTextIsWriteOnly = src->saHpiCtrlTextIsWriteOnly;

    dst->saHpiCtrlTextMaxChars = src->saHpiCtrlTextMaxChars;

    dst->saHpiCtrlTextMaxLines = src->saHpiCtrlTextMaxLines;

    dst->saHpiCtrlTextLanguage = src->saHpiCtrlTextLanguage;

    dst->saHpiCtrlTextType = src->saHpiCtrlTextType;

    dst->saHpiCtrlTextDefaultLine = src->saHpiCtrlTextDefaultLine;

    memcpy( dst->saHpiCtrlTextDefault, src->saHpiCtrlTextDefault, src->saHpiCtrlTextDefault_len );
    dst->saHpiCtrlTextDefault_len = src->saHpiCtrlTextDefault_len;

    dst->saHpiCtrlTextLine = src->saHpiCtrlTextLine;

    memcpy( dst->saHpiCtrlTextState, src->saHpiCtrlTextState, src->saHpiCtrlTextState_len );
    dst->saHpiCtrlTextState_len = src->saHpiCtrlTextState_len;

    dst->saHpiCtrlTextOem = src->saHpiCtrlTextOem;

    memcpy( dst->saHpiCtrlTextRDR, src->saHpiCtrlTextRDR, src->saHpiCtrlTextRDR_len );
    dst->saHpiCtrlTextRDR_len = src->saHpiCtrlTextRDR_len;

    return 0;
}

/**
 * the *_extract_index routine
 *
 * This routine is called when a set request is received for an index
 * that was not found in the table container. Here, we parse the oid
 * in the the individual index components and copy those indexes to the
 * context. Then we make sure the indexes for the new row are valid.
 */
int
saHpiCtrlTextTable_extract_index( saHpiCtrlTextTable_context * ctx, netsnmp_index * hdr )
{
    /*
     * temporary local storage for extracting oid index
     *
     * extract index uses varbinds (netsnmp_variable_list) to parse
     * the index OID into the individual components for each index part.
     */
    /** TODO: add storage for external index(s)! */
    netsnmp_variable_list var_saHpiDomainId;
    netsnmp_variable_list var_saHpiResourceId;
    netsnmp_variable_list var_saHpiResourceIsHistorical;
    netsnmp_variable_list var_saHpiCtrlTextEntryId;
    int err;

    /*
     * copy index, if provided
     */
    if(hdr) {
        netsnmp_assert(ctx->index.oids == NULL);
        if(snmp_clone_mem( (void*)&ctx->index.oids, hdr->oids,
                           hdr->len * sizeof(oid) )) {
            return -1;
        }
        ctx->index.len = hdr->len;
    }

    /*
     * initialize variable that will hold each component of the index.
     * If there are multiple indexes for the table, the variable_lists
     * need to be linked together, in order.
     */
       /** TODO: add code for external index(s)! */
       memset( &var_saHpiDomainId, 0x00, sizeof(var_saHpiDomainId) );
       var_saHpiDomainId.type = ASN_UNSIGNED; /* type hint for parse_oid_indexes */
       /** TODO: link this index to the next, or NULL for the last one */
       var_saHpiDomainId.next_variable = &var_saHpiResourceId;

       memset( &var_saHpiResourceId, 0x00, sizeof(var_saHpiResourceId) );
       var_saHpiResourceId.type = ASN_UNSIGNED; /* type hint for parse_oid_indexes */
       /** TODO: link this index to the next, or NULL for the last one */
       var_saHpiResourceId.next_variable = &var_saHpiResourceIsHistorical;

       memset( &var_saHpiResourceIsHistorical, 0x00, sizeof(var_saHpiResourceIsHistorical) );
       var_saHpiResourceIsHistorical.type = ASN_INTEGER; /* type hint for parse_oid_indexes */
       /** TODO: link this index to the next, or NULL for the last one */
       var_saHpiResourceIsHistorical.next_variable = &var_saHpiCtrlTextEntryId;

       memset( &var_saHpiCtrlTextEntryId, 0x00, sizeof(var_saHpiCtrlTextEntryId) );
       var_saHpiCtrlTextEntryId.type = ASN_UNSIGNED; /* type hint for parse_oid_indexes */
       /** TODO: link this index to the next, or NULL for the last one */
       var_saHpiCtrlTextEntryId.next_variable = NULL;


    /*
     * parse the oid into the individual index components
     */
    err = parse_oid_indexes( hdr->oids, hdr->len, &var_saHpiDomainId );
    if (err == SNMP_ERR_NOERROR) {
       /*
        * copy index components into the context structure
        */
              /** skipping external index saHpiDomainId */
   
              /** skipping external index saHpiResourceId */
   
              /** skipping external index saHpiResourceIsHistorical */
   
                ctx->saHpiCtrlTextEntryId = *var_saHpiCtrlTextEntryId.val.integer;

		err = saHpiDomainId_check_index(
			*var_saHpiDomainId.val.integer);
		err = saHpiResourceEntryId_check_index(
			*var_saHpiResourceId.val.integer);  
		err = saHpiResourceIsHistorical_check_index(
			*var_saHpiResourceIsHistorical.val.integer);
		err = saHpiCtrlTextEntryId_check_index(
			*var_saHpiCtrlTextEntryId.val.integer);   
   
    }

    /*
     * parsing may have allocated memory. free it.
     */
    snmp_reset_var_buffers( &var_saHpiDomainId );

    return err;
}

/************************************************************
 * the *_can_activate routine is called
 * when a row is changed to determine if all the values
 * set are consistent with the row's rules for a row status
 * of ACTIVE.
 *
 * return 1 if the row could be ACTIVE
 * return 0 if the row is not ready for the ACTIVE state
 */
int saHpiCtrlTextTable_can_activate(saHpiCtrlTextTable_context *undo_ctx,
                      saHpiCtrlTextTable_context *row_ctx,
                      netsnmp_request_group * rg)
{
    /*
     * TODO: check for activation requirements here
     */


    /*
     * be optimistic.
     */
    return 1;
}

/************************************************************
 * the *_can_deactivate routine is called when a row that is
 * currently ACTIVE is set to a state other than ACTIVE. If
 * there are conditions in which a row should not be allowed
 * to transition out of the ACTIVE state (such as the row being
 * referred to by another row or table), check for them here.
 *
 * return 1 if the row can be set to a non-ACTIVE state
 * return 0 if the row must remain in the ACTIVE state
 */
int saHpiCtrlTextTable_can_deactivate(saHpiCtrlTextTable_context *undo_ctx,
                        saHpiCtrlTextTable_context *row_ctx,
                        netsnmp_request_group * rg)
{
    /*
     * TODO: check for deactivation requirements here
     */
    return 1;
}

/************************************************************
 * the *_can_delete routine is called to determine if a row
 * can be deleted.
 *
 * return 1 if the row can be deleted
 * return 0 if the row cannot be deleted
 */
int saHpiCtrlTextTable_can_delete(saHpiCtrlTextTable_context *undo_ctx,
                    saHpiCtrlTextTable_context *row_ctx,
                    netsnmp_request_group * rg)
{
    /*
     * probably shouldn't delete a row that we can't
     * deactivate.
     */
    if(saHpiCtrlTextTable_can_deactivate(undo_ctx,row_ctx,rg) != 1)
        return 0;
    
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
 * returns a newly allocated saHpiCtrlTextTable_context
 *   structure if the specified indexes are not illegal
 * returns NULL for errors or illegal index values.
 */
saHpiCtrlTextTable_context *
saHpiCtrlTextTable_create_row( netsnmp_index* hdr)
{
    saHpiCtrlTextTable_context * ctx =
        SNMP_MALLOC_TYPEDEF(saHpiCtrlTextTable_context);
    if(!ctx)
        return NULL;
        
    /*
     * TODO: check indexes, if necessary.
     */
    if(saHpiCtrlTextTable_extract_index( ctx, hdr )) {
        free(ctx->index.oids);
        free(ctx);
        return NULL;
    }

    /* netsnmp_mutex_init(ctx->lock);
       netsnmp_mutex_lock(ctx->lock); */

    /*
     * TODO: initialize any default values here. This is also
     * first place you really should allocate any memory for
     * yourself to use.  If you allocated memory earlier,
     * make sure you free it for earlier error cases!
     */
    /**
     ctx->saHpiCtrlTextMode = 0;
     ctx->saHpiCtrlTextLine = 0;
     ctx->saHpiCtrlTextState = 0;
    */

    return ctx;
}

/************************************************************
 * the *_duplicate row routine
 */
saHpiCtrlTextTable_context *
saHpiCtrlTextTable_duplicate_row( saHpiCtrlTextTable_context * row_ctx)
{
    saHpiCtrlTextTable_context * dup;

    if(!row_ctx)
        return NULL;

    dup = SNMP_MALLOC_TYPEDEF(saHpiCtrlTextTable_context);
    if(!dup)
        return NULL;
        
    if(saHpiCtrlTextTable_row_copy(dup,row_ctx)) {
        free(dup);
        dup = NULL;
    }

    return dup;
}

/************************************************************
 * the *_delete_row method is called to delete a row.
 */
netsnmp_index * saHpiCtrlTextTable_delete_row( saHpiCtrlTextTable_context * ctx )
{
  /* netsnmp_mutex_destroy(ctx->lock); */

    if(ctx->index.oids)
        free(ctx->index.oids);

    /*
     * TODO: release any memory you allocated here...
     */

    /*
     * release header
     */
    free( ctx );

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
void saHpiCtrlTextTable_set_reserve1( netsnmp_request_group *rg )
{
    saHpiCtrlTextTable_context *row_ctx =
            (saHpiCtrlTextTable_context *)rg->existing_row;
    saHpiCtrlTextTable_context *undo_ctx =
            (saHpiCtrlTextTable_context *)rg->undo_info;
    netsnmp_variable_list *var;
    netsnmp_request_group_item *current;
    int rc;


    /*
     * TODO: loop through columns, check syntax and lengths. For
     * columns which have no dependencies, you could also move
     * the value/range checking here to attempt to catch error
     * cases as early as possible.
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;
        rc = SNMP_ERR_NOERROR;

        switch(current->tri->colnum) {

        case COLUMN_SAHPICTRLTEXTMODE:
            /** SaHpiCtrlMode = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER,
                                                sizeof(row_ctx->saHpiCtrlTextMode));
        break;

        case COLUMN_SAHPICTRLTEXTLINE:
            /** Unsigned8 = ASN_INTEGER */
            rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER,
                                                sizeof(row_ctx->saHpiCtrlTextLine));
        break;

        case COLUMN_SAHPICTRLTEXTSTATE:
            /** SaHpiText = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
                                                sizeof(row_ctx->saHpiCtrlTextState));
        break;

        default: /** We shouldn't get here */
            rc = SNMP_ERR_GENERR;
            snmp_log(LOG_ERR, "unknown column in "
                     "saHpiCtrlTextTable_set_reserve1\n");
        }

        if (rc)
           netsnmp_set_mode_request_error(MODE_SET_BEGIN, current->ri, rc );
        rg->status = SNMP_MAX( rg->status, current->ri->status );
    }

    /*
     * done with all the columns. Could check row related
     * requirements here.
     */
}

void saHpiCtrlTextTable_set_reserve2( netsnmp_request_group *rg )
{
    saHpiCtrlTextTable_context *row_ctx = (saHpiCtrlTextTable_context *)rg->existing_row;
    saHpiCtrlTextTable_context *undo_ctx = (saHpiCtrlTextTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;
    netsnmp_variable_list *var;
    int rc;

    rg->rg_void = rg->list->ri;

    /*
     * TODO: loop through columns, check for valid
     * values and any range constraints.
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;
        rc = SNMP_ERR_NOERROR;

        switch(current->tri->colnum) {

        case COLUMN_SAHPICTRLTEXTMODE:
            /** SaHpiCtrlMode = ASN_INTEGER */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( *var->val.integer != XXX ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_SAHPICTRLTEXTLINE:
            /** Unsigned8 = ASN_INTEGER */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( *var->val.integer != XXX ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        case COLUMN_SAHPICTRLTEXTSTATE:
            /** SaHpiText = ASN_OCTET_STR */
                    /*
                     * TODO: routine to check valid values
                     *
                     * EXAMPLE:
                     *
                    * if ( XXX_check_value( var->val.string, XXX ) ) {
                *    rc = SNMP_ERR_INCONSISTENTVALUE;
                *    rc = SNMP_ERR_BADVALUE;
                * }
                */
        break;

        default: /** We shouldn't get here */
            netsnmp_assert(0); /** why wasn't this caught in reserve1? */
        }

        if (rc)
           netsnmp_set_mode_request_error(MODE_SET_BEGIN, current->ri, rc);
    }

    /*
     * done with all the columns. Could check row related
     * requirements here.
     */
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
void saHpiCtrlTextTable_set_action( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
    saHpiCtrlTextTable_context *row_ctx = (saHpiCtrlTextTable_context *)rg->existing_row;
    saHpiCtrlTextTable_context *undo_ctx = (saHpiCtrlTextTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

    int            row_err = 0;

    /*
     * TODO: loop through columns, copy varbind values
     * to context structure for the row.
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;

        switch(current->tri->colnum) {

        case COLUMN_SAHPICTRLTEXTMODE:
            /** SaHpiCtrlMode = ASN_INTEGER */
            row_ctx->saHpiCtrlTextMode = *var->val.integer;
        break;

        case COLUMN_SAHPICTRLTEXTLINE:
            /** Unsigned8 = ASN_INTEGER */
            row_ctx->saHpiCtrlTextLine = *var->val.integer;
        break;

        case COLUMN_SAHPICTRLTEXTSTATE:
            /** SaHpiText = ASN_OCTET_STR */
            memcpy(row_ctx->saHpiCtrlTextState,var->val.string,var->val_len);
            row_ctx->saHpiCtrlTextState_len = var->val_len;
        break;

        default: /** We shouldn't get here */
            netsnmp_assert(0); /** why wasn't this caught in reserve1? */
        }
    }

    if(row_err) {
        netsnmp_set_mode_request_error(MODE_SET_BEGIN,
                                       (netsnmp_request_info*)rg->rg_void,
                                       row_err);
        return;
    }

    /*
     * TODO: if you have dependencies on other tables, this would be
     * a good place to check those, too.
     */
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
void saHpiCtrlTextTable_set_commit( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
    saHpiCtrlTextTable_context *row_ctx = (saHpiCtrlTextTable_context *)rg->existing_row;
    saHpiCtrlTextTable_context *undo_ctx = (saHpiCtrlTextTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

    /*
     * loop through columns
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;

        switch(current->tri->colnum) {

        case COLUMN_SAHPICTRLTEXTMODE:
            /** SaHpiCtrlMode = ASN_INTEGER */
        break;

        case COLUMN_SAHPICTRLTEXTLINE:
            /** Unsigned8 = ASN_INTEGER */
        break;

        case COLUMN_SAHPICTRLTEXTSTATE:
            /** SaHpiText = ASN_OCTET_STR */
        break;

        default: /** We shouldn't get here */
            netsnmp_assert(0); /** why wasn't this caught in reserve1? */
        }
    }

    /*
     * done with all the columns. Could check row related
     * requirements here.
     */
}

/************************************************************
 * If either of the RESERVE calls fail, the write routines
 * are called again with the FREE action, to release any resources
 * that have been allocated. The agent will then return a failure
 * response to the requesting application.
 *
 * AFTER calling this routine, the agent will delete undo_info.
 */
void saHpiCtrlTextTable_set_free( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
    saHpiCtrlTextTable_context *row_ctx = (saHpiCtrlTextTable_context *)rg->existing_row;
    saHpiCtrlTextTable_context *undo_ctx = (saHpiCtrlTextTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

    /*
     * loop through columns
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;

        switch(current->tri->colnum) {

        case COLUMN_SAHPICTRLTEXTMODE:
            /** SaHpiCtrlMode = ASN_INTEGER */
        break;

        case COLUMN_SAHPICTRLTEXTLINE:
            /** Unsigned8 = ASN_INTEGER */
        break;

        case COLUMN_SAHPICTRLTEXTSTATE:
            /** SaHpiText = ASN_OCTET_STR */
        break;

        default: 
		break;
        }
    }

    /*
     * done with all the columns. Could check row related
     * requirements here.
     */
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
void saHpiCtrlTextTable_set_undo( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
    saHpiCtrlTextTable_context *row_ctx = (saHpiCtrlTextTable_context *)rg->existing_row;
    saHpiCtrlTextTable_context *undo_ctx = (saHpiCtrlTextTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

    /*
     * loop through columns
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;

        switch(current->tri->colnum) {

        case COLUMN_SAHPICTRLTEXTMODE:
            /** SaHpiCtrlMode = ASN_INTEGER */
        break;

        case COLUMN_SAHPICTRLTEXTLINE:
            /** Unsigned8 = ASN_INTEGER */
        break;

        case COLUMN_SAHPICTRLTEXTSTATE:
            /** SaHpiText = ASN_OCTET_STR */
        break;

        default: /** We shouldn't get here */
            netsnmp_assert(0); /** why wasn't this caught in reserve1? */
        }
    }

    /*
     * done with all the columns. Could check row related
     * requirements here.
     */
}

/************************************************************
 *
 * Initialize the saHpiCtrlTextTable table by defining its contents and how it's structured
 */
void
initialize_table_saHpiCtrlTextTable(void)
{
    netsnmp_table_registration_info *table_info;

    if(my_handler) {
        snmp_log(LOG_ERR, "initialize_table_saHpiCtrlTextTable_handler called again\n");
        return;
    }

    memset(&cb, 0x00, sizeof(cb));

    /** create the table structure itself */
    table_info = SNMP_MALLOC_TYPEDEF(netsnmp_table_registration_info);

    /* if your table is read only, it's easiest to change the
       HANDLER_CAN_RWRITE definition below to HANDLER_CAN_RONLY */
    my_handler = netsnmp_create_handler_registration("saHpiCtrlTextTable",
                                             netsnmp_table_array_helper_handler,
                                             saHpiCtrlTextTable_oid,
                                             saHpiCtrlTextTable_oid_len,
                                             HANDLER_CAN_RWRITE);
            
    if (!my_handler || !table_info) {
        snmp_log(LOG_ERR, "malloc failed in "
                 "initialize_table_saHpiCtrlTextTable_handler\n");
        return; /** mallocs failed */
    }

    /***************************************************
     * Setting up the table's definition
     */
    /*
     * TODO: add any external indexes here.
     */
        /** TODO: add code for external index(s)! */

    /*
     * internal indexes
     */
        /** index: saHpiDomainId */
        netsnmp_table_helper_add_index(table_info, ASN_UNSIGNED);
        /** index: saHpiResourceId */
        netsnmp_table_helper_add_index(table_info, ASN_UNSIGNED);
        /** index: saHpiResourceIsHistorical */
        netsnmp_table_helper_add_index(table_info, ASN_INTEGER);
        /** index: saHpiCtrlTextEntryId */
        netsnmp_table_helper_add_index(table_info, ASN_UNSIGNED);

    table_info->min_column = saHpiCtrlTextTable_COL_MIN;
    table_info->max_column = saHpiCtrlTextTable_COL_MAX;

    /***************************************************
     * registering the table with the master agent
     */
    cb.get_value = saHpiCtrlTextTable_get_value;
    cb.container = netsnmp_container_find("saHpiCtrlTextTable_primary:"
                                          "saHpiCtrlTextTable:"
                                          "table_container");

    netsnmp_container_add_index(cb.container,
                                netsnmp_container_find("saHpiCtrlTextTable_secondary:"
                                                       "saHpiCtrlTextTable:"
                                                       "table_container"));
    cb.container->next->compare = saHpiCtrlTextTable_cmp;

    cb.can_set = 1;

    cb.create_row = (UserRowMethod*)saHpiCtrlTextTable_create_row;

    cb.duplicate_row = (UserRowMethod*)saHpiCtrlTextTable_duplicate_row;
    cb.delete_row = (UserRowMethod*)saHpiCtrlTextTable_delete_row;
    cb.row_copy = (Netsnmp_User_Row_Operation *)saHpiCtrlTextTable_row_copy;

    cb.can_activate = (Netsnmp_User_Row_Action *)saHpiCtrlTextTable_can_activate;
    cb.can_deactivate = (Netsnmp_User_Row_Action *)saHpiCtrlTextTable_can_deactivate;
    cb.can_delete = (Netsnmp_User_Row_Action *)saHpiCtrlTextTable_can_delete;

    cb.set_reserve1 = saHpiCtrlTextTable_set_reserve1;
    cb.set_reserve2 = saHpiCtrlTextTable_set_reserve2;
    cb.set_action = saHpiCtrlTextTable_set_action;
    cb.set_commit = saHpiCtrlTextTable_set_commit;
    cb.set_free = saHpiCtrlTextTable_set_free;
    cb.set_undo = saHpiCtrlTextTable_set_undo;

    DEBUGMSGTL(("initialize_table_saHpiCtrlTextTable",
                "Registering table saHpiCtrlTextTable "
                "as a table array\n"));
    netsnmp_table_container_register(my_handler, table_info, &cb,
                                     cb.container, 1);
}

/************************************************************
 * saHpiCtrlTextTable_get_value
 *
 * This routine is called for get requests to copy the data
 * from the context to the varbind for the request. If the
 * context has been properly maintained, you don't need to
 * change in code in this fuction.
 */
int saHpiCtrlTextTable_get_value(
            netsnmp_request_info *request,
            netsnmp_index *item,
            netsnmp_table_request_info *table_info )
{
    netsnmp_variable_list *var = request->requestvb;
    saHpiCtrlTextTable_context *context = (saHpiCtrlTextTable_context *)item;

    switch(table_info->colnum) {

        case COLUMN_SAHPICTRLTEXTENTRYID:
            /** SaHpiEntryId = ASN_UNSIGNED */
            snmp_set_var_typed_value(var, ASN_UNSIGNED,
                         (char*)&context->saHpiCtrlTextEntryId,
                         sizeof(context->saHpiCtrlTextEntryId) );
        break;
    
        case COLUMN_SAHPICTRLTEXTNUM:
            /** SaHpiInstrumentId = ASN_UNSIGNED */
            snmp_set_var_typed_value(var, ASN_UNSIGNED,
                         (char*)&context->saHpiCtrlTextNum,
                         sizeof(context->saHpiCtrlTextNum) );
        break;
    
        case COLUMN_SAHPICTRLTEXTOUTPUTTYPE:
            /** SaHpiCtrlOutputType = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->saHpiCtrlTextOutputType,
                         sizeof(context->saHpiCtrlTextOutputType) );
        break;
    
        case COLUMN_SAHPICTRLTEXTDEFAULTMODE:
            /** SaHpiCtrlMode = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->saHpiCtrlTextDefaultMode,
                         sizeof(context->saHpiCtrlTextDefaultMode) );
        break;
    
        case COLUMN_SAHPICTRLTEXTMODE:
            /** SaHpiCtrlMode = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->saHpiCtrlTextMode,
                         sizeof(context->saHpiCtrlTextMode) );
        break;
    
        case COLUMN_SAHPICTRLTEXTISREADONLY:
            /** TruthValue = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->saHpiCtrlTextIsReadOnly,
                         sizeof(context->saHpiCtrlTextIsReadOnly) );
        break;
    
        case COLUMN_SAHPICTRLTEXTISWRITEONLY:
            /** TruthValue = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->saHpiCtrlTextIsWriteOnly,
                         sizeof(context->saHpiCtrlTextIsWriteOnly) );
        break;
    
        case COLUMN_SAHPICTRLTEXTMAXCHARS:
            /** Unsigned8 = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->saHpiCtrlTextMaxChars,
                         sizeof(context->saHpiCtrlTextMaxChars) );
        break;
    
        case COLUMN_SAHPICTRLTEXTMAXLINES:
            /** Unsigned8 = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->saHpiCtrlTextMaxLines,
                         sizeof(context->saHpiCtrlTextMaxLines) );
        break;
    
        case COLUMN_SAHPICTRLTEXTLANGUAGE:
            /** SaHpiTextLanguage = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->saHpiCtrlTextLanguage,
                         sizeof(context->saHpiCtrlTextLanguage) );
        break;
    
        case COLUMN_SAHPICTRLTEXTTYPE:
            /** SaHpiTextType = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->saHpiCtrlTextType,
                         sizeof(context->saHpiCtrlTextType) );
        break;
    
        case COLUMN_SAHPICTRLTEXTDEFAULTLINE:
            /** Unsigned8 = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->saHpiCtrlTextDefaultLine,
                         sizeof(context->saHpiCtrlTextDefaultLine) );
        break;
    
        case COLUMN_SAHPICTRLTEXTDEFAULT:
            /** SaHpiText = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->saHpiCtrlTextDefault,
                         context->saHpiCtrlTextDefault_len );
        break;
    
        case COLUMN_SAHPICTRLTEXTLINE:
            /** Unsigned8 = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->saHpiCtrlTextLine,
                         sizeof(context->saHpiCtrlTextLine) );
        break;
    
        case COLUMN_SAHPICTRLTEXTSTATE:
            /** SaHpiText = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->saHpiCtrlTextState,
                         context->saHpiCtrlTextState_len );
        break;
    
        case COLUMN_SAHPICTRLTEXTOEM:
            /** UNSIGNED32 = ASN_UNSIGNED */
            snmp_set_var_typed_value(var, ASN_UNSIGNED,
                         (char*)&context->saHpiCtrlTextOem,
                         sizeof(context->saHpiCtrlTextOem) );
        break;
    
        case COLUMN_SAHPICTRLTEXTRDR:
            /** RowPointer = ASN_OBJECT_ID */
            snmp_set_var_typed_value(var, ASN_OBJECT_ID,
                         (char*)&context->saHpiCtrlTextRDR,
                         context->saHpiCtrlTextRDR_len );
        break;
    
    default: /** We shouldn't get here */
        snmp_log(LOG_ERR, "unknown column in "
                 "saHpiCtrlTextTable_get_value\n");
        return SNMP_ERR_GENERR;
    }
    return SNMP_ERR_NOERROR;
}

/************************************************************
 * saHpiCtrlTextTable_get_by_idx
 */
const saHpiCtrlTextTable_context *
saHpiCtrlTextTable_get_by_idx(netsnmp_index * hdr)
{
    return (const saHpiCtrlTextTable_context *)
        CONTAINER_FIND(cb.container, hdr );
}


