/*
 * (C) Copyright IBM Corp. 2005
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *   David Judkovics  <djudkovi@us.ibm.com>
 *   Daniel de Araujo <ddearauj@us.ibm.com>
 *
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
#include "saHpiDomainReferenceTable.h"
#include <hpiSubagent.h>
#include <hpiCheckIndice.h>

static     netsnmp_handler_registration *my_handler = NULL;
static     netsnmp_table_array_callbacks cb;


oid saHpiDomainReferenceTable_oid[] = { saHpiDomainReferenceTable_TABLE_OID };
size_t saHpiDomainReferenceTable_oid_len = OID_LENGTH(saHpiDomainReferenceTable_oid);

/************************************************************/
/************************************************************/
/************************************************************/
/************************************************************/

/*************************************************************
 * oid and fucntion declarations scalars
 */

static oid saHpiDomainReferenceEntryCount_oid[] = { 1,3,6,1,4,1,18568,2,1,1,2,3 };
static u_long domain_reference_entry_count = 0;

/**
 * int handle_saHpiDomainReferenceEntryCount()  
 */
int
handle_saHpiDomainReferenceEntryCount(netsnmp_mib_handler *handler,
                          netsnmp_handler_registration *reginfo,
                          netsnmp_agent_request_info   *reqinfo,
                          netsnmp_request_info         *requests);

/**
 * int initialize_table_saHpiDomainReferenceEntryCount(void)
 */
int initialize_table_saHpiDomainReferenceEntryCount(void);

/*
 * void poplulate_saHpiDomainReferenceTable()  
 */
 int
 poplulate_saHpiDomainReferenceTable(SaHpiSessionIdT sessionid) 
{
	
	SaErrorT rv;
	SaHpiDomainInfoT domain_info;
	
	SaHpiEntryIdT 	NextEntryId;
	SaHpiDrtEntryT	DrtEntry;		
	
	oid domain_reference_oid[DOMAIN_REFERENCE_INDEX_NR];
	netsnmp_index domain_reference_index;
	saHpiDomainReferenceTable_context *domain_reference_context;

	DEBUGMSGTL ((AGENT, "poplulate_saHpiDomainReferenceTable: called\n"));
	
	rv = saHpiDomainInfoGet(sessionid, &domain_info);
	if (rv != SA_OK) {	
		DEBUGMSGTL ((AGENT, 
		"poplulate_saHpiDomainReferenceTable: saHpiDomainInfoGet Failed: rv = %d\n",
		rv));
		return AGENT_ERR_INTERNAL_ERROR;
	}		
	
	/* Get the first Drt Entry for this Session */	
	NextEntryId = SAHPI_FIRST_ENTRY;
	do  {
		rv = saHpiDrtEntryGet ( sessionid, 
					NextEntryId, 
					&NextEntryId, 
					&DrtEntry);			
								
		if (rv != SA_OK) {
			DEBUGMSGTL ((AGENT, 
			"poplulate_saHpiDomainReferenceTable: saHpiDEntryGet Failed: rv = %d\n",
			rv));
			return AGENT_ERR_INTERNAL_ERROR;
		}
		
		domain_reference_index.len = DOMAIN_REFERENCE_INDEX_NR;
		domain_reference_oid[0] = domain_info.DomainId;
		domain_reference_oid[1] = DrtEntry.DomainId;
		domain_reference_index.oids = (oid *) & domain_reference_oid;
		
		/* See if it exists. */
		domain_reference_context = NULL;
		domain_reference_context = CONTAINER_FIND (cb.container,&domain_reference_index);
			
		if (!domain_reference_context) { 
			// New entry. Add it
			domain_reference_context = 
				saHpiDomainReferenceTable_create_row ( &domain_reference_index);
		}
		if (!domain_reference_context) {
			snmp_log (LOG_ERR, "Not enough memory for a DomainReference row!");
			return AGENT_ERR_INTERNAL_ERROR;
		}		
		
		/** SaHpiDomainId = ASN_UNSIGNED */
		domain_reference_context->saHpiDomainRef = DrtEntry.DomainId;
		
		/** TruthValue = ASN_INTEGER */
		domain_reference_context->saHpiDomainReferenceIsPeer =
	    	(DrtEntry.IsPeer == SAHPI_TRUE) ? MIB_TRUE : MIB_FALSE;	
                	    
		CONTAINER_INSERT (cb.container, domain_reference_context);
	 																	
	} while ( NextEntryId != SAHPI_LAST_ENTRY);
	
	domain_reference_entry_count = CONTAINER_SIZE (cb.container);
	
	return rv;
}

/**
 * 
 * @sessionid
 * 
 * @return 
 */
SaErrorT clear_domain_reference_entry(SaHpiDomainIdT domain_id)
{
        SaErrorT rv = SA_OK;
        netsnmp_index *row_idx;
        saHpiDomainReferenceTable_context *ctx;

	DEBUGMSGTL ((AGENT, "clear_domain_reference_entry, called\n"));	
	DEBUGMSGTL ((AGENT, "           domainId   [%d]\n", domain_id));	

        row_idx = CONTAINER_FIRST(cb.container);
        if (row_idx) //At least one entry was found.
        {
                do {
                        ctx = CONTAINER_FIND(cb.container, row_idx);
                        
                        row_idx = CONTAINER_NEXT(cb.container, row_idx);

                        if (ctx->index.oids[saHpiDRTDomainId_INDEX] == 
                             domain_id) {

                                /* all conditions met remove row */
                                CONTAINER_REMOVE (cb.container, ctx);
                                saHpiDomainReferenceTable_delete_row (ctx);
                                domain_reference_entry_count = 
                                        CONTAINER_SIZE (cb.container);
                                DEBUGMSGTL ((AGENT, 
                                             "clear_domain_reference_entry:"
                                             " found row: removing\n"));
                        }

                } while (row_idx);
        } 

        return rv;
}

int
handle_saHpiDomainReferenceEntryCount(netsnmp_mib_handler *handler,
                          netsnmp_handler_registration *reginfo,
                          netsnmp_agent_request_info   *reqinfo,
                          netsnmp_request_info         *requests)
{
	DEBUGMSGTL ((AGENT, 
				"handle_saHpiDomainReferenceEntryCount: Entry Count is %d\n", 
				domain_reference_entry_count));

    domain_reference_entry_count = CONTAINER_SIZE (cb.container);
						
    switch(reqinfo->mode) {

        case MODE_GET:
            snmp_set_var_typed_value(	
            	requests->requestvb, 
            	ASN_COUNTER,
                (u_char *) &domain_reference_entry_count,
                sizeof(domain_reference_entry_count));
            break;


        default:
            /* we should never get here, so this is a really bad error */
            return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}

/*
 * int initialize_table_saHpiDomainInfoEntryCount(void)
 */
int
initialize_table_saHpiDomainReferenceEntryCount(void)
{
	DEBUGMSGTL ((AGENT, 
		"initialize_table_saHpiDomainReferenceEntryCount, called\n"));
			
	netsnmp_register_scalar(
		netsnmp_create_handler_registration(
			"saHpiDomainReferenceEntryCount", 
			handle_saHpiDomainReferenceEntryCount,
			saHpiDomainReferenceEntryCount_oid, 
			OID_LENGTH(saHpiDomainReferenceEntryCount_oid),
			HANDLER_CAN_RONLY )); 
	return 0;
}
/************************************************************/
/************************************************************/
/************************************************************/
/************************************************************/

/************************************************************
 * keep binary tree to find context by name
 */
static int saHpiDomainReferenceTable_cmp( const void *lhs, const void *rhs );

/************************************************************
 * compare two context pointers here. Return -1 if lhs < rhs,
 * 0 if lhs == rhs, and 1 if lhs > rhs.
 */
static int
saHpiDomainReferenceTable_cmp( const void *lhs, const void *rhs )
{
    saHpiDomainReferenceTable_context *context_l =
        (saHpiDomainReferenceTable_context *)lhs;
    saHpiDomainReferenceTable_context *context_r =
        (saHpiDomainReferenceTable_context *)rhs;

    /*
     * check primary key, then secondary. Add your own code if
     * there are more than 2 indexes
     */

	DEBUGMSGTL ((AGENT, "saHpiDomainReferenceTable_cmp, called\n"));

	/* check for NULL pointers */
	if(lhs == NULL || rhs == NULL ) {
	    DEBUGMSGTL((AGENT,
	        "saHpiDomainReferenceTable_cmp() NULL pointer ERROR\n" ));
		return 0;
	}	
	/* CHECK FIRST INDEX,  saHpiDomainId */
	if ( context_l->index.oids[0] < context_r->index.oids[0])
	    return -1;
		
	if ( context_l->index.oids[0] > context_r->index.oids[0])
	    return 1;			         
	       
	if ( context_l->index.oids[0] == context_r->index.oids[0]) {
	       /* If saHpiDomainId index is equal sort by second index */
	       /* CHECK SECOND INDEX,  saHpiDomainRef */
	       if ( context_l->index.oids[1] < context_r->index.oids[1])
		  return -1;
		
	       if ( context_l->index.oids[1] > context_r->index.oids[1])
		  return 1;

	       if ( context_l->index.oids[1] == context_r->index.oids[1])
		  return 0;
	}
	return 0;
}

/************************************************************
 * Initializes the saHpiDomainReferenceTable module
 */
void
init_saHpiDomainReferenceTable(void)
{

    DEBUGMSGTL ((AGENT, "init_saHpiDomainReferenceTable, called\n"));

    initialize_table_saHpiDomainReferenceTable();
    
    initialize_table_saHpiDomainReferenceEntryCount();

}

/************************************************************
 * the *_row_copy routine
 */
static int saHpiDomainReferenceTable_row_copy(saHpiDomainReferenceTable_context * dst,
                         saHpiDomainReferenceTable_context * src)
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
    dst->saHpiDomainRef = src->saHpiDomainRef;

    dst->saHpiDomainReferenceIsPeer = src->saHpiDomainReferenceIsPeer;

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
saHpiDomainReferenceTable_extract_index( 
	saHpiDomainReferenceTable_context * ctx, netsnmp_index * hdr )
{
    /*
     * temporary local storage for extracting oid index
     *
     * extract index uses varbinds (netsnmp_variable_list) to parse
     * the index OID into the individual components for each index part.
     */
    netsnmp_variable_list var_saHpiDomainId;
    netsnmp_variable_list var_saHpiDomainRef;
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
       	memset( &var_saHpiDomainId, 0x00, sizeof(var_saHpiDomainId) );
       	var_saHpiDomainId.type = ASN_UNSIGNED;    
       	var_saHpiDomainId.next_variable = &var_saHpiDomainRef; 
       
       	memset( &var_saHpiDomainRef, 0x00, sizeof(var_saHpiDomainRef) );
       	var_saHpiDomainRef.type = ASN_UNSIGNED; 
		var_saHpiDomainRef.next_variable = NULL; 
		
    /*
     * parse the oid into the individual index components
     */
    err = parse_oid_indexes( hdr->oids, hdr->len, &var_saHpiDomainId );
    if (err == SNMP_ERR_NOERROR) {
       /*
        * copy index components into the context structure
        */
              /** skipping external index saHpiDomainId */
   
                ctx->saHpiDomainRef = *var_saHpiDomainRef.val.integer;
   
   
           /*
            * TODO: check index for valid values. For EXAMPLE:
            *
              * if ( *var_saHpiDomainId.val.integer != XXX ) {
          *    err = -1;
          * }
          */
           /*
            * TODO: check index for valid values. For EXAMPLE:
            *
              * if ( *var_saHpiDomainRef.val.integer != XXX ) {
          *    err = -1;
          * }
          */
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
int saHpiDomainReferenceTable_can_activate(saHpiDomainReferenceTable_context *undo_ctx,
                      saHpiDomainReferenceTable_context *row_ctx,
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
int saHpiDomainReferenceTable_can_deactivate(saHpiDomainReferenceTable_context *undo_ctx,
                        saHpiDomainReferenceTable_context *row_ctx,
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
int saHpiDomainReferenceTable_can_delete(saHpiDomainReferenceTable_context *undo_ctx,
                    saHpiDomainReferenceTable_context *row_ctx,
                    netsnmp_request_group * rg)
{
    /*
     * probably shouldn't delete a row that we can't
     * deactivate.
     */
    if(saHpiDomainReferenceTable_can_deactivate(undo_ctx,row_ctx,rg) != 1)
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
 * returns a newly allocated saHpiDomainReferenceTable_context
 *   structure if the specified indexes are not illegal
 * returns NULL for errors or illegal index values.
 */
saHpiDomainReferenceTable_context *
saHpiDomainReferenceTable_create_row( netsnmp_index* hdr)
{
    saHpiDomainReferenceTable_context * ctx =
        SNMP_MALLOC_TYPEDEF(saHpiDomainReferenceTable_context);
    if(!ctx)
        return NULL;
        
    /*
     * TODO: check indexes, if necessary.
     */
    if(saHpiDomainReferenceTable_extract_index( ctx, hdr )) {
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
    */

    return ctx;
}

/************************************************************
 * the *_duplicate row routine
 */
saHpiDomainReferenceTable_context *
saHpiDomainReferenceTable_duplicate_row( saHpiDomainReferenceTable_context * row_ctx)
{
    saHpiDomainReferenceTable_context * dup;

    if(!row_ctx)
        return NULL;

    dup = SNMP_MALLOC_TYPEDEF(saHpiDomainReferenceTable_context);
    if(!dup)
        return NULL;
        
    if(saHpiDomainReferenceTable_row_copy(dup,row_ctx)) {
        free(dup);
        dup = NULL;
    }

    return dup;
}

/************************************************************
 * the *_delete_row method is called to delete a row.
 */
netsnmp_index * saHpiDomainReferenceTable_delete_row( saHpiDomainReferenceTable_context * ctx )
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
void saHpiDomainReferenceTable_set_reserve1( netsnmp_request_group *rg )
{
//    saHpiDomainReferenceTable_context *row_ctx =
//            (saHpiDomainReferenceTable_context *)rg->existing_row;
//    saHpiDomainReferenceTable_context *undo_ctx =
//            (saHpiDomainReferenceTable_context *)rg->undo_info;
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

        default: /** We shouldn't get here */
            rc = SNMP_ERR_GENERR;
            snmp_log(LOG_ERR, "unknown column in "
                     "saHpiDomainReferenceTable_set_reserve1\n");
        }

        if (rc)
           netsnmp_request_set_error( current->ri, rc );
        rg->status = SNMP_MAX( rg->status, current->ri->status );
    }

    /*
     * done with all the columns. Could check row related
     * requirements here.
     */
}

void saHpiDomainReferenceTable_set_reserve2( netsnmp_request_group *rg )
{
//    saHpiDomainReferenceTable_context *row_ctx = (saHpiDomainReferenceTable_context *)rg->existing_row;
//    saHpiDomainReferenceTable_context *undo_ctx = (saHpiDomainReferenceTable_context *)rg->undo_info;
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

        default: /** We shouldn't get here */
            netsnmp_assert(0); /** why wasn't this caught in reserve1? */
        }

        if (rc)
           netsnmp_request_set_error( current->ri, rc);
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
void saHpiDomainReferenceTable_set_action( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
//    saHpiDomainReferenceTable_context *row_ctx = (saHpiDomainReferenceTable_context *)rg->existing_row;
//   saHpiDomainReferenceTable_context *undo_ctx = (saHpiDomainReferenceTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

//    int            row_err = 0;

    /*
     * TODO: loop through columns, copy varbind values
     * to context structure for the row.
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;

        switch(current->tri->colnum) {

        default: /** We shouldn't get here */
            netsnmp_assert(0); /** why wasn't this caught in reserve1? */
        }
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
void saHpiDomainReferenceTable_set_commit( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
//    saHpiDomainReferenceTable_context *row_ctx = (saHpiDomainReferenceTable_context *)rg->existing_row;
//    saHpiDomainReferenceTable_context *undo_ctx = (saHpiDomainReferenceTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

    /*
     * loop through columns
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;

        switch(current->tri->colnum) {

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
void saHpiDomainReferenceTable_set_free( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
//    saHpiDomainReferenceTable_context *row_ctx = (saHpiDomainReferenceTable_context *)rg->existing_row;
//    saHpiDomainReferenceTable_context *undo_ctx = (saHpiDomainReferenceTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

    /*
     * loop through columns
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;

        switch(current->tri->colnum) {

        default: /** We shouldn't get here */
        	break;    /** should have been logged in reserve1 */
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
void saHpiDomainReferenceTable_set_undo( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
//    saHpiDomainReferenceTable_context *row_ctx = (saHpiDomainReferenceTable_context *)rg->existing_row;
//    saHpiDomainReferenceTable_context *undo_ctx = (saHpiDomainReferenceTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

    /*
     * loop through columns
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;

        switch(current->tri->colnum) {

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
 * Initialize the saHpiDomainReferenceTable table by defining its contents and how it's structured
 */
void
initialize_table_saHpiDomainReferenceTable(void)
{
    netsnmp_table_registration_info *table_info;
    
    DEBUGMSGTL ((AGENT, "initialize_table_saHpiDomainReferenceTable, called\n"));

    if(my_handler) {
        snmp_log(LOG_ERR, "initialize_table_saHpiDomainReferenceTable_handler called again\n");
        return;
    }

    memset(&cb, 0x00, sizeof(cb));

    /** create the table structure itself */
    table_info = SNMP_MALLOC_TYPEDEF(netsnmp_table_registration_info);

    /* if your table is read only, it's easiest to change the
       HANDLER_CAN_RWRITE definition below to HANDLER_CAN_RONLY */
    my_handler = netsnmp_create_handler_registration("saHpiDomainReferenceTable",
                                             netsnmp_table_array_helper_handler,
                                             saHpiDomainReferenceTable_oid,
                                             saHpiDomainReferenceTable_oid_len,
                                             HANDLER_CAN_RWRITE);
            
    if (!my_handler || !table_info) {
        snmp_log(LOG_ERR, "malloc failed in "
                 "initialize_table_saHpiDomainReferenceTable_handler\n");
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
        /** index: saHpiDomainRef */
        netsnmp_table_helper_add_index(table_info, ASN_UNSIGNED);

    table_info->min_column = saHpiDomainReferenceTable_COL_MIN;
    table_info->max_column = saHpiDomainReferenceTable_COL_MAX;

    /***************************************************
     * registering the table with the master agent
     */
    cb.get_value = saHpiDomainReferenceTable_get_value;
    cb.container = netsnmp_container_find("saHpiDomainReferenceTable_primary:"
                                          "saHpiDomainReferenceTable:"
                                          "table_container");

    netsnmp_container_add_index(cb.container,
                                netsnmp_container_find("saHpiDomainReferenceTable_secondary:"
                                                       "saHpiDomainReferenceTable:"
                                                       "table_container"));
    cb.container->next->compare = saHpiDomainReferenceTable_cmp;


    cb.can_set = 1;

    cb.create_row = (UserRowMethod*)saHpiDomainReferenceTable_create_row;

    cb.duplicate_row = (UserRowMethod*)saHpiDomainReferenceTable_duplicate_row;
    cb.delete_row = (UserRowMethod*)saHpiDomainReferenceTable_delete_row;
    cb.row_copy = (Netsnmp_User_Row_Operation *)saHpiDomainReferenceTable_row_copy;

    cb.can_activate = (Netsnmp_User_Row_Action *)saHpiDomainReferenceTable_can_activate;
    cb.can_deactivate = (Netsnmp_User_Row_Action *)saHpiDomainReferenceTable_can_deactivate;
    cb.can_delete = (Netsnmp_User_Row_Action *)saHpiDomainReferenceTable_can_delete;

    cb.set_reserve1 = saHpiDomainReferenceTable_set_reserve1;
    cb.set_reserve2 = saHpiDomainReferenceTable_set_reserve2;
    cb.set_action = saHpiDomainReferenceTable_set_action;
    cb.set_commit = saHpiDomainReferenceTable_set_commit;
    cb.set_free = saHpiDomainReferenceTable_set_free;
    cb.set_undo = saHpiDomainReferenceTable_set_undo;

    DEBUGMSGTL(("initialize_table_saHpiDomainReferenceTable",
                "Registering table saHpiDomainReferenceTable "
                "as a table array\n"));
    netsnmp_table_container_register(my_handler, table_info, &cb,
                                     cb.container, 1);
}

/************************************************************
 * saHpiDomainReferenceTable_get_value
 *
 * This routine is called for get requests to copy the data
 * from the context to the varbind for the request. If the
 * context has been properly maintained, you don't need to
 * change in code in this fuction.
 */
int saHpiDomainReferenceTable_get_value(
            netsnmp_request_info *request,
            netsnmp_index *item,
            netsnmp_table_request_info *table_info )
{
    netsnmp_variable_list *var = request->requestvb;
    saHpiDomainReferenceTable_context *context = (saHpiDomainReferenceTable_context *)item;

    switch(table_info->colnum) {

        case COLUMN_SAHPIDOMAINREF:
            /** SaHpiDomainId = ASN_UNSIGNED */
            snmp_set_var_typed_value(var, ASN_UNSIGNED,
                         (char*)&context->saHpiDomainRef,
                         sizeof(context->saHpiDomainRef) );
        break;
    
        case COLUMN_SAHPIDOMAINREFERENCEISPEER:
            /** TruthValue = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->saHpiDomainReferenceIsPeer,
                         sizeof(context->saHpiDomainReferenceIsPeer) );
        break;
    
    default: /** We shouldn't get here */
        snmp_log(LOG_ERR, "unknown column in "
                 "saHpiDomainReferenceTable_get_value\n");
        return SNMP_ERR_GENERR;
    }
    return SNMP_ERR_NOERROR;
}

/************************************************************
 * saHpiDomainReferenceTable_get_by_idx
 */
const saHpiDomainReferenceTable_context *
saHpiDomainReferenceTable_get_by_idx(netsnmp_index * hdr)
{
    return (const saHpiDomainReferenceTable_context *)
        CONTAINER_FIND(cb.container, hdr );
}


