/* -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2005
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author: Renier Morales <renier@openhpi.org>
 */

#include <SaHpi.h>
#include <oh_utils.h>

/*******************************************************************
 * WARNING! This file is auto-magically generated by:
 *          ./gen_epath_pattern_tests.py
 *          Do not change this file manually. Update script instead
 *******************************************************************/

/**
 * This takes an entity path and an entity path's pattern,
 * and knowing the proper result beforehand, checks if the
 * pattern matches the entity path. If the proper result is
 * achieved, the test passes.
 **/
int main(int argc, char **argv)
{

        char *ep_str = "{SBC_BLADE,3}";
        char *epp_str = "*{SYSTEM_CHASSIS,1}{SBC_BLADE,.}*";
        oh_entitypath_pattern epp;
        SaHpiEntityPathT ep;
        SaErrorT error = SA_OK;
        SaHpiBoolT match = SAHPI_FALSE;

        error = oh_encode_entitypath(ep_str, &ep);
        if (error) { printf("Encoding of entitypath failed.\n"); return -1; }

        error = oh_compile_entitypath_pattern(epp_str, &epp);
        if (error) { printf("Compilation of pattern failed.\n"); return -1; }

        if (oh_match_entitypath_pattern(&epp, &ep) != match)
                return -1;
        

        return 0;
}

