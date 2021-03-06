/*      -*- set_resources-c -*-
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Unknown (initial author of list_resources.c)
 *     Julie Fleischer <julie.n.fleischer@intel.com>
 */

/* set_resources.c - This program was put together from
 * list_resources.c to test setting thresholds in the
 * sysfs plugin.  It should be able to work for other plugins
 * too, but you may want to change the test cases.
 */
#include <stdlib.h>
#include <stdio.h>
#include <SaHpi.h>
#include <unistd.h>
#include <string.h>
#include <epath_utils.h>

/* debug macros */
#define warn(str) fprintf(stderr,"%s: " str "\n", __FUNCTION__)
#define error(str, e) fprintf(stderr,str ": %s\n", get_error_string(e))

/* Function prototypes */
SaErrorT discover_domain(SaHpiDomainIdT, SaHpiSessionIdT, SaHpiRptEntryT);
const char * get_error_string(SaErrorT);
const char * get_sensor_type(SaHpiSensorTypeT type);
const char * get_sensor_category(SaHpiEventCategoryT category);
void list_rdr(SaHpiSessionIdT session_id, SaHpiResourceIdT resource_id);
void interpreted2str (SaHpiSensorInterpretedT interpreted);
void printreading (SaHpiSensorReadingT reading);
void printthresholds (SaHpiSensorThresholdsT thres);

/**
 * main: main program loop
 *
 *
 *
 * Return value: int 0
 **/
int main(int arc, const char *argv[])
{
        SaErrorT 		err;
        SaHpiVersionT		version;
        SaHpiSessionIdT 	session_id;
        SaHpiRptEntryT		entry;
        //SaHpiSelInfoT		Info;
        
	
	/* First step in HPI and openhpi */
        err = saHpiInitialize(&version);
        if (SA_OK != err) {
                error("saHpiInitialize", err);
                exit(-1);
        }
        
	/* Every domain requires a new session */
	/* This example is for one domain, one session */
        err = saHpiSessionOpen(SAHPI_DEFAULT_DOMAIN_ID, &session_id, NULL);
        if (SA_OK != err) {
                error("saHpiSessionOpen", err);
                return err;
        }
        
        err = discover_domain(SAHPI_DEFAULT_DOMAIN_ID, session_id, entry);
        if (SA_OK != err) {
                warn("an error was encountered, results may be incomplete");
        }

        err = saHpiFinalize();
        if (SA_OK != err) {
                error("saHpiFinalize", err);
                exit(-1);

	}
	return 0;
}

SaErrorT discover_domain(SaHpiDomainIdT domain_id, SaHpiSessionIdT session_id, SaHpiRptEntryT entry)
{
	
	SaErrorT	err;
	SaHpiRptInfoT	rpt_info_before;
	SaHpiRptInfoT   rpt_info_after;
	SaHpiEntryIdT	current;
	SaHpiEntryIdT   next;

        err = saHpiResourcesDiscover(session_id);
        if (SA_OK != err) {
                error("saHpiResourcesDiscover", err);
                return err;
        }
 	warn("list_resources: discover done");       
        /* grab copy of the update counter before traversing RPT */
        err = saHpiRptInfoGet(session_id, &rpt_info_before);
        if (SA_OK != err) {
                error("saHpiRptInfoGet", err);
                return err;
        }
        
        warn("Scanning RPT...");
        next = SAHPI_FIRST_ENTRY;
        do {
                current = next;
                err = saHpiRptEntryGet(session_id, current, &next, &entry);
                if (SA_OK != err) {
                        if (current != SAHPI_FIRST_ENTRY) {
                                error("saHpiRptEntryGet", err);
                                return err;
                        } else {
                                warn("Empty RPT\n");
                                break;
                        }
                }

		printf("***Records:\n");
                printf("%s\n", (char *)entry.ResourceTag.Data);
                printf("Entry ID: %d\n", (int) entry.EntryId);
                printf("Resource ID: %d\n", (int) entry.ResourceId);
                
                if (entry.ResourceCapabilities & SAHPI_CAPABILITY_RDR) 
                        list_rdr(session_id, entry.ResourceId);
                printf("\tEntryId: %d\n", next);
        } while (next != SAHPI_LAST_ENTRY);

	printf("SAHPI_LAST_ENTRY\n");
		
        /* wait for update in RPT */
        while (1) {
                err = saHpiRptInfoGet(session_id, &rpt_info_after);
                if (SA_OK != err) {
                        error("saHpiRptInfoGet", err);
                        return err;
                }
                if (rpt_info_before.UpdateCount != rpt_info_after.UpdateCount) {
                        rpt_info_before = rpt_info_after;
                } else
			break;
        };

        return SA_OK;
}

const char * get_error_string(SaErrorT error)
{
        switch(error) {
                case SA_ERR_HPI_ERROR:
                        return "SA_ERR_HPI_ERROR";
                case SA_ERR_HPI_UNSUPPORTED_API:
                        return "SA_ERR_UNSUPPORTED_API";
                case SA_ERR_HPI_BUSY:
                        return "SA_ERR_HPI_BUSY";
                case SA_ERR_HPI_INVALID:
                        return "SA_ERR_HPI_INVALID";
                case SA_ERR_HPI_INVALID_CMD:
                        return "SA_ERR_HPI_INVALID_CMD";
                case SA_ERR_HPI_TIMEOUT:
                        return "SA_ERR_HPI_TIMEOUT";
                case SA_ERR_HPI_OUT_OF_SPACE:
                        return "SA_ERR_HPI_OUT_OF_SPACE";
                case SA_ERR_HPI_DATA_TRUNCATED:
                        return "SA_ERR_HPI_DATA_TRUNCATED";
                case SA_ERR_HPI_DATA_LEN_INVALID:
                        return "SA_ERR_HPI_DATA_LEN_INVALID";
                case SA_ERR_HPI_DATA_EX_LIMITS:
                        return "SA_ERR_HPI_DATA_EX_LIMITS";
                case SA_ERR_HPI_INVALID_PARAMS:
                        return "SA_ERR_HPI_INVALID_PARAMS";
                case SA_ERR_HPI_INVALID_DATA:
                        return "SA_ERR_HPI_INVALID_DATA";
                case SA_ERR_HPI_NOT_PRESENT:
                        return "SA_ERR_HPI_NOT_PRESENT";
                case SA_ERR_HPI_INVALID_DATA_FIELD:
                        return "SA_ERR_HPI_INVALID_DATA_FIELD";
                case SA_ERR_HPI_INVALID_SENSOR_CMD:
                        return "SA_ERR_HPI_INVALID_SENSOR_CMD";
                case SA_ERR_HPI_NO_RESPONSE:
                        return "SA_ERR_HPI_NO_RESPONSE";
                case SA_ERR_HPI_DUPLICATE:
                        return "SA_ERR_HPI_DUPLICATE";
                case SA_ERR_HPI_UPDATING:
                        return "SA_ERR_HPI_UPDATING";
                case SA_ERR_HPI_INITIALIZING:
                        return "SA_ERR_HPI_INITIALIZING";
                case SA_ERR_HPI_UNKNOWN:
                        return "SA_ERR_HPI_UNKNOWN";
                case SA_ERR_HPI_INVALID_SESSION:
                        return "SA_ERR_HPI_INVALID_SESSION";
                case SA_ERR_HPI_INVALID_DOMAIN:
                        return "SA_ERR_HPI_INVALID_DOMAIN";
                case SA_ERR_HPI_INVALID_RESOURCE:
                        return "SA_ERR_HPI_INVALID_RESOURCE";
                case SA_ERR_HPI_INVALID_REQUEST:
                        return "SA_ERR_HPI_INVALID_REQUEST";
                case SA_ERR_HPI_ENTITY_NOT_PRESENT:
                        return "SA_ERR_HPI_ENTITY_NOT_PRESENT";
                case SA_ERR_HPI_UNINITIALIZED:
                        return "SA_ERR_HPI_UNINITIALIZED";
                default:
			printf("Invalid error code=%d\n", error);
                        return "(Invalid error code)";
        }
}

void list_rdr(SaHpiSessionIdT session_id, SaHpiResourceIdT resource_id)
{
        SaErrorT             	err;
        SaHpiEntryIdT        	current_rdr;
        SaHpiEntryIdT        	next_rdr;
        SaHpiRdrT            	rdr;

	SaHpiSensorReadingT	reading;
	SaHpiSensorTypeT	sensor_type;
	SaHpiSensorNumT		sensor_num;
	SaHpiEventCategoryT	category;
	SaHpiSensorThresholdsT	thres; 
	
	int i=0;
	static int maxtest = 6;
	static SaHpiSensorThresholdsT	thresset[] = {
		/* 0 - raw LowCritical and UpCritical readings */
		{
			.LowCritical = {
				.ValuesPresent = SAHPI_SRF_RAW,
				.Raw = 10000
			},
			.UpCritical = {
				.ValuesPresent = SAHPI_SRF_RAW,
				.Raw = 30000
			}
		},
		/* 1 - raw LowCritical, interpreted UpCritical */
		{
			.LowCritical = {
				.ValuesPresent = SAHPI_SRF_RAW,
				.Raw = 22000
			},
			.UpCritical = {
				.ValuesPresent = SAHPI_SRF_INTERPRETED,
				.Interpreted = {
					.Type = SAHPI_SENSOR_INTERPRETED_TYPE_UINT32,
					.Value.SensorUint32 = 50
				}
			}
		},
		/* 2 - no LowCritical, raw UpCritical */
		{
			.UpCritical = {
				.ValuesPresent = SAHPI_SRF_RAW,
				.Raw = 77000
			}
		},
		/* 3 - interpreted LowCritical, no UpCritical */
		{
			.LowCritical = {
				.ValuesPresent = SAHPI_SRF_INTERPRETED,
				.Interpreted = {
					.Type = SAHPI_SENSOR_INTERPRETED_TYPE_UINT32,
					.Value.SensorUint32 = 20
				}
			}
		},
		/* 4 - no LowCritical, no UpCritical */
		{
			.LowMajor = {
				.ValuesPresent = SAHPI_SRF_RAW,
				.Raw = 81000
			},
			.UpMajor = {
				.ValuesPresent = SAHPI_SRF_RAW,
				.Raw = 85000
			}
		},
		/* 5 - raw and interpreted LowCritical, raw and interpreted UpCritical */
		{
			.LowCritical = {
				.ValuesPresent = SAHPI_SRF_RAW | SAHPI_SRF_INTERPRETED,
				.Raw = 19000, /* don't make raw/interpreted match for easier validation */
				.Interpreted = {
					.Type = SAHPI_SENSOR_INTERPRETED_TYPE_UINT32,
					.Value.SensorUint32 = 20
				}
			},
			.UpCritical = {
				.ValuesPresent = SAHPI_SRF_RAW | SAHPI_SRF_INTERPRETED,
				.Raw = 55000, /* don't make raw/interpreted match for easier validation */
				.Interpreted = {
					.Type = SAHPI_SENSOR_INTERPRETED_TYPE_UINT32,
					.Value.SensorUint32 = 50
				}
			}
		}
	};

        printf("RDR Info:\n");
        next_rdr = SAHPI_FIRST_ENTRY;
        do {
                current_rdr = next_rdr;
                err = saHpiRdrGet(session_id, resource_id, current_rdr, 
                                &next_rdr, &rdr);
                if (SA_OK != err) {
                        if (current_rdr == SAHPI_FIRST_ENTRY)
                                printf("Empty RDR table\n");
                        else
                                error("saHpiRdrGet", err);
                        return;                        
                }
                
                printf("\tRecordId: %x\n", rdr.RecordId);
		
		if (rdr.RdrType == SAHPI_SENSOR_RDR)
		{			
			SaErrorT val;
			
			sensor_num = rdr.RdrTypeUnion.SensorRec.Num;
			
			val = saHpiSensorTypeGet(session_id, resource_id, 
						 sensor_num, &sensor_type, 
						 &category);
			
			printf("\tSensor num: %i\n\tType: %s\n", sensor_num, get_sensor_type(sensor_type)); 
			printf("\tCategory: %s\n", get_sensor_category(category)); 

			memset(&reading, 0, sizeof(SaHpiSensorReadingT));

			err = saHpiSensorReadingGet(session_id, resource_id, sensor_num, &reading);
			if (err != SA_OK) {
				printf("Error=%d reading sensor data {sensor, %d}\n", err, sensor_num);
				continue;
			}

			if (reading.ValuesPresent & SAHPI_SRF_RAW) {
				printf("\tValues Present: RAW\n");
				printf("\t\tRaw value: %d\n", reading.Raw);
			}

			if (reading.ValuesPresent & SAHPI_SRF_INTERPRETED) {
				printf("\tValues Present: Interpreted\n");
                                printf("\t\t");
				interpreted2str(reading.Interpreted);
			}

			if (reading.ValuesPresent & SAHPI_SRF_EVENT_STATE) {
				printf("\tValues Present: Event State\n");
			}

			if (rdr.RdrTypeUnion.SensorRec.ThresholdDefn.IsThreshold == SAHPI_TRUE) {
                                memset(&thres, 0, sizeof(SaHpiSensorThresholdsT));
				err = saHpiSensorThresholdsGet(session_id, resource_id, sensor_num, &thres);
				if (err != SA_OK) {
					printf("Error=%d reading sensor thresholds {sensor, %d}\n", err, sensor_num);
					continue;
				}
				
				printf("Threshold data received.\n");
				printthresholds(thres);
				printf("Threshold data to set to.\n");
				printthresholds(thresset[i]);
				err = saHpiSensorThresholdsSet(session_id, resource_id, 
						sensor_num, &thresset[i]);
				if (err != SA_OK) {
					printf("Error=%d setting sensor thresholds {sensor, %d}\n", err, sensor_num);
				}
				printf("Threshold data received.\n");
				err = saHpiSensorThresholdsGet(session_id, resource_id, sensor_num, &thres);
				if (err != SA_OK) {
					printf("Error=%d reading sensor thresholds {sensor, %d}\n", err, sensor_num);
					continue;
				}
				printthresholds(thres);
			}
		}
		i++;
		i = (i<maxtest) ? i : maxtest;
		
                printf("\n"); /* Produce blank line between rdrs. */
        }while(next_rdr != SAHPI_LAST_ENTRY);
}

/**
 * printthresholds:
 * @thres: structure structure with threshold data 
 *
 * Print threshold data
 **/
void printthresholds (SaHpiSensorThresholdsT thres)
{
	if (thres.LowCritical.ValuesPresent) {
		printf("\t\tThreshold:  Low Critical Values\n");
		printreading(thres.LowCritical);
	}
	if (thres.LowMajor.ValuesPresent) {
		printf("\t\tThreshold:  Low Major Values\n");
		printreading(thres.LowMajor);
	}
	if (thres.LowMinor.ValuesPresent) {
		printf("\t\tThreshold:  Low Minor Values\n");
		printreading(thres.LowMinor);
	}
	if (thres.UpCritical.ValuesPresent) {
		printf("\t\tThreshold:  Up Critical Values\n");
		printreading(thres.UpCritical);
	}
	if (thres.UpMajor.ValuesPresent) {
		printf("\t\tThreshold:  Up Major Values\n");
		printreading(thres.UpMajor);
	}
	if (thres.UpMinor.ValuesPresent) {
		printf("\t\tThreshold:  Up Minor Values\n");
		printreading(thres.UpMinor);
	}
	if (thres.PosThdHysteresis.ValuesPresent) {
		printf("\t\tThreshold:  Pos Threshold Hysteresis Values\n");
		printreading(thres.PosThdHysteresis);
	}
	if (thres.NegThdHysteresis.ValuesPresent) {
		printf("\t\tThreshold:  Neg Threshold Hysteresis Values\n");
		printreading(thres.NegThdHysteresis);
	}
}

/**
 * interpreted2str:
 * @interpreted: structure with interpreted data for sensor
 *
 * Print a string containing the interpreted data from the
 * interpreted data structure given.
 **/

void interpreted2str (SaHpiSensorInterpretedT interpreted)
{
        char *str = calloc(SAHPI_SENSOR_BUFFER_LENGTH ,sizeof(char));

        printf("Interpreted value: ");
                
	switch (interpreted.Type) {
		case SAHPI_SENSOR_INTERPRETED_TYPE_UINT8:
			snprintf(str,SAHPI_SENSOR_BUFFER_LENGTH,"%d",interpreted.Value.SensorUint8);
			break;
		case SAHPI_SENSOR_INTERPRETED_TYPE_UINT16:
			snprintf(str,SAHPI_SENSOR_BUFFER_LENGTH,"%d",interpreted.Value.SensorUint16);
			break;
		case SAHPI_SENSOR_INTERPRETED_TYPE_UINT32:
			snprintf(str,SAHPI_SENSOR_BUFFER_LENGTH,"%d",interpreted.Value.SensorUint32);
			break;
		case SAHPI_SENSOR_INTERPRETED_TYPE_INT8:
			snprintf(str,SAHPI_SENSOR_BUFFER_LENGTH,"%d",interpreted.Value.SensorInt8);
			break;
		case SAHPI_SENSOR_INTERPRETED_TYPE_INT16:
			snprintf(str,SAHPI_SENSOR_BUFFER_LENGTH,"%d",interpreted.Value.SensorInt16);
			break;
		case SAHPI_SENSOR_INTERPRETED_TYPE_INT32:
			snprintf(str,SAHPI_SENSOR_BUFFER_LENGTH,"%d",interpreted.Value.SensorInt32);
			break;
		case SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32:
			snprintf(str,SAHPI_SENSOR_BUFFER_LENGTH,"%6.2f",interpreted.Value.SensorFloat32);
			break;
		case SAHPI_SENSOR_INTERPRETED_TYPE_BUFFER:
			snprintf(str,SAHPI_SENSOR_BUFFER_LENGTH,"%s",interpreted.Value.SensorBuffer);
			break;
		default:
			printf("Invalid interpreted type=%d\n", interpreted.Type);
			break;
	}
        printf("%s\n",str);
        
	free(str);
}

const char * get_sensor_type(SaHpiSensorTypeT type) 
{
	switch(type) {
		case SAHPI_TEMPERATURE:
			return "TEMPERATURE SENSOR";
		case SAHPI_VOLTAGE:
			return "VOLTAGE SENSOR";
		case SAHPI_CURRENT:
			return "CURRENT SENSOR";
		case SAHPI_FAN:
			return "FAN SENSOR";
		case SAHPI_PHYSICAL_SECURITY:
			return "PHYSICAL SECURITY";
		case SAHPI_PLATFORM_VIOLATION:
			return "PLATFORM_VIOLATION";
		case SAHPI_PROCESSOR:
			return "PROCESSOR";
		case SAHPI_POWER_SUPPLY:
			return "POWER SUPPLY";
		case SAHPI_POWER_UNIT:
			return "POWER UNIT";
		case SAHPI_COOLING_DEVICE:
			return "COOLING DEVICE";
		case SAHPI_OTHER_UNITS_BASED_SENSOR:
			return "OTHER UNITS BASED SENSOR";
		case SAHPI_MEMORY:
			return "MEMORY";
    		case SAHPI_DRIVE_SLOT:
			return "SAHPI_DRIVE_SLOT"; 
    		case SAHPI_POST_MEMORY_RESIZE:
			return "SAHPI_POST_MEMORY_RESIZE"; 
    		case SAHPI_SYSTEM_FW_PROGRESS:
			return "SAHPI_SYSTEM_FW_PROGRESS"; 
    		case SAHPI_EVENT_LOGGING_DISABLED:
			return "SAHPI_EVENT_LOGGING_DISABLED"; 
    		case SAHPI_RESERVED1:
			return "SAHPI_RESERVED1"; 
    		case SAHPI_SYSTEM_EVENT:
			return "SAHPI_SYSTEM_EVENT"; 
    		case SAHPI_CRITICAL_INTERRUPT:
			return "SAHPI_CRITICAL_INTERRUPT"; 
    		case SAHPI_BUTTON:
			return "SAHPI_BUTTON"; 
    		case SAHPI_MODULE_BOARD:
			return "SAHPI_MODULE_BOARD"; 
    		case SAHPI_MICROCONTROLLER_COPROCESSOR:
			return "SAHPI_MICROCONTROLLER_COPROCESSOR"; 
    		case SAHPI_ADDIN_CARD:
			return "SAHPI_ADDIN_CARD"; 
    		case SAHPI_CHASSIS:
			return "SAHPI_CHASSIS"; 
    		case SAHPI_CHIP_SET:
			return "SAHPI_CHIP_SET"; 
    		case SAHPI_OTHER_FRU:
			return "SAHPI_OTHER_FRU"; 
    		case SAHPI_CABLE_INTERCONNECT:
			return "SAHPI_CABLE_INTERCONNECT"; 
    		case SAHPI_TERMINATOR:
			return "SAHPI_TERMINATOR"; 
    		case SAHPI_SYSTEM_BOOT_INITIATED:
			return "SAHPI_SYSTEM_BOOT_INITIATED"; 
    		case SAHPI_BOOT_ERROR:
			return "SAHPI_BOOT_ERROR"; 
    		case SAHPI_OS_BOOT:
			return "SAHPI_OS_BOOT"; 
    		case SAHPI_OS_CRITICAL_STOP:
			return "SAHPI_OS_CRITICAL_STOP"; 
    		case SAHPI_SLOT_CONNECTOR:
			return "SAHPI_SLOT_CONNECTOR"; 
    		case SAHPI_SYSTEM_ACPI_POWER_STATE:
			return "SAHPI_SYSTEM_ACPI_POWER_STATE"; 
    		case SAHPI_RESERVED2:
			return "SAHPI_RESERVED2"; 
    		case SAHPI_PLATFORM_ALERT:
			return "SAHPI_PLATFORM_ALERT"; 
    		case SAHPI_ENTITY_PRESENCE:
			return "SAHPI_ENTITY_PRESENCE"; 
    		case SAHPI_MONITOR_ASIC_IC:
			return "SAHPI_MONITOR_ASIC_IC"; 
		case SAHPI_LAN:
			return "SAHPI_LAN";
    		case SAHPI_MANAGEMENT_SUBSYSTEM_HEALTH:
			return "SAHPI_MANAGEMENT_SUBSYSTEM_HEALTH"; 
    		case SAHPI_BATTERY:
			return "SAHPI_BATTERY"; 
    		case SAHPI_OPERATIONAL:
			return "SAHPI_OPERATIONAL=0xA"; 
    		case SAHPI_OEM_SENSOR:
			return "SAHPI_OEM_SENSOR=0xC"; 
		default:
			printf("Invalid sensor type=%d\n", type);
			return "Invalid Sensor Type";
	}

return "\0";

}

const char * get_sensor_category (SaHpiEventCategoryT category)
{
	switch (category)
	{
		case SAHPI_EC_UNSPECIFIED:
			return "SAHPI_EC_UNSPECIFIED";
		case SAHPI_EC_THRESHOLD:
			return "SAHPI_EC_THRESHOLD"; 
		case SAHPI_EC_USAGE:
			return "SAHPI_EC_USAGE"; 
		case SAHPI_EC_STATE:
			return "SAHPI_EC_STATE"; 
		case SAHPI_EC_PRED_FAIL:
			return "SAHPI_EC_PRED_FAIL"; 
		case SAHPI_EC_LIMIT:
			return "SAHPI_EC_LIMIT";
		case SAHPI_EC_PERFORMANCE:
			return "SAHPI_EC_PERFORMANCE"; 
		case SAHPI_EC_SEVERITY:
		   	return "SAHPI_EC_SEVERITY"; 
		case SAHPI_EC_PRESENCE:
			return "SAHPI_EC_PRESENSE";
		case SAHPI_EC_ENABLE:
			return "SAHPI_EC_ENABLE";
		case SAHPI_EC_AVAILABILITY:
			return "SAHPI_EC_AVAILABILITY";
		case SAHPI_EC_REDUNDANCY:
			return "SAHPI_EC_REDUNDANCY";
		case SAHPI_EC_USER:
			return "SAHPI_EC_USER";
		case SAHPI_EC_GENERIC:
			return "SAHPI_EC_GENERIC";
                default:
			printf("Invalid sensor category=%d\n", category);
			return "Invalid Category";
	}

	return("\0");
}

/**
 * printreading:
 * @reading: structure with data to print
 *
 * Print out data from a sensor reading (used when
 * printing out threshold data).
 **/

void printreading (SaHpiSensorReadingT reading)
{
	if (reading.ValuesPresent & SAHPI_SRF_RAW) {
		printf("\t\t\tValues Present: RAW\n");
		printf("\t\t\t\tRaw value: %d\n", reading.Raw);
	}
	
	
	if (reading.ValuesPresent & SAHPI_SRF_INTERPRETED)
		printf("\t\t\tValues Present: Interpreted\n");

        printf("\t\t\t\t");
        interpreted2str(reading.Interpreted);

	if (reading.ValuesPresent & SAHPI_SRF_EVENT_STATE)
		printf("\t\t\tValues Present: Event State\n");
}



