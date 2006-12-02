/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
 *               2006 by IBM Corp.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Description: 
 *    This client application shows how two (2) openhpi plugins can be used to display  
 *    and manage resources of an IBM Blade with Basedboard Management Controller (BMC).
 * 
 *    Both the ipmi and snmp_bc plugin have the same IBM Blade target.  Resources from
 *    both plugins are combined to show a complete view of the IBM Blade.  
 *   
 * @@ WARNING @@ RESTRICTIONS @@ WARNING @@ RESTRICTIONS @@ WARNING @@ RESTRICTIONS @@
 * 
 *    - This client application is designed to run **only** on an IBM Blade with Basedboard
 *      Management Controller (BMC) 
 * 
 *    - This Openhpi and application run on (inband) an IBM Blade.  
 * 
 *    - Version 0.1 of the application only functions correctly in non-daemon Openhpi env
 *      Openhpi configure parms: ./configure --disabled-daemon --enable-ipmi --enable-snmp_bc
 * 
 *    - openhpi.conf file for this application should look similar to the following
 *                     plugin libipmi
 *
 *                     handler libipmi        {
 *                     entity_root = "{SYSTEM_CHASSIS,2}"
 *                     name = "smi"
 *                     addr = 0
 *                     }
 *
 *                     plugin  libsnmp_bc
 *
 *                     handler libsnmp_bc {
 *                     host = "bc.mm.ip.address"
 *                     version = "3"
 *                     community = "bc_community"
 *                     entity_root = "{SYSTEM_CHASSIS,1}"
 *                     security_name = "myid"
 *                     passphrase = "mypassword"
 *                     security_level = "authNoPriv"
 *                     auth_type = "MD5" 
 *                     }
 *
 *
 * @@ WARNING @@ RESTRICTIONS @@ WARNING @@ RESTRICTIONS @@ WARNING @@ RESTRICTIONS @@
 *
 * Authors:
 * 11/25/2004  kouzmich   Changed as new threshold client (first release)
 * Changes:
 * 10/13/2006  pdphan     Copy from kouzmich's hpithres.c to hpionIBMblade.c
 * 			  Add functions specifically for 
 *                          an IBM blade with Baseboard Management Controller (BMC)      
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#include <SaHpi.h>
#include <oh_utils.h>
#include <oh_config.h>

#define READ_BUF_SIZE	1024
#define MAX_BYTE_COUNT 128

typedef enum {
	UNKNOWN_TYPE = 0,
	LOWER_CRIT,
	LOWER_MAJOR,
	LOWER_MINOR,
	UPPER_CRIT,
	UPPER_MAJOR,
	UPPER_MINOR,
	POS_HYST,
	NEG_HYST
} ThresTypes_t;

typedef struct {
	int			rpt_ind;
	int			is_value;
	int			modify;
	SaHpiRdrT		Rdr;
	SaHpiSensorReadingT	reading;
	SaHpiSensorThresholdsT	thresholds;
} Rdr_t;

typedef struct {
	SaHpiRptEntryT	Rpt;
	int		nrdrs;
	Rdr_t		*rdrs;
} Rpt_t;

typedef enum {
	COM_UNDEF = 0,
	COM_EXIT,
	COM_HELP,
	COM_RPT,
	COM_RDR,
	COM_SENS,
	COM_MOD,
	COM_UNDO
} Com_enum_t;

typedef struct {
	char	*command_name;
	Com_enum_t	type;
} Commands_def_t;

Commands_def_t	Coms[] = {
	{ "quit",	COM_EXIT },
	{ "q",		COM_EXIT },
	{ "exit",	COM_EXIT },
	{ "help",	COM_HELP },
	{ "h",		COM_HELP },
	{ "rpt",	COM_RPT },
	{ "rdr",	COM_RDR },
	{ "sen",	COM_SENS },
	{ "mod",	COM_MOD },
	{ "undo",	COM_UNDO },
	{ NULL,		COM_UNDEF }
};

Rpt_t	*Rpts;
int	nrpts = 0;
long int blade_slot = 0;
char *root_tuple_ipmi;
char *root_tuple_snmp_bc;


int	fdebug = 0;
char	progver[] = "0.1";

SaHpiSessionIdT		sessionid;

static void *resize_array(void *Ar, int item_size, int *last_num, int add_num)
/* Resize array Ar:
 *	item_size - item size (bytes)
 *	last_num  - current array size
 *	add_num   - new array size = last_num + add_num
 * Return:	new pointer
 */
{
	void	*R;
	int	new_num = *last_num + add_num;

	if (new_num <= 0)
		return((void *)NULL);
	R = malloc(item_size * new_num);
	memset(R, 0, item_size * new_num);
	if (*last_num > 0) {
		memcpy(R, Ar, *last_num * item_size);
		free(Ar);
	}
	*last_num = new_num;
	return(R);
}

static void print_value(SaHpiSensorReadingT *item, char *mes)
{
	char *val;

	if (item->IsSupported != SAHPI_TRUE)
		return;
	switch (item->Type) {
		case SAHPI_SENSOR_READING_TYPE_INT64:
			printf("%lld %s\n", item->Value.SensorInt64, mes);
			return;
		case SAHPI_SENSOR_READING_TYPE_UINT64:
			printf("%llu %s\n", item->Value.SensorUint64, mes);
			return;
		case SAHPI_SENSOR_READING_TYPE_FLOAT64:
			printf("%10.3f %s\n", item->Value.SensorFloat64, mes);
			return;
		case SAHPI_SENSOR_READING_TYPE_BUFFER:
			val = (char *)(item->Value.SensorBuffer);
			if (val != NULL)
				printf("%s %s\n", val, mes);
			return;
	}
}

static void ShowThres(SaHpiSensorThresholdsT *sensbuff)
{
	printf("    Supported Thresholds:\n");
	print_value(&(sensbuff->LowCritical), "      Lower Critical Threshold(lc):");
	print_value(&(sensbuff->LowMajor), "      Lower Major Threshold(la):");
	print_value(&(sensbuff->LowMinor), "      Lower Minor Threshold(li):");
	print_value(&(sensbuff->UpCritical), "      Upper Critical Threshold(uc):");
	print_value(&(sensbuff->UpMajor), "      Upper Major Threshold(ua):");
	print_value(&(sensbuff->UpMinor), "      Upper Minor Threshold(ui):");
	print_value(&(sensbuff->PosThdHysteresis), "      Positive Threshold Hysteresis(ph):");
	print_value(&(sensbuff->NegThdHysteresis), "      Negative Threshold Hysteresis(nh):");
	printf("\n");
}

static void show_rpt(Rpt_t *Rpt, int ind)
{
	printf("%3d RPT: id = %3d  ResourceId = %3d  Tag = %s\n",
		ind, Rpt->Rpt.EntryId, Rpt->Rpt.ResourceId, Rpt->Rpt.ResourceTag.Data);
}

static int find_rpt_by_id(SaHpiEntryIdT id)
{
	int	i;

	for (i = 0; i < nrpts; i++)
		if (Rpts[i].Rpt.EntryId == id) return(i);
	return(-1);
}

static int select_ep(Rpt_t *Rpt)
{
	SaErrorT rv;
	unsigned int i;
	oh_big_textbuffer bigbuf1, bigbuf2;
	rv = oh_init_bigtext(&bigbuf1);
	if (rv) return(rv);
	rv = oh_init_bigtext(&bigbuf2);
	if (rv) return(rv);
	SaHpiEntityPathT ipmi_root_ep, snmp_bc_root_ep;
	SaHpiEntityPathT want_root_ep;
	
	rv = oh_encode_entitypath(root_tuple_ipmi, &ipmi_root_ep);
	rv = oh_encode_entitypath(root_tuple_snmp_bc, &snmp_bc_root_ep);
	oh_init_ep(&want_root_ep);	

        /* Find resource (rpt) that belongs to ipmi handler  */
        for (i=0; i<SAHPI_MAX_ENTITY_PATH; i++) {
                if (Rpt->Rpt.ResourceEntity.Entry[i].EntityType == SAHPI_ENT_SYSTEM_CHASSIS) {
                            break;
                }
        }

	want_root_ep.Entry[0].EntityType = Rpt->Rpt.ResourceEntity.Entry[i].EntityType;
	want_root_ep.Entry[0].EntityLocation = Rpt->Rpt.ResourceEntity.Entry[i].EntityLocation;
	if ( oh_cmp_ep(&ipmi_root_ep, &want_root_ep) == SAHPI_TRUE) {
			return 1;
	} else if ( oh_cmp_ep(&snmp_bc_root_ep, &want_root_ep) == SAHPI_TRUE) {

        	for (i=0; i<SAHPI_MAX_ENTITY_PATH; i++) {
                	if ((Rpt->Rpt.ResourceEntity.Entry[i].EntityType == SAHPI_ENT_PHYSICAL_SLOT) && (Rpt->Rpt.ResourceEntity.Entry[i].EntityLocation == blade_slot)) 
                            return(1);
                }
        }
	return 0;
	
}

static void show_rpts(char *S)
{
	int	i, rpt;

	i = sscanf(S, "%d", &rpt);
	if (i == 1) {
		i = find_rpt_by_id(rpt);
		if (i >= 0)
			oh_print_rptentry(&(Rpts[i].Rpt), 1);
		else
			printf("No RPT for id: %d\n", rpt);
		return;
	}

	for (i = 0; i < nrpts; i++) {
		if (select_ep(Rpts+i) == 1)
			show_rpt(Rpts + i, i);
	}
}

static int find_rdr_by_id(Rpt_t *R, SaHpiEntryIdT id)
{
	int	i;

	for (i = 0; i < R->nrdrs; i++)
		if (R->rdrs[i].Rdr.RecordId == id) return(i);
	return(-1);
}

static void show_rdr(Rdr_t *Rdr, int ind)
{
	
	printf("    %3d RDR: id = %3d  Data = %s\n",
		ind, Rdr->Rdr.RecordId, Rdr->Rdr.IdString.Data);
}

static void show_sen(Rdr_t *Rdr, int rptid)
{
	if (Rdr->Rdr.RdrType != SAHPI_SENSOR_RDR)
		return;
	printf("    RPT id = %3d RDR id = %3d  sensornum = %3d  Data = %s\n",
		rptid, Rdr->Rdr.RecordId, Rdr->Rdr.RdrTypeUnion.SensorRec.Num,
		Rdr->Rdr.IdString.Data);
}

static void show_sens_for_rpt(Rpt_t *R)
{
	int	j;

	for (j = 0; j < R->nrdrs; j++) {
		show_sen(R->rdrs + j, R->Rpt.EntryId);
	}
}

static void show_rdrs_for_rpt(Rpt_t *R)
{
	int	j;

	for (j = 0; j < R->nrdrs; j++) {
		show_rdr(R->rdrs + j, j);
	}
}

static void show_rdrs(char *S)
{
	int	i, j, rpt, rdr;
	Rpt_t	*R;

	i = sscanf(S, "%d %d", &rpt, &rdr);
	if (i == 1) {
		i = find_rpt_by_id(rpt);
		if (i >= 0) {
			show_rpt(Rpts + i, i);
			show_rdrs_for_rpt(Rpts + i);
		} else
			printf("No RPT for id: %d\n", rpt);
		return;
	}
	if (i == 2) {
		i = find_rpt_by_id(rpt);
		if (i >= 0) {
			j = find_rdr_by_id(Rpts + i, rdr);
			if (j >= 0)
				oh_print_rdr(&(Rpts[i].rdrs[j].Rdr), 2);
			else
				printf("No RDR %d for rpt %d\n", rdr, rpt);
		} else
			printf("No RPT for id: %d\n", rpt);
		return;
	}

	for (i = 0, R = Rpts; i < nrpts; i++, R++) {
		if (select_ep(R) == 1) show_rpt(R, i);
		if (select_ep(R) == 1) show_rdrs_for_rpt(R);
	}
}

static void show_reading_value(Rpt_t *Rpt, Rdr_t *R)
{
	SaHpiSensorUnitsT	k;
	SaErrorT		rv;
	char			unit[128];

	rv = saHpiSensorReadingGet(sessionid, Rpt->Rpt.ResourceId,
		R->Rdr.RdrTypeUnion.SensorRec.Num, &(R->reading), NULL);

	if (rv != SA_OK) {
		printf("ERROR: %s\n", oh_lookup_error(rv));
		return;
	};
	k = R->Rdr.RdrTypeUnion.SensorRec.DataFormat.BaseUnits;
	printf("Reading value: ");
	snprintf(unit, 128, "%s", oh_lookup_sensorunits(k));
	print_value(&(R->reading), unit);
}

static void show_thresholds(Rpt_t *Rpt, Rdr_t *R)
{
	SaHpiSensorThresholdsT	buf;
	SaErrorT		rv;

	rv = saHpiSensorThresholdsGet(sessionid, Rpt->Rpt.ResourceId,
		R->Rdr.RdrTypeUnion.SensorRec.Num, &buf);
	if (rv != SA_OK) {
		printf("ERROR: %s\n", oh_lookup_error(rv));
		return;
	};
	printf("  Thresholds:\n");
	ShowThres(&buf);
	if (R->is_value == 0) {
		R->thresholds = buf;
		R->is_value = 1;
	}
}

static int find_sensor_by_num(Rpt_t *R, int num)
{
	int	i;

	for (i = 0; i < R->nrdrs; i++) {
		if (R->rdrs[i].Rdr.RdrType == SAHPI_SENSOR_RDR) {
			if (R->rdrs[i].Rdr.RdrTypeUnion.SensorRec.Num == num)
				return(i);
		}
	};
	return(-1);
}

static void show_sens(char *S)
{
	int		i, j, rpt, num;
	Rpt_t		*Rpt;
	Rdr_t		*Rdr;

	i = sscanf(S, "%d %d", &rpt, &num);
	if (i == 1) {
		i = find_rpt_by_id(rpt);
		if (i >= 0)
			show_sens_for_rpt(Rpts + i);
		return;
	}
	if (i == 2) {
		i = find_rpt_by_id(rpt);
		if (i < 0) {
			printf("No RPT for id: %d\n", rpt);
			return;
		};
		j = find_sensor_by_num(Rpts + i, num);
		if (j < 0) {
			printf("No sensor %d for rpt %d\n", num, rpt);
			return;
		};
		oh_print_rdr(&(Rpts[i].rdrs[j].Rdr), 2);
		show_reading_value(Rpts + i, Rpts[i].rdrs + j);
		show_thresholds(Rpts + i, Rpts[i].rdrs + j);
		return;
	}

	for (i = 0, Rpt = Rpts; i < nrpts; i++, Rpt++) {
		if (select_ep(Rpt) == 1) {
			for (j = 0, Rdr = Rpt->rdrs; j < Rpt->nrdrs; Rdr++, j++)
				show_sen(Rdr, Rpt->Rpt.EntryId);
		} 
	}
}

static int get_number(char *mes, int *res)
{
	char	buf[READ_BUF_SIZE];

	printf("%s", mes);
	fgets(buf, READ_BUF_SIZE, stdin);
	return (sscanf(buf, "%d", res));
}

static void set_thres_value(SaHpiSensorReadingT *R, double val)
{
	if (R->IsSupported == 0) {
		printf("ERROR: this threshold isn't supported\n");
		return;
	};
	R->Value.SensorFloat64 = val;
}

static void mod_sen(void)
{
	int			i, rpt, rdr, num;
	char			buf[READ_BUF_SIZE], *S;
	Rpt_t			*Rpt;
	Rdr_t			*Rdr;
	SaHpiSensorThresholdsT	thres;
	SaErrorT		rv;
	ThresTypes_t		type = UNKNOWN_TYPE;
	float			f;
	SaHpiEventT		event;

	i = get_number("RPT number: ", &rpt);
	if (i != 1) {
		printf("ERROR: no RPT number\n");
		return;
	};
	i = find_rpt_by_id(rpt);
	if (i < 0) {
		printf("ERROR: invalid RPT number\n");
		return;
	};
	rpt = i;
	Rpt = Rpts + rpt;
	show_sens_for_rpt(Rpt);
	i = get_number("Sensor number: ", &num);
	if (i != 1) {
		printf("ERROR: no Sensor number\n");
		return;
	};
	rdr = find_sensor_by_num(Rpt, num);
	if (rdr < 0) {
		printf("ERROR: invalid sensor number\n");
		return;
	};
	Rdr = Rpt->rdrs + rdr;
	oh_print_rdr(&(Rdr->Rdr), 2);
	show_reading_value(Rpt, Rdr);
	rv = saHpiSensorThresholdsGet(sessionid, Rpt->Rpt.ResourceId,
		Rdr->Rdr.RdrTypeUnion.SensorRec.Num, &thres);
	if (rv != SA_OK) {
		printf("ERROR: %s\n", oh_lookup_error(rv));
		return;
	};
	printf("  Thresholds:\n");
	ShowThres(&thres);
	if (Rdr->is_value == 0) {
		Rdr->thresholds = thres;
		Rdr->is_value = 1;
	};
	printf("threshold type (lc, la, li, uc, ua, ui, ph, nh): ");
	fgets(buf, READ_BUF_SIZE, stdin);
	S = buf;
	while (*S == ' ') S++;
	if (strlen(S) < 2) {
		printf("ERROR: invalid threshold type: %s\n", S);
		return;
	};

	if (strncmp(S, "lc", 2) == 0) type = LOWER_CRIT;
	if (strncmp(S, "la", 2) == 0) type = LOWER_MAJOR;
	if (strncmp(S, "li", 2) == 0) type = LOWER_MINOR;
	if (strncmp(S, "uc", 2) == 0) type = UPPER_CRIT;
	if (strncmp(S, "ua", 2) == 0) type = UPPER_MAJOR;
	if (strncmp(S, "ui", 2) == 0) type = UPPER_MINOR;
	if (strncmp(S, "ph", 2) == 0) type = POS_HYST;
	if (strncmp(S, "nh", 2) == 0) type = NEG_HYST;

	if (type == UNKNOWN_TYPE) {
		printf("ERROR: unknown threshold type: %s\n", S);
		return;
	};
	
	printf("new value: ");
	fgets(buf, READ_BUF_SIZE, stdin);
	i = sscanf(buf, "%f", &f);
	if (i == 0) {
		printf("ERROR: no value\n");
		return;
	};
	switch (type) {
		case LOWER_CRIT:
			set_thres_value(&(thres.LowCritical), (double)f);
			break;
		case LOWER_MAJOR:
			set_thres_value(&(thres.LowMajor), (double)f);
			break;
		case LOWER_MINOR:
			set_thres_value(&(thres.LowMinor), (double)f);
			break;
		case UPPER_CRIT:
			set_thres_value(&(thres.UpCritical), (double)f);
			break;
		case UPPER_MAJOR:
			set_thres_value(&(thres.UpMajor), (double)f);
			break;
		case UPPER_MINOR:
			set_thres_value(&(thres.UpMinor), (double)f);
			break;
		case POS_HYST:
			set_thres_value(&(thres.PosThdHysteresis), (double)f);
			break;
		case NEG_HYST:
			set_thres_value(&(thres.NegThdHysteresis), (double)f);
			break;
		default:
			printf("ERROR: unknown threshold\n");
	};
	printf("\n  Nem threshold:\n");
	ShowThres(&thres);
	printf("Is it correct (yes, no)?:");
	fgets(buf, READ_BUF_SIZE, stdin);
	S = buf;
	while (*S == ' ') S++;
	if (strncmp(S, "yes", 3) != 0)
		return;
	rv = saHpiSensorThresholdsSet(sessionid, Rpt->Rpt.ResourceId,
		Rdr->Rdr.RdrTypeUnion.SensorRec.Num, &thres);
	if (rv != SA_OK) {
		printf("ERROR: saHpiSensorThresholdsSet: %s\n", oh_lookup_error(rv));
		return;
	};
	Rdr->modify = 1;
	for (i = 0; i < 10; i++) {
		rv = saHpiEventGet(sessionid, SAHPI_TIMEOUT_IMMEDIATE, &event,
			NULL, NULL, NULL);
		if (rv == SA_OK)
			break;
		if (fdebug) printf("sleep before saHpiEventGet\n");
		sleep(1);
	};
	saHpiSensorThresholdsGet(sessionid, Rpt->Rpt.ResourceId,
		Rdr->Rdr.RdrTypeUnion.SensorRec.Num, &thres);
	printf("Current thresholds:\n");
	ShowThres(&thres);
}

static void undo(void)
{
	int			i, j;
	Rpt_t			*Rpt;
	Rdr_t			*Rdr;

	for (i = 0, Rpt = Rpts; i < nrpts; i++, Rpt++) {
		for (j = 0, Rdr = Rpt->rdrs; j < Rpt->nrdrs; j++, Rdr++) {
			if (Rdr->modify == 0) continue;
			saHpiSensorThresholdsSet(sessionid, Rpt->Rpt.ResourceId,
				Rdr->Rdr.RdrTypeUnion.SensorRec.Num, &(Rdr->thresholds));
			Rdr->modify = 0;
		}
	}
}

static void get_rpts(void)
{
	SaHpiEntryIdT	rptentryid, nextrptentryid;
	SaErrorT	rv;
	SaHpiRptEntryT	rptentry;
	int		n;

	rptentryid = SAHPI_FIRST_ENTRY;
	while (rptentryid != SAHPI_LAST_ENTRY) {
		rv = saHpiRptEntryGet(sessionid, rptentryid, &nextrptentryid, &rptentry);
		if (rv != SA_OK)
			break;
		n = nrpts;
		Rpts = (Rpt_t *)resize_array(Rpts, sizeof(Rpt_t), &nrpts, 1);
		rptentry.ResourceTag.Data[rptentry.ResourceTag.DataLength] = 0;
		Rpts[n].Rpt =  rptentry;
		rptentryid = nextrptentryid;
	}
}

static void get_rdrs(Rpt_t *Rpt)
{
	SaHpiRdrT		rdr;
	SaErrorT		rv;
	int			n;
	SaHpiEntryIdT		entryid;
	SaHpiEntryIdT		nextentryid;
	SaHpiResourceIdT	resourceid;

	entryid = SAHPI_FIRST_ENTRY;
	while (entryid !=SAHPI_LAST_ENTRY) {
		resourceid = Rpt->Rpt.ResourceId;
		rv = saHpiRdrGet(sessionid, resourceid, entryid, &nextentryid, &rdr);
		if (rv != SA_OK)
			break;
		n = Rpt->nrdrs;
		Rpt->rdrs = (Rdr_t *)resize_array(Rpt->rdrs, sizeof(Rdr_t), &(Rpt->nrdrs), 1);
		rdr.IdString.Data[rdr.IdString.DataLength] = 0;
		Rpt->rdrs[n].Rdr = rdr;
		entryid = nextentryid;
	}
}

static void help(void)
{
	printf("Available commands:\n");
	printf("	exit, quit, q		- exit\n");
	printf("	help, h			- this instruction\n");
	printf("	rpt			- show all RPT entries\n");
	printf("	rpt <id>		- show #id RPT entry\n");
	printf("	rdr			- show all RDR entries\n");
	printf("	rdr <rptid>		- show RDRs entries for #rptid\n");
	printf("	rdr <rptid> <rdrid>	- show #rdrid RDR entry for #rptid\n");
	printf("	sen			- show all sensors\n");
	printf("	sen <rptid>		- show sensors for #rptid\n");
	printf("	sen <rptid> <num>	- show #num sensor for #rptid\n");
	printf("	mod			- modify thresholds\n");
	printf("	undo			- delete all changes\n");
}

static Com_enum_t find_command(char *S)
{
	int	i;

	for (i = 0;; i++) {
		if (Coms[i].command_name == (char *)NULL)
			return(COM_UNDEF);
		if (strcmp(Coms[i].command_name, S) == 0)
			return(Coms[i].type);
	}
}

static int parse_command(char *Str)
{
	int		len;
	char		*S, *S1;
	Com_enum_t	com;

	S = Str;
	while (*S != 0) {
		if ((*S == '\t') || (*S == '\n')) *S = ' ';
		S++;
	};

	S = Str;
	while (*S == ' ') S++;
	len = strlen(S);
	if (len == 0) return(0);

	S1 = S;
	while ((*S1 != 0) && (*S1 != ' ')) S1++;
	if (*S1 != 0) 
		*S1++ = 0;
	
	com = find_command(S);
	switch (com) {
		case COM_UNDEF:
			printf("Invalid command: %s\n", S);
			help();
			break;
		case COM_EXIT:	return(-1);
		case COM_RPT:	show_rpts(S1); break;
		case COM_RDR:	show_rdrs(S1); break;
		case COM_SENS:	show_sens(S1); break;
		case COM_MOD:	mod_sen(); break;
		case COM_UNDO:	undo(); break;
		case COM_HELP:	help(); break;
	};
	return(0);
}

static SaErrorT hpiIBMspecial_check_environment(void) 
{

        SaErrorT        rv;
        char *config_file = NULL;
        GSList *node = NULL;
	char *plugin_name;	
        struct oh_parsed_config config = {NULL, NULL, 0, 0, 0, 0};	

	/* Load in config file */
        if ((config_file = getenv("OPENHPI_CONF")) == NULL) {
		config_file = "/etc/openhpi/openhpi.conf";
	}

        rv = oh_load_config(config_file, &config);
        if (rv != SA_OK) {
		printf("Can not load OPENHPI_CONF file.\n");
		return(SA_ERR_HPI_ERROR);
	}

        /* Search handler cofigureation for the 2 root tupples */
        for (node = config.handler_configs; node; node = node->next) {
                GHashTable *handler_config = (GHashTable *)node->data;
                plugin_name = (char *)g_hash_table_lookup(handler_config, "plugin");
		if (strcmp(plugin_name, "libipmi") == 0) {
        		root_tuple_ipmi = (char *)g_hash_table_lookup(handler_config, "entity_root");
        		if (!root_tuple_ipmi) {
                		printf("Cannot find \"entity_root\" for ipmi plugin.");
                		return SA_ERR_HPI_ERROR;
			}
		} else if(strcmp(plugin_name, "libsnmp_bc") == 0) {
        		root_tuple_snmp_bc = (char *)g_hash_table_lookup(handler_config, "entity_root");
        		if (!root_tuple_snmp_bc) {
                		printf("Cannot find \"entity_root\" for snmp_bc plugin.");
                		return(SA_ERR_HPI_ERROR);
			}		
		}
		
                config.handlers_defined++;
        }
	
	if (config.handlers_defined != 2) {
		printf("Configuration error: we do not have exactly 2 handlers defined.\n");
		return(SA_ERR_HPI_ERROR);
	}			
	return(SA_OK);	
}
	
static SaErrorT hpiIBMspecial_find_blade_slot(void)
{

        char shell_command[MAX_BYTE_COUNT];
        char VPD_DATA[MAX_BYTE_COUNT];
        char str[2];
        int sys_rv = 0;
        FILE *Fp = NULL;

        unsigned long int CH_SLOT = 0, 
			  OFFSET = 0, 
			  OFFSET_LSB = 0,
			  OFFSET_MSB = 0;

        int  byte_count;
        char BYTE_READ;
        /*
        printf("Find offset to VPD Block 2.\n");
        */
        snprintf(shell_command, MAX_BYTE_COUNT,
                "ipmitool raw 0x2e 0x0a 0x02 0x00 0x00 0x07 0x09 0x10 0x01 > /tmp/hpiS01\n");
        sys_rv = system(shell_command);
        if (sys_rv != 0) {
                printf("\"Finding offset to VPD Block 2\" has FAILED.\n");
                return(-1);
        }


        Fp  = fopen("/tmp/hpiS01", "r");
        byte_count = 0;
        while(fscanf(Fp, "%c", &BYTE_READ) != -1){
                if (BYTE_READ != 0x20)
                        VPD_DATA[byte_count++]=BYTE_READ;
        }
        fclose(Fp);

        memcpy(&str, &VPD_DATA[6], 2);
        if (byte_count >= 3) OFFSET = strtol(str,NULL, 16);

        OFFSET = OFFSET * 0x1A;
        OFFSET = OFFSET + 0x0908;
        /*
        OFFSET_LSB=$(($OFFSET&0x00FF))
        OFFSET_MSB=$(($OFFSET&0xFF00))
        OFFSET_MSB=$(($OFFSET_MSB>>8))
        */
        OFFSET_LSB = OFFSET&0x00FF;
        OFFSET_MSB = OFFSET&0xFF00;
        OFFSET_MSB = OFFSET_MSB >> 8;

        //X=`ipmitool raw 0x2e 0x0a 0x02 0x00 0x00 $OFFSET_LSB $OFFSET_MSB 0x10 0x1A`
        snprintf(shell_command, MAX_BYTE_COUNT,
               "ipmitool raw 0x2e 0x0a 0x02 0x00 0x00 0x%x 0x%x 0x10 0x1A > /tmp/hpiS02\n",
                 (unsigned int)OFFSET_LSB, (unsigned int)OFFSET_MSB);
        sys_rv = system(shell_command);
        if (sys_rv != 0) {
                printf("\"Finding offset to VPD Block 2\" has FAILED.\n");
                return(-1);
        }

        Fp  = fopen("/tmp/hpiS02", "r");
        byte_count = 0;
        while(fscanf(Fp, "%c", &BYTE_READ) != -1){
                if (BYTE_READ != 0x20)
                        VPD_DATA[byte_count++]=BYTE_READ;
        }
        fclose(Fp);

        memcpy(&str, &VPD_DATA[39], 2);
        if (byte_count >= 3) CH_SLOT= strtol(str,NULL, 16);

        /*
          CH_SLOT=`echo $X | gawk '{i=20; x=x $i; print x}'`
        */
        printf("Blade is installed in slot number %d.\n", (int)CH_SLOT);
	blade_slot = CH_SLOT;
	return(SA_OK);
}

								
int main(int argc, char **argv)
{
	int		c, i;
	SaErrorT	rv;
	char		buf[READ_BUF_SIZE];
	char		*S;

	printf("%s  ver %s\n", argv[0], progver);
	while ( (c = getopt( argc, argv,"x?")) != EOF )
		switch(c)  {
			case 'x':
				fdebug = 1;
				break;
			default:
				printf("Usage: %s [-x]\n", argv[0]);
				printf("   -x  Display debug messages\n");
				return(1);
		}

	rv = hpiIBMspecial_check_environment();
	if (rv != SA_OK) {
		printf("%s is not invoked in its special setup environment\n", argv[0]);
		return(-1);
	}

	rv = hpiIBMspecial_find_blade_slot();
	if (rv != SA_OK) {
		printf("ipmitool can not find slot number in which this blade resides.\n");
		return(-1);
	}

	rv = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID, &sessionid, NULL);

	if (rv != SA_OK) {
		printf("saHpiSessionOpen: %s\n", oh_lookup_error(rv));
		return(-1);
	}
 
	rv = saHpiDiscover(sessionid);

	if (fdebug) printf("saHpiDiscover: %s\n", oh_lookup_error(rv));

	rv = saHpiSubscribe(sessionid);
	if (rv != SA_OK) {
		printf( "saHpiSubscribe error %d\n",rv);
		return(-1);
	}	
	
	/* make the RPT list */
	get_rpts();
	/* get rdrs for the RPT list */
	for (i = 0; i < nrpts; i++)
		get_rdrs(Rpts + i);
			
	help();

	for (;;) {
		printf("==> ");
		S = fgets(buf, READ_BUF_SIZE, stdin);
		if (parse_command(S) < 0) break;
	};
	rv = saHpiSessionClose(sessionid);
	return(0);
}
 /* end hpthres.c */
