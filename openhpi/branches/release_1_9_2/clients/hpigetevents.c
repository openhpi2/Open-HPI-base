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
 *      Peter D Phan <pdphan@us.ibm.com>
 *      Steve Sherman <stevees@us.ibm.com>
 *
 *     10/13/2004 kouzmich  changed -t option for infinite wait
 *			    added -d option for call saHpiDiscover after saHpiSubscribe
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include <SaHpi.h>
#include <oh_utils.h>

#include <unistd.h>

#define HPI_NSEC_PER_SEC 1000000000LL

char progver[] = "0.1";
int fdebug = 0;

static void Usage(char **argv)
{
	printf("Usage %s [-t <value> | SAHPI_TIMEOUT_BLOCK | BLOCK] [-d] [-x]\n",argv[0]);
	printf("	where:	-t <value> - wait <value> seconds for event;\n");
	printf("		-t SAHPI_TIMEOUT_BLOCK or BLOCK - infinite wait\n");
	printf("		-d - call saHpiDiscover() after saHpiSubscribe()\n");
	printf("		-x - displays eXtra debug messages\n");
}

int main(int argc, char **argv)
{
        int c, wait = 0;
	char *timeout_str= (char *)NULL;
	int do_discover_after_subscribe = 0;
        SaErrorT rv;

        /*SaHpiVersionT hpiVer;*/
        SaHpiSessionIdT sessionid;
        
        /* Domain */
        SaHpiDomainInfoT domainInfo;

        /*SaHpiRptInfoT rptinfo;*/
        SaHpiRptEntryT rptentry;
        SaHpiEntryIdT rptentryid;
        SaHpiEntryIdT nextrptentryid;
        SaHpiResourceIdT resourceid;
        SaHpiEventLogInfoT info;
        SaHpiRdrT rdr;
        SaHpiTimeoutT timeout; 
        SaHpiEventT event;

        memset(&rptentry, 0, sizeof(rptentry));
        
        printf("%s: version %s\n",argv[0],progver); 

        while ( (c = getopt( argc, argv,"t:xd?")) != EOF ) {
                switch(c) {
                        case 't':
                                timeout_str = optarg;
                                break;
                        case 'd': 
                                do_discover_after_subscribe = 1; 
                                break;
                        case 'x': 
                                fdebug = 1; 
                                break;
                        default:
				Usage(argv);
				exit(1);
                }
        }
        if (timeout_str != (char *)NULL) {
		if ((strcmp(timeout_str, "SAHPI_TIMEOUT_BLOCK") == 0) ||
		    (strcmp(timeout_str, "BLOCK") == 0)) {
			timeout = SAHPI_TIMEOUT_BLOCK;
		} else {
			wait = atoi(timeout_str);
                	timeout = (SaHpiTimeoutT)(wait * HPI_NSEC_PER_SEC);
		}
        } else
                timeout = (SaHpiTimeoutT) SAHPI_TIMEOUT_IMMEDIATE;

        printf("************** timeout:[%lld] ****************\n", timeout);    
        rv = saHpiDiscover(sessionid);
        rv = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID,&sessionid,NULL);
        if (rv != SA_OK) {
                if (rv == SA_ERR_HPI_ERROR) 
                        printf("saHpiSessionOpen: error %d, SpiLibd not running\n",rv);
                else
                        printf("saHpiSessionOpen: %s\n", oh_lookup_error(rv));
                exit(-1);
        }
 
        printf( "Subscribe to events\n");
        rv = saHpiSubscribe( sessionid );
        if (rv != SA_OK) return rv;

	if (do_discover_after_subscribe) {
        	rv = saHpiDiscover(sessionid);
	        if (fdebug) printf("saHpiDiscover %s\n", oh_lookup_error(rv));
	}

        rv = saHpiDomainInfoGet(sessionid, &domainInfo);

        if (fdebug) printf("saHpiDomainInfoGet %s\n", oh_lookup_error(rv));
        printf("DomainInfo: UpdateCount = %d, UpdateTime = %lx\n",
               domainInfo.RptUpdateCount, (unsigned long)domainInfo.RptUpdateTimestamp);
        
        /* walk the RPT list */
        rptentryid = SAHPI_FIRST_ENTRY;
        while ((rv == SA_OK) && (rptentryid != SAHPI_LAST_ENTRY))
        {       
                printf("**********************************************\n");

                rv = saHpiRptEntryGet(sessionid, rptentryid, &nextrptentryid, &rptentry);

                if (fdebug) 
                        printf("saHpiRptEntryGet %s\n", oh_lookup_error(rv));

                if (rv == SA_OK) {

                        resourceid = rptentry.ResourceId;

                        if (fdebug) 
                                printf("RPT %x capabilities = %x\n", 
                                       resourceid,
                                       rptentry.ResourceCapabilities);

                        if ( (rptentry.ResourceCapabilities & SAHPI_CAPABILITY_EVENT_LOG)) {
                                /* Using EventLogInfo to build up event queue - for now */
                                rv = saHpiEventLogInfoGet(sessionid,resourceid,&info);
				
                                if (fdebug) 
                                        printf("saHpiEventLogInfoGet %s\n", oh_lookup_error(rv));
					
				if (rv == SA_OK) 
					oh_print_eventloginfo(&info, 4);
                        } else {
                                if (fdebug) 
                                        printf("RPT doesn't have SEL\n");
                        }

                        rptentry.ResourceTag.Data[rptentry.ResourceTag.DataLength] = 0; 

                        printf("rptentry[%d] tag: %s\n", resourceid,rptentry.ResourceTag.Data);

                        rptentryid = nextrptentryid;

                }

                printf("**********************************************\n");
        }
        
        printf( "Go and get the event\n");
	//int count = 0;
        //while (count < 10) {
	while (1) {

		rdr.RdrType = SAHPI_NO_RECORD;

		rv = saHpiEventGet( sessionid, timeout, &event, &rdr, &rptentry, NULL);
                if (rv != SA_OK) { 
                        if (rv != SA_ERR_HPI_TIMEOUT) {
                                printf( "Error during EventGet : %s - Test FAILED\n",
					oh_lookup_error(rv));
                                break;
                        } else {
				if (timeout == SAHPI_TIMEOUT_BLOCK) {
					printf("Timeout while infinite wait - Test FAILED\n");
				} else {
                                	printf( "\n\n****** Time, %d seconds, expired waiting for"
					        " event.\n", wait);
				}
                                break;
                        }
                } else {
			oh_print_event(&event, 4);			
                }
		//count++;
        }

        /* Unsubscribe to future events */
        printf( "Unsubscribe\n");
        rv = saHpiUnsubscribe( sessionid );

        rv = saHpiSessionClose(sessionid);

        printf("HPIGETEVENT END\n");
        
        exit(0);
}
 
/* end hpigetevents.c */
