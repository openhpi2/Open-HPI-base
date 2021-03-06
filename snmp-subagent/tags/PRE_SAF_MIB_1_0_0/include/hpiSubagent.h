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
 *   Konrad Rzeszutek <konradr@us.ibm.com>
 *
 * $Id:
 *
 */

#include <SaHpi.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <glib.h>

#ifndef _HPI_SUBAGENT_
#define _HPI_SUBAGENT_

#define SAF_UNSIGNED_64_LEN 12

/*
 * global mutex used mainly for event servicing"
 */
//GMutex *thread_mutex = NULL;

/*
 * Our Net-SNMP "call-sign"
 */
#define AGENT "hpiSubagent"
//#define AGENT "read_config"
/*
 * Max amount of memory to allocate when using saHpiInventoryEntityDataRead
 */
#define MAXBUF (1024*16)
/*
 * Error codes for all of the functions.
 */

// Everything went fine
#define AGENT_ERR_NOERROR 0
// Problems initializing HPI library (perhaps the plugins are non-existent?)
#define AGENT_ERR_INIT -254
// Problems openning the session (right options in /etc/openhpi.conf file?)
#define AGENT_ERR_SESSION_OPEN -253
// Problems closing the session
#define AGENT_ERR_SESSION_CLOSE -254
// Not enough memory for allocating memory
#define AGENT_ERR_MEMORY_FAULT -253
// Couldn't not perform discovery of resources.
#define AGENT_ERR_DISCOVER -252
// Couldn't get RPT information
#define AGENT_ERR_RPTGET -251
// Internal operation error
#define AGENT_ERR_OPERATION -250
// Internal programmer error. Should never surface to the user.
#define AGENT_ERR_INTERNAL_ERROR -249

#define AGENT_ERR_NOT_FOUND -248

#define AGENT_ERR_BUILD_TRAP -247

#define AGENT_ERR_SUBSCRIBE -246

#define AGENT_ERR_WRONG_DELIM -245

/* current sahpi.h missed SA_ERR_INVENT_DATA_TRUNCATED */
#ifndef SA_ERR_INVENT_DATA_TRUNCATED
//#warning "No 'SA_ERR_INVENT_DATA_TRUNCATED 'definition in sahpi.h!"
#define SA_ERR_INVENT_DATA_TRUNCATED    (SaErrorT)(SA_HPI_ERR_BASE - 1000)
#endif

/*
 * True and false values in this subagent (which are the
 * same as SAHPI_TRUE and SAHPI_FALSE)
 */
#define AGENT_TRUE 1
#define AGENT_FALSE 0

/*
 * True and false values for 'TruthValue' as defined per SMI-v2
 */
#define MIB_TRUE 1
#define MIB_FALSE 2

/*
 * Return codes for adding (or changing) entries in the tables.
 *
 * AGENT_NEW_ENTRY is if we try to add information to the table and 
 * this return code means that the row with such information never existed.
 * In case of a row that did exist (based on the index value) than it will
 * be overwritten.
 *
 * AGENT_ENTRY_EXIST means that the entry exists and has not been
 * altered.
 *
 * AGENT_ERR_NULL_DATA means the data provided is NULL
 *
 * The named value that is used to determine if the row exist or not is
 * 'long hash' value which for new rows is set to '0'; for existing its 
 * the result of the 'calculate_hash_value' which is compared to the given
 * information to determine if data needs to be updated.
 */
#define AGENT_NEW_ENTRY 2
#define AGENT_ENTRY_EXIST 3
#define AGENT_ERR_NULL_DATA 4
/*
 * OIDs for subagent
 */

//  .iso.org.dod.internet.private.enterprises.saforum.experimental.hpiE.openhpiE
#define hpi_OID 1,3,6,1,4,1,18568,1,1,1
#define hpiEntity_OID hpi_OID,1
#define hpiEvents_OID hpi_OID,2
#define hpiResources_OID hpi_OID,3
#define hpiSensor_OID hpiResources_OID,7
#define hpiNotifications_OID hpi_OID,4

#define systemEvents_OID hpiEvents_OID,2
#define events_OID hpiEvents_OID,1

// SnmpTrapOID.0
#define snmptrap_oid 1,3,6,1,6,3,1,1,4,1,0

/*
 * Handler for parsing configuration tokens.
 */
#define TRAPS_TOKEN "send_traps_on_startup"
#define INTERVAL_TOKEN "check_hpi_interval"
#define MAX_EVENT_TOKEN "max_event_rows"

void hpiSubagent_parse_config_traps (const char *, char *);
void hpiSubagent_parse_config_interval (const char *, char *);
void hpiSubagent_parse_config_max_event (const char *, char *);


/*
 * Close the HPI session.
 * 
 * Return:
 *   AGENT_ERR_NOERROR if no problems,
 *   AGENT_ERR_SESSION_CLOSE if encountered problems. Query 'rcSaHpi()' to
 *      find out what kind of return code SAHPI library returns.
 */
int closeSaHpiSession (void);

/*
 * Get the HPI session key. If the session hasn't been started this
 * routine takes care of that.
 *
 * Parameters:
 *  [OUT] SaHpiSessionIdT *out - the value of the session id
 *
 * Return:
 *  AGENT_ERR_NOERROR - everything is OK.
 *  AGENT_ERR_INIT - errors during SaHpiInitialize.
 *  AGENT_ERR_SESSION_OPEN - errors  during SaHpiSessionOpen
 *  AGENT_ERR_DISCOVER - error during saHpiResourcesDiscover
 *
 * Query the 'rcSaHpi()' to get the SAHPI return code.
 */
int getSaHpiSession (SaHpiSessionIdT * out);

/*
 * Find out if the SAHPI information changed. The 'answer'
 * will be set to AGENT_TRUE or AGENT_FALSE and 'info'
 * will be set with the respective latest SaHpiRptInfoT
 * information.
 *
 * Parameters:
 *   [OUT] int *answer
 *   [OUT] SaHpiRptInfoT *info
 *
 * Return:
 *  AGENT_ERR_NOERROR - everything is OK.
 *  AGENT_ERR_INIT - errors during SaHpiInitialize.
 *  AGENT_ERR_SESSION_OPEN - errors  during SaHpiSessionOpen
 *  AGENT_ERR_DISCOVER - error during saHpiResourcesDiscover
 *  AGENT_ERR_RPTGET - error during saHpiRptInfoGet operation
 *  
 */
int didSaHpiChanged (int *answer, void * info);

/*
 * Convert the decimal SAHPI error codes into character
 * strings.
 *
 * @param d error code
 *
 * @return textual representation of the error.
 */
const char * get_error_string(SaErrorT error);

/*
 * Return the latest return code for the SAHPI library operation.
 * This value is not mutexed, so if you perform more than one SAHPI
 * library operation - only the latest will available.
 *
 * Return:
 *  Consult SaHpi.h for the latest information.
 */
SaErrorT rcSaHpi (void);

/*
 * Calculate a hash value of the 'data' which is 'len' bytes
 * big.
 *
 * Note: If 'len' is set outside the boundary of 'data' this will 
 * segment fault your program.
 *
 * @param *data  [IN] void *data - pointer to the data
 * @param len  [IN] int len - length of the data in bytes.
 *
 * @return hash value. 
 */
long calculate_hash_value (void *data, int len);

/*
 * Construct a full OID from three different sources:
 *  - prefix 
 *  - column
 *  - index 
 *
 * 
 * @param prefix [IN] The 'prefix' should be in a OID format. 
 *       For example a prefix to 'experimental' is  "1,3,6,1,3" (prefix_len is 5).
 * @param column [IN] The 'column' is the OID before the index value. The same format as the prefix
 *       and is optional.
 * @param index [IN] From the 'index' value (which is a netsnmp_index) the OID (and its corresponding
 *       length) is extracted. It is optional.
 * @param out_oid [OUT] out_oid is where the new OID will be stored.
 * @param in_len  [IN] in_len the count (not in bytes) of the allocated 'out_oid'
 * @param out_len [OUT] out_len the count (not in bytes) of the new resulting OID.
 *

 * @return AGENT_ERR_NOERROR - operation went ok
 * @return AGENT_ERR_MEMORY_FAULT - the resulting OID would be bigger than the 'in_len'
 *    count.
 */
int build_full_oid (oid * prefix, size_t prefix_len,
		    oid * column, size_t column_len,
		    netsnmp_index * index,
		    oid * out_oid, size_t in_len, size_t * out_len);

#ifndef TRAP_VARS_HAVE
#define TRAP_VARS_HAVE
typedef struct trap_vars_
{
  int column;
  u_char type;
  u_char *value;
  size_t value_len;
} trap_vars;
#endif

netsnmp_variable_list *build_notification (const netsnmp_index * index,
					   const trap_vars * var,
					   const size_t var_len,
					   const oid * notification_oid,
					   const size_t notification_oid_len,
					   const oid * root_table_oid,
					   const size_t root_table_oid_len,
					   const SaHpiDomainIdT domain_id,
					   const oid * domain_id_oid,
					   const size_t domain_id_oid_len,
					   const SaHpiResourceIdT resource_id,
					   const oid * resource_id_oid,
					   const size_t resource_id_oid_len);


#endif /* _HPI_SUBAGENT_ */
