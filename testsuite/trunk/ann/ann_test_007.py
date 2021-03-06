#!/usr/bin/env python

"""
 (C) Copyright IBM Corp. 2008
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 file and program are licensed under a BSD style license.  See
 the Copying file included with the OpenHPI distribution for
 full licensing terms.
 
 Authors:
    Suntrupth S Yadav <suntrupth@in.ibm.com>
"""

import unittest
from openhpi import *

class TestSequence(unittest.TestCase):

    """
    main: Announcement test
 
    This test adds one announcement to the list
 
    Return value: 0 on success, 1 on failure
    """

    def runTest(self):
            announ=SaHpiAnnouncementT( # <-- Constructor
                EntryId=0, # <-- keyword argument example
                Timestamp=0,
                AddedByUser=SAHPI_FALSE,
                Severity=SAHPI_CRITICAL,
                Acknowledged=SAHPI_FALSE,
                StatusCond=SaHpiConditionT(
                        Type=SAHPI_STATUS_COND_TYPE_SENSOR,
                        Entity=SaHpiEntityPathT(
                                Entry=[
                                        SaHpiEntityT(EntityType=SAHPI_ENT_SYSTEM_BOARD, EntityLocation=1),
                                        SaHpiEntityT(EntityType=SAHPI_ENT_ROOT, EntityLocation=0)
                                ]
                        ),
                        DomainId=1,
                        ResourceId=1,
                        SensorNum=1,
                        EventState=SAHPI_ES_UNSPECIFIED,
                        Name=SaHpiNameT(Length=6, Value='announ'), # Length equals the length of Value.
                        Mid=123
                )
               
        )             # we will not worry about the Data field for this test 

            ann = oh_announcement_create()

            rc = oh_announcement_append(ann, announ)

            announ.Severity = SAHPI_MAJOR
            rc = oh_announcement_append(ann, announ)

            announ.Severity = SAHPI_MINOR
            rc = oh_announcement_append(ann, announ)

            rc = oh_announcement_del(ann, 2, SAHPI_MAJOR)
            self.assertEqual(rc != SA_OK,False)
                

            i = 0
	    a = SaHpiAnnouncementT(EntryId=SAHPI_FIRST_ENTRY)
	    error = oh_announcement_get_next(ann, SAHPI_ALL_SEVERITIES, SAHPI_FALSE, a)
	    while error == SA_OK:
                i += 1
                error = oh_announcement_get_next(ann, SAHPI_ALL_SEVERITIES, SAHPI_FALSE, a)

            self.assertEqual(i != 2,False)
                
if __name__=='__main__':
    unittest.main()
