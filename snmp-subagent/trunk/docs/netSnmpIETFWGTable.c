/*
 * Note: this file originally auto-generated by mib2c using
 *       : mib2c.array-user.conf,v 5.15 2002/12/18 00:33:10 rstory Exp $
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
 *     http://www.net-snmp.org/tutorial/
 *
 *
 * You can also join the #net-snmp channel on irc.openprojects.net
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

#include "netSnmpIETFWGTable.h"

static     netsnmp_handler_registration *my_handler = NULL;
static     netsnmp_table_array_callbacks cb;

oid netSnmpIETFWGTable_oid[] = { netSnmpIETFWGTable_TABLE_OID };
size_t netSnmpIETFWGTable_oid_len = OID_LENGTH(netSnmpIETFWGTable_oid);


#ifdef netSnmpIETFWGTable_IDX2
/************************************************************
 * keep binary tree to find context by name
 */
static int netSnmpIETFWGTable_cmp( const void *lhs, const void *rhs );

/************************************************************
 * compare two context pointers here. Return -1 if lhs < rhs,
 * 0 if lhs == rhs, and 1 if lhs > rhs.
 */
static int
netSnmpIETFWGTable_cmp( const void *lhs, const void *rhs )
{
    netSnmpIETFWGTable_context *context_l =
        (netSnmpIETFWGTable_context *)lhs;
    netSnmpIETFWGTable_context *context_r =
        (netSnmpIETFWGTable_context *)rhs;

    /*
     * check primary key, then secondary. Add your own code if
     * there are more than 2 indexes
     */
    int rc;

    /*
     * TODO: implement compare. Remove this ifdef code and
     * add your own code here.
     */
#ifdef TABLE_CONTAINER_TODO
    snmp_log(LOG_ERR,
             "netSnmpIETFWGTable_compare not implemented! Container order undefined\n" );
    return 0;
#endif
    
    /*
     * EXAMPLE:
     *   
     * rc = strncmp( context_l->xxName, context_r->xxName,
     *               SNMP_MIN(context_l->xxName_len, context_r->xxName_len) );
     *
     * if(rc)
     *   return rc;
     *
     * TODO: fix secondary keys (or delete if there are none)
     *
     * if(context_l->yy < context_r->yy) 
     *   return -1;
     *
     * return (context_l->yy == context_r->yy) ? 0 : 1;
     */
}

/************************************************************
 * search tree
 */
/** TODO: set additional indexes as parameters */
netSnmpIETFWGTable_context *
netSnmpIETFWGTable_get( const char *name, int len )
{
    netSnmpIETFWGTable_context tmp;

    /** we should have a secondary index */
    netsnmp_assert(cb.container->next != NULL);
    
    /*
     * TODO: implement compare. Remove this ifdef code and
     * add your own code here.
     */
#ifdef TABLE_CONTAINER_TODO
    snmp_log(LOG_ERR, "netSnmpIETFWGTable_get not implemented!\n" );
    return NULL;
#endif

    /*
     * EXAMPLE:
     *
     * if(len > sizeof(tmp.xxName))
     *   return NULL;
     *
     * strncpy( tmp.xxName, name, sizeof(tmp.xxName) );
     * tmp.xxName_len = len;
     *
     * return CONTAINER_FIND(cb.container->next, &tmp);
     */
}
#endif

static void
print_string(netsnmp_index *i, void *v)
{
    int a;
    printf("item %p = [",i);
    for (a = 1; a <= i->oids[0]; a++)
	printf("%c", (char) i->oids[a]);
    printf("]\n");
}

/************************************************************
 * Initializes the netSnmpIETFWGTable module
 */
void
init_netSnmpIETFWGTable(void)
{
   netsnmp_index index;
   oid index_oid[MAX_OID_LEN];
   char *index_char[] = {"hickory","joe","hickory","bob","new orleans","help"};
   int i,j;
   netSnmpIETFWGTable_context *ctx;

   initialize_table_netSnmpIETFWGTable();

   for (i = 0; i< 6; i++) {
     /*
       First value of an index that is ASN_OCTET_STR is 
       the length of the string.
     */
	index_oid[0] = strlen(index_char[i]);
	/* The rest is the data copied. */
	for (j = 0; j < index_oid[0];j++) {
		index_oid[j+1] = *(index_char[i]+j);	

	}
        index.oids = (oid *) &index_oid;
        index.len = index_oid[0]+1;
	ctx = NULL;
	/* Search for it first. */
	ctx = CONTAINER_FIND (cb.container, &index);
	if (!ctx) {
	  /* No dice. We add the new row */
		ctx = netSnmpIETFWGTable_create_row( &index);
		printf("inserting %s\n", ctx->	 nsIETFWGName);
		CONTAINER_INSERT (cb.container, ctx);
	}

   } 
   /*
    Since we are done adding the rows, let us display them for the fun.
    The easy way:
   */

   CONTAINER_FOR_EACH(cb.container, print_string, NULL);

   /*     
   We do not like 'joe', so we remove him.
   */
   index_oid[0] = 3;
   index_oid[1] = 'j'; index_oid[2] = 'o'; index_oid[3] = 'e';
   index.oids = (oid *) &index_oid;
   index.len = 4;
   CONTAINER_REMOVE( cb.container, &index);
   printf("Removed joe\n");

   /*
     Or the hard way:
   */
   
   ctx = CONTAINER_FIRST(cb.container);
   printf("Find first = %p %s\n",ctx, ctx->nsIETFWGName);
    while( ctx ) {
        ctx = CONTAINER_NEXT(cb.container,ctx);
        if(ctx)
            printf("Find next = %p %s\n",ctx, ctx->nsIETFWGName);
        else
            printf("Find next = %p\n",ctx);
    }

  
}

/************************************************************
 * the *_row_copy routine
 */
static int netSnmpIETFWGTable_row_copy(netSnmpIETFWGTable_context * dst,
                         netSnmpIETFWGTable_context * src)
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
    memcpy( dst->nsIETFWGName, src->nsIETFWGName, src->nsIETFWGName_len );
    dst->nsIETFWGName_len = src->nsIETFWGName_len;

    memcpy( dst->nsIETFWGChair1, src->nsIETFWGChair1, src->nsIETFWGChair1_len );
    dst->nsIETFWGChair1_len = src->nsIETFWGChair1_len;

    memcpy( dst->nsIETFWGChair2, src->nsIETFWGChair2, src->nsIETFWGChair2_len );
    dst->nsIETFWGChair2_len = src->nsIETFWGChair2_len;

    return 0;
}

#ifdef netSnmpIETFWGTable_SET_HANDLING

/*
 * the *_extract_index routine
 */
int
netSnmpIETFWGTable_extract_index( netSnmpIETFWGTable_context * ctx, netsnmp_index * hdr )
{
    /*
     * temporary local storage for extracting oid index
     */
    netsnmp_variable_list var_nsIETFWGName;
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

    /**
     * Create variable to hold each component of the index
     */
       memset( &var_nsIETFWGName, 0x00, sizeof(var_nsIETFWGName) );
       var_nsIETFWGName.type = ASN_OCTET_STR;
       /** TODO: link this index to the next, or NULL for the last one */
#ifdef TABLE_CONTAINER_TODO
    snmp_log(LOG_ERR, "netSnmpIETFWGTable_extract_index index list not implemented!\n" );
    return 0;
#else
       var_nsIETFWGName.next_variable = NULL;
#endif


    /*
     * parse the oid into the individual components
     */
    err = parse_oid_indexes( hdr->oids, hdr->len, &var_nsIETFWGName );
    if (err == SNMP_ERR_NOERROR) {
       /*
        * copy components into the context structure
        */
              /** skipping external index nsIETFWGName */
                if(var_nsIETFWGName.val_len > sizeof(ctx->nsIETFWGName))
                   err = -1;
                else
                    memcpy( ctx->nsIETFWGName, var_nsIETFWGName.val.string, var_nsIETFWGName.val_len );
                ctx->nsIETFWGName_len = var_nsIETFWGName.val_len;
   
   
           /*
            * TODO: check index for valid values. For EXAMPLE:
            *
              * if ( XXX_check_value( var_nsIETFWGName.val.string, XXX ) ) {
          *    err = -1;
          * }
          */
    }

    /*
     * parsing may have allocated memory. free it.
     */
    snmp_reset_var_buffers( &var_nsIETFWGName );

    return err;
}

/************************************************************
 * the *_can_delete routine is called to determine if a row
 * can be deleted.
 *
 * return 1 if the row can be deleted
 * return 0 if the row cannot be deleted
 */
int netSnmpIETFWGTable_can_delete(netSnmpIETFWGTable_context *undo_ctx,
                    netSnmpIETFWGTable_context *row_ctx,
                    netsnmp_request_group * rg)
{
    /*
     * probably shouldn't delete a row that we can't
     * deactivate.
     *
    if(netSnmpIETFWGTable_can_deactivate(undo_ctx,row_ctx,rg) != 1)
        return 0;

     *
     * TODO: check for other deletion requirements here
     */
    return 1;
}

#ifdef netSnmpIETFWGTable_ROW_CREATION
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
 * returns a newly allocated netSnmpIETFWGTable_context
 *   structure if the specified indexes are not illegal
 * returns NULL for errors or illegal index values.
 */
netSnmpIETFWGTable_context *
netSnmpIETFWGTable_create_row( netsnmp_index* hdr)
{
    netSnmpIETFWGTable_context * ctx =
        SNMP_MALLOC_TYPEDEF(netSnmpIETFWGTable_context);
    if(!ctx)
        return NULL;
        
    /*
     * TODO: check indexes, if necessary.
     */
    if(netSnmpIETFWGTable_extract_index( ctx, hdr )) {
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
    ctx->nsIETFWGChair1_len = 0;
    ctx->nsIETFWGChair2_len = 0;

    return ctx;
}
#endif

/************************************************************
 * the *_duplicate row routine
 */
netSnmpIETFWGTable_context *
netSnmpIETFWGTable_duplicate_row( netSnmpIETFWGTable_context * row_ctx)
{
    netSnmpIETFWGTable_context * dup;

    if(!row_ctx)
        return NULL;

    dup = SNMP_MALLOC_TYPEDEF(netSnmpIETFWGTable_context);
    if(!dup)
        return NULL;
        
    if(netSnmpIETFWGTable_row_copy(dup,row_ctx)) {
        free(dup);
        dup = NULL;
    }

    return dup;
}

/************************************************************
 * the *_delete_row method is called to delete a row.
 */
netsnmp_index * netSnmpIETFWGTable_delete_row( netSnmpIETFWGTable_context * ctx )
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
void netSnmpIETFWGTable_set_reserve1( netsnmp_request_group *rg )
{
    netSnmpIETFWGTable_context *row_ctx =
            (netSnmpIETFWGTable_context *)rg->existing_row;
    netSnmpIETFWGTable_context *undo_ctx =
            (netSnmpIETFWGTable_context *)rg->undo_info;
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

        case COLUMN_NSIETFWGCHAIR1:
            /** OCTETSTR = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
                                                sizeof(row_ctx->nsIETFWGChair1));
        break;

        case COLUMN_NSIETFWGCHAIR2:
            /** OCTETSTR = ASN_OCTET_STR */
            rc = netsnmp_check_vb_type_and_size(var, ASN_OCTET_STR,
                                                sizeof(row_ctx->nsIETFWGChair2));
        break;

        default: /** We shouldn't get here */
            rc = SNMP_ERR_GENERR;
            snmp_log(LOG_ERR, "unknown column in "
                     "netSnmpIETFWGTable_set_reserve1\n");
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

void netSnmpIETFWGTable_set_reserve2( netsnmp_request_group *rg )
{
    netSnmpIETFWGTable_context *row_ctx = (netSnmpIETFWGTable_context *)rg->existing_row;
    netSnmpIETFWGTable_context *undo_ctx = (netSnmpIETFWGTable_context *)rg->undo_info;
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

        case COLUMN_NSIETFWGCHAIR1:
            /** OCTETSTR = ASN_OCTET_STR */
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

        case COLUMN_NSIETFWGCHAIR2:
            /** OCTETSTR = ASN_OCTET_STR */
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
void netSnmpIETFWGTable_set_action( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
    netSnmpIETFWGTable_context *row_ctx = (netSnmpIETFWGTable_context *)rg->existing_row;
    netSnmpIETFWGTable_context *undo_ctx = (netSnmpIETFWGTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;


    /*
     * TODO: loop through columns, copy varbind values
     * to context structure for the row.
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;

        switch(current->tri->colnum) {

        case COLUMN_NSIETFWGCHAIR1:
            /** OCTETSTR = ASN_OCTET_STR */
            memcpy(row_ctx->nsIETFWGChair1,var->val.string,var->val_len);
            row_ctx->nsIETFWGChair1_len = var->val_len;
        break;

        case COLUMN_NSIETFWGCHAIR2:
            /** OCTETSTR = ASN_OCTET_STR */
            memcpy(row_ctx->nsIETFWGChair2,var->val.string,var->val_len);
            row_ctx->nsIETFWGChair2_len = var->val_len;
        break;

        default: /** We shouldn't get here */
            netsnmp_assert(0); /** why wasn't this caught in reserve1? */
        }
    }

    /*
     * done with all the columns. Could check row related
     * requirements here.
     */
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
void netSnmpIETFWGTable_set_commit( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
    netSnmpIETFWGTable_context *row_ctx = (netSnmpIETFWGTable_context *)rg->existing_row;
    netSnmpIETFWGTable_context *undo_ctx = (netSnmpIETFWGTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

    /*
     * loop through columns
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;

        switch(current->tri->colnum) {

        case COLUMN_NSIETFWGCHAIR1:
            /** OCTETSTR = ASN_OCTET_STR */
        break;

        case COLUMN_NSIETFWGCHAIR2:
            /** OCTETSTR = ASN_OCTET_STR */
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
void netSnmpIETFWGTable_set_free( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
    netSnmpIETFWGTable_context *row_ctx = (netSnmpIETFWGTable_context *)rg->existing_row;
    netSnmpIETFWGTable_context *undo_ctx = (netSnmpIETFWGTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

    /*
     * loop through columns
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;

        switch(current->tri->colnum) {

        case COLUMN_NSIETFWGCHAIR1:
            /** OCTETSTR = ASN_OCTET_STR */
        break;

        case COLUMN_NSIETFWGCHAIR2:
            /** OCTETSTR = ASN_OCTET_STR */
        break;

        default: /** We shouldn't get here */
            /** should have been logged in reserve1 */
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
void netSnmpIETFWGTable_set_undo( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
    netSnmpIETFWGTable_context *row_ctx = (netSnmpIETFWGTable_context *)rg->existing_row;
    netSnmpIETFWGTable_context *undo_ctx = (netSnmpIETFWGTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

    /*
     * loop through columns
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;

        switch(current->tri->colnum) {

        case COLUMN_NSIETFWGCHAIR1:
            /** OCTETSTR = ASN_OCTET_STR */
        break;

        case COLUMN_NSIETFWGCHAIR2:
            /** OCTETSTR = ASN_OCTET_STR */
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

#endif /** netSnmpIETFWGTable_SET_HANDLING */


/************************************************************
 *
 * Initialize the netSnmpIETFWGTable table by defining its contents and how it's structured
 */
void
initialize_table_netSnmpIETFWGTable(void)
{
    netsnmp_table_registration_info *table_info;

    if(my_handler) {
        snmp_log(LOG_ERR, "initialize_table_netSnmpIETFWGTable_handler called again\n");
        return;
    }

    memset(&cb, 0x00, sizeof(cb));

    /** create the table structure itself */
    table_info = SNMP_MALLOC_TYPEDEF(netsnmp_table_registration_info);

    /* if your table is read only, it's easiest to change the
       HANDLER_CAN_RWRITE definition below to HANDLER_CAN_RONLY */
    my_handler = netsnmp_create_handler_registration("netSnmpIETFWGTable",
                                             netsnmp_table_array_helper_handler,
                                             netSnmpIETFWGTable_oid,
                                             netSnmpIETFWGTable_oid_len,
                                             HANDLER_CAN_RWRITE);
            
    if (!my_handler || !table_info) {
        snmp_log(LOG_ERR, "malloc failed in "
                 "initialize_table_netSnmpIETFWGTable_handler\n");
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
        /** index: nsIETFWGName */
        netsnmp_table_helper_add_index(table_info, ASN_OCTET_STR);

    table_info->min_column = netSnmpIETFWGTable_COL_MIN;
    table_info->max_column = netSnmpIETFWGTable_COL_MAX;

    /***************************************************
     * registering the table with the master agent
     */
    cb.get_value = netSnmpIETFWGTable_get_value;
    cb.container = netsnmp_container_find("netSnmpIETFWGTable_primary:"
                                          "netSnmpIETFWGTable:"
                                          "table_container");
#ifdef netSnmpIETFWGTable_IDX2
    netsnmp_container_add_index(cb.container,
                                netsnmp_container_find("netSnmpIETFWGTable_secondary:"
                                                       "netSnmpIETFWGTable:"
                                                       "table_container"));
    cb.container->next->compare = netSnmpIETFWGTable_cmp;
#endif
#ifdef netSnmpIETFWGTable_SET_HANDLING
    cb.can_set = 1;
#ifdef netSnmpIETFWGTable_ROW_CREATION
    cb.create_row = (UserRowMethod*)netSnmpIETFWGTable_create_row;
#endif
    cb.duplicate_row = (UserRowMethod*)netSnmpIETFWGTable_duplicate_row;
    cb.delete_row = (UserRowMethod*)netSnmpIETFWGTable_delete_row;
    cb.row_copy = (Netsnmp_User_Row_Operation *)netSnmpIETFWGTable_row_copy;
/*
    cb.can_activate = (Netsnmp_User_Row_Action *)netSnmpIETFWGTable_can_activate;
    cb.can_deactivate = (Netsnmp_User_Row_Action *)netSnmpIETFWGTable_can_deactivate;
*/
    cb.can_delete = (Netsnmp_User_Row_Action *)netSnmpIETFWGTable_can_delete;

    cb.set_reserve1 = netSnmpIETFWGTable_set_reserve1;
    cb.set_reserve2 = netSnmpIETFWGTable_set_reserve2;
    cb.set_action = netSnmpIETFWGTable_set_action;
    cb.set_commit = netSnmpIETFWGTable_set_commit;
    cb.set_free = netSnmpIETFWGTable_set_free;
    cb.set_undo = netSnmpIETFWGTable_set_undo;
#endif
    DEBUGMSGTL(("initialize_table_netSnmpIETFWGTable",
                "Registering table netSnmpIETFWGTable "
                "as a table array\n"));
    netsnmp_table_container_register(my_handler, table_info, &cb,
                                     cb.container, 1);
}

/************************************************************
 * netSnmpIETFWGTable_get_value
 */
int netSnmpIETFWGTable_get_value(
            netsnmp_request_info *request,
            netsnmp_index *item,
            netsnmp_table_request_info *table_info )
{
    netsnmp_variable_list *var = request->requestvb;
    netSnmpIETFWGTable_context *context = (netSnmpIETFWGTable_context *)item;

    switch(table_info->colnum) {

        case COLUMN_NSIETFWGNAME:
            /** OCTETSTR = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->nsIETFWGName,
                         context->nsIETFWGName_len );
        break;
    
        case COLUMN_NSIETFWGCHAIR1:
            /** OCTETSTR = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->nsIETFWGChair1,
                         context->nsIETFWGChair1_len );
        break;
    
        case COLUMN_NSIETFWGCHAIR2:
            /** OCTETSTR = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->nsIETFWGChair2,
                         context->nsIETFWGChair2_len );
        break;
    
    default: /** We shouldn't get here */
        snmp_log(LOG_ERR, "unknown column in "
                 "netSnmpIETFWGTable_get_value\n");
        return SNMP_ERR_GENERR;
    }
    return SNMP_ERR_NOERROR;
}

/************************************************************
 * netSnmpIETFWGTable_get_by_idx
 */
const netSnmpIETFWGTable_context *
netSnmpIETFWGTable_get_by_idx(netsnmp_index * hdr)
{
    return (const netSnmpIETFWGTable_context *)
        CONTAINER_FIND(cb.container, hdr );
}


