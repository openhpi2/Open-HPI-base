/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Tariq Shureih <tariq.shureih@intel.com>
 *     Louis Zhuang <louis.zhuang@linux.intel.com>
 */

#include "ipmi.h"
#include <oh_utils.h>
#include <string.h>

//static void add_control_event_data_format(ipmi_control_t	*control,
					   //SaHpiCtrlRecT	*rec)
//{


//}

static SaHpiCtrlOutputTypeT _control_type_from_ipmi_to_hpi(int ipmi_type)
{
        switch (ipmi_type) {
		case IPMI_CONTROL_ALARM:
                        return SAHPI_CTRL_FRONT_PANEL_LOCKOUT;
                        
		case IPMI_CONTROL_DISPLAY:
                        return SAHPI_CTRL_LCD_DISPLAY;

                case IPMI_CONTROL_LIGHT:
                        return SAHPI_CTRL_LED;
                        
		case IPMI_CONTROL_FAN_SPEED:
                        return SAHPI_CTRL_FAN_SPEED;

                case IPMI_CONTROL_IDENTIFIER:
		case IPMI_CONTROL_RELAY:
                default:
                        return SAHPI_CTRL_OEM;                       
        } 
}

static void add_control_event_control_rec(ipmi_control_t	*control,
					SaHpiCtrlRecT	*rec)
{
        static int control_num = 0;
	int	control_type;
	rec->Num        = ++control_num;
	control_type    = ipmi_control_get_type(control);
        rec->OutputType = _control_type_from_ipmi_to_hpi(control_type);
        rec->Type       = SAHPI_CTRL_TYPE_OEM; 
		
	//rec->Ignore     = (SaHpiBoolT)ipmi_control_get_ignore_if_no_entity(control);

	//add_control_event_data_format(control, rec);

	/* We do not care about oem. */
	rec->Oem = 0;
}

static void add_control_event_rdr(ipmi_control_t		*control, 
				 SaHpiRdrT		*rdr,
				 SaHpiEntityPathT	parent_ep,
				 SaHpiResourceIdT	res_id)
{
	char	name[32];
	//SaHpiEntityPathT rdr_ep;

	memset(name,'\0',32);
	rdr->RecordId = 0;
	rdr->RdrType = SAHPI_CTRL_RDR;
	//rdr->Entity.Entry[0].EntityType = (SaHpiEntityTypeT)id;
	//rdr->Entity.Entry[0].EntityLocation = (SaHpiEntityLocationT)instance;
	//rdr->Entity.Entry[1].EntityType = 0;
	//rdr->Entity.Entry[1].EntityLocation = 0;
	rdr->Entity = parent_ep;

	/* append ep */
	//oh_encode_entitypath(entity_root, &rdr_ep);

	//oh_concat_ep (&rdr->Entity, &rdr_ep);

	add_control_event_control_rec(control, &rdr->RdrTypeUnion.CtrlRec);

	ipmi_control_get_id(control, name, 32);
	rdr->IdString.DataType = SAHPI_TL_TYPE_ASCII6;
	rdr->IdString.Language = SAHPI_LANG_ENGLISH;
	rdr->IdString.DataLength = 32;

	memcpy(rdr->IdString.Data,name, strlen(name) + 1);
}

static void add_control_event(ipmi_entity_t	*ent,
			     ipmi_control_t	*control,
			     struct oh_handler_state *handler,
			     SaHpiEntityPathT	parent_ep,
			     SaHpiResourceIdT	rid)
{
        ipmi_control_id_t         *control_id; 
	struct oh_event           *e;
        struct ohoi_resource_info *info;

        control_id = malloc(sizeof(*control_id));
        if (!control_id) {
                dbg("Out of memory");
                return;
        }
        *control_id = ipmi_control_convert_to_id(control);
        
	e = malloc(sizeof(*e));
	if (!e) {
                free(control_id);
		dbg("Out of space");   
		return;
	}
	memset(e, 0, sizeof(*e));

	e->type = OH_ET_RDR;
	add_control_event_rdr(control, &e->u.rdr_event.rdr, parent_ep, rid);	

	info = oh_get_resource_data(handler->rptcache, rid);
        if (!info) {
                free(e);
                dbg("No info in resource(%d)\n", rid);
                return;
        }
        e->u.rdr_event.rdr.RdrTypeUnion.SensorRec.Num = info->ctrl_count;
        info->ctrl_count++;

        rid = oh_uid_lookup(&e->u.rdr_event.rdr.Entity);
        
	oh_add_rdr(handler->rptcache, rid, &e->u.rdr_event.rdr, control_id, 1);
}

/*
 * add_alarm_rdr
 */
static void add_alarm_rdr(
                char			*name,
                int			num,
		SaHpiResourceIdT   	rptid,
                SaHpiEntityPathT        parent_ent,
		ipmi_control_id_t   	*control_id,
		struct oh_handler_state *handler)
{
	SaHpiRdrT               rdr_temp;
	SaHpiRdrT               *rdr;
 
	rdr = &rdr_temp;
        rdr->RecordId = 0;
        rdr->RdrType = SAHPI_CTRL_RDR;
        rdr->Entity = parent_ent;
 
        rdr->IdString.DataType = SAHPI_TL_TYPE_ASCII6;
        rdr->IdString.Language = SAHPI_LANG_ENGLISH;
        rdr->IdString.DataLength = strlen(name);
        memcpy(rdr->IdString.Data, name, strlen(name));

        rdr->RdrTypeUnion.CtrlRec.Num   = num;
        rdr->RdrTypeUnion.CtrlRec.Type         = SAHPI_CTRL_TYPE_DIGITAL;
        rdr->RdrTypeUnion.CtrlRec.OutputType   = SAHPI_CTRL_LED; 
        rdr->RdrTypeUnion.CtrlRec.Oem          = OEM_ALARM_BASE + num;

        oh_add_rdr(handler->rptcache, rptid, rdr, control_id, 1);
	dbg("add_alarm_rdr: %s\n",name); 
}

/*
 * add_alarm_rdrs
 */
static int add_alarm_rdrs(
		struct oh_handler_state *handler,
		SaHpiRptEntryT *rpt,
		ipmi_control_t     *control)
{
	SaHpiResourceIdT   	rid;
        SaHpiEntityPathT        ent;
	static ipmi_control_id_t   alarm_control_id;  /*save this */
	static int alarms_done = 0;

	if (alarms_done) return 0;  /* only do alarms the first time */
	rid = rpt->ResourceId;
	ent = rpt->ResourceEntity;

	alarm_control_id = ipmi_control_convert_to_id(control);

	rpt->ResourceCapabilities |=  SAHPI_CAPABILITY_RDR;
	rpt->ResourceCapabilities |=  SAHPI_CAPABILITY_CONTROL;

	add_alarm_rdr("Power Alarm LED",   0,rid,ent,&alarm_control_id,handler);
	add_alarm_rdr("Critical Alarm LED",1,rid,ent,&alarm_control_id,handler);
	add_alarm_rdr("Major Alarm LED",   2,rid,ent,&alarm_control_id,handler);
	add_alarm_rdr("Minor Alarm LED",   3,rid,ent,&alarm_control_id,handler);
	alarms_done = 1;
	return 0;
}

void ohoi_control_event(enum ipmi_update_e op,
		        ipmi_entity_t      *ent,
			ipmi_control_t     *control,
			void               *cb_data)
{
	struct oh_handler_state *handler = cb_data;
        struct ohoi_resource_info *ohoi_res_info;
	
	ipmi_entity_id_t	entity_id;	
	SaHpiRptEntryT		*rpt_entry;
	int rv;
        
	entity_id = ipmi_entity_convert_to_id(ent);
	rpt_entry = ohoi_get_resource_by_entityid(handler->rptcache,
						  &entity_id);   
	
	if (!rpt_entry) {
			dump_entity_id("Control with RPT Entry?!", entity_id);
			return;
	}

	ohoi_res_info = oh_get_resource_data(handler->rptcache,
                                             rpt_entry->ResourceId);
        
	if (op == IPMI_ADDED) {
                int ctrl_type;
                
		/* attach power and reset to chassis entity since
		   IPMI provides them as such */
		dbg("resource: %s", rpt_entry->ResourceTag.Data);
                ctrl_type = ipmi_control_get_type(control);
                switch (ctrl_type) {
                        case IPMI_CONTROL_ONE_SHOT_RESET:
                                dbg("Attach reset control into entity");
                                ohoi_res_info->reset_ctrl
                                        = ipmi_control_convert_to_id(control);
                                rpt_entry->ResourceCapabilities |=
                                    SAHPI_CAPABILITY_RESET;
                                break;
                        case IPMI_CONTROL_POWER:
                                dbg("Attach power control into entity");
                                ohoi_res_info->power_ctrl
                                        = ipmi_control_convert_to_id(control);
                                rpt_entry->ResourceCapabilities |=
                                    SAHPI_CAPABILITY_POWER;
                                break;
			case IPMI_CONTROL_ALARM:
				rv = add_alarm_rdrs(handler,rpt_entry,control);
				break;
                        default:
                                dbg("Other control(%d) is storaged in RDR", ctrl_type);
                                rpt_entry->ResourceCapabilities |= SAHPI_CAPABILITY_CONTROL;
                                add_control_event(ent, control, handler,
                                                  rpt_entry->ResourceEntity,
                                                  rpt_entry->ResourceId);
                }
                                
	}
	dbg("Set updated for res_inf0 %p(%d). Control", ohoi_res_info, rpt_entry->ResourceId);
	entity_rpt_set_updated(ohoi_res_info, handler->data);;
}
	
