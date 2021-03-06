/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003, 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Renier Morales <renierm@users.sf.net>
 */

/************************************************************************
 * Warning! If you change this file, please make the corresponding change
 *          t/snmp_bc_session.c to keep the simulator code in sync.
 ************************************************************************/

#include <SaHpi.h>

#include <snmp_util.h>
#include <snmp_bc_session.h>
#include <snmp_bc.h>
#include <rpt_utils.h>
#include <sel_utils.h>
#include <bc_resources.h>
#include <bc_str2event.h>
#include <snmp_bc_event.h>

/**
 * snmp_bc_open: open snmp blade center plugin
 * @handler_config: hash table passed by infrastructure
 **/

void *snmp_bc_open(GHashTable *handler_config)
{
        struct oh_handler_state *handle;
        struct snmp_bc_hnd *custom_handle;
        char *root_tuple;

        root_tuple = (char *)g_hash_table_lookup(handler_config, "entity_root");
        if(!root_tuple) {
                dbg("ERROR: Cannot open snmp_bc plugin. No entity root found in configuration.");
                return NULL;
        }
        
        handle = (struct oh_handler_state *)g_malloc0(sizeof(struct oh_handler_state));
        custom_handle =
                (struct snmp_bc_hnd *)g_malloc0(sizeof(struct snmp_bc_hnd));
        if(!handle || !custom_handle) {
                dbg("Could not allocate memory for handle or custom_handle.");
                return NULL;
        }
        handle->data = custom_handle;
        
        handle->config = handler_config;

        /* Initialize RPT cache */
        handle->rptcache = (RPTable *)g_malloc0(sizeof(RPTable));
 
        /* Initialize SEL cache */
        handle->selcache =  oh_sel_create(OH_SEL_MAX_SIZE);

	/* Initialize String-to-Event hash table */
	if (str2event_hash_init()) {
		dbg("Couldn't initialize str2event hash table.");
		return NULL;
	}
	
	/* Initialize BC_Event_Number-to-HPI_Event hash table */
	if (event2hpi_hash_init()) {
		dbg("Couldn't initialize event2hpi hash table.");
		return NULL;
	}
        
        /* Initialize snmp library */
        init_snmp("oh_snmp_bc");
        
        snmp_sess_init(&(custom_handle->session)); /* Setting up all defaults for now. */
        custom_handle->session.peername = (char *)g_hash_table_lookup(handle->config, "host");

        /* set the SNMP version number */
        custom_handle->session.version = SNMP_VERSION_1;

        /* set the SNMPv1 community name used for authentication */
        custom_handle->session.community = (char *)g_hash_table_lookup(handle->config, "community");
        custom_handle->session.community_len = strlen(custom_handle->session.community);

        /* windows32 specific net-snmp initialization (is a noop on unix) */
        SOCK_STARTUP;

        custom_handle->ss = snmp_open(&(custom_handle->session));

        if(!custom_handle->ss) {
                snmp_perror("ack");
                snmp_log(LOG_ERR, "something horrible happened!!!\n");
                dbg("Unable to open snmp session.");
                return NULL;
        }

	/* Set BladeCenter Type */
	{
		struct snmp_value  get_value;

		if ((snmp_get(custom_handle->ss, SNMP_BC_BLADECENTER_TYPE, &get_value) != 0) ||
		    (get_value.type != ASN_OCTET_STR)) {
			dbg("SNMP could not read %s; Type=%d.\n", 
			    SNMP_BC_BLADECENTER_TYPE, get_value.type);
			return NULL;
		}

		if (!strcmp(get_value.string, SNMP_BC_BCE_MODEL)) {
			strcpy(bc_type, SNMP_BC_PLATFORM_BCE);
		}
		else if (!strcmp(get_value.string, SNMP_BC_BCT_MODEL_AC)) {
			strcpy(bc_type, SNMP_BC_PLATFORM_BCT);
		}
		else if (!strcmp(get_value.string, SNMP_BC_BCT_MODEL_DC)) {
			strcpy(bc_type, SNMP_BC_PLATFORM_BCT);
		}
		else {
			dbg("Unrecognized BladeCenter Type=%s.\n", get_value.string);
			return NULL;
		}
	}

        return handle;
}

/**
 * snmp_bc_close: shut down plugin connection
 * @hnd: a pointer to the snmp_bc_hnd struct that contains
 * a pointer to the snmp session and another to the configuration
 * data.
 **/

void snmp_bc_close(void *hnd)
{
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        struct snmp_bc_hnd *custom_handle =
                (struct snmp_bc_hnd *)handle->data;

        oh_sel_close(handle->selcache);
        snmp_close(custom_handle->ss);
        /* Should we free handle->config? */

        /* windows32 specific net-snmp cleanup (is a noop on unix) */
        SOCK_CLEANUP;

	/* Cleanup str2event hash table */
	str2event_hash_free();

	/* Cleanup event2hpi hash table */
	event2hpi_hash_free();
}

