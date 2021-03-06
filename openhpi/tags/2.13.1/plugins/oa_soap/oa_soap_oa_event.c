/*
 * Copyright (C) 2007-2008, Hewlett-Packard Development Company, LLP
 *                     All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the Hewlett-Packard Corporation, nor the names
 * of its contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Author(s)
 *      Raghavendra P.G. <raghavendra.pg@hp.com>
 *
 * This file has the OA related events handling
 *
 *      process_oa_insertion_event()    - Processes the OA insertion event
 *
 *      process_oa_extraction_event()   - Processes the OA extraction event
 *
 *      process_oa_failover_event()     - Processes the OA failover event
 *                                        points the active_con filed of
 *                                        oa_handler to currently active OA
 *                                        Starts the re-discovery to sync up
 *                                        with current status of the system
 *
 *      process_oa_info_event()         - Processes the OA info event
 *                                        If OA info event is just after OA
 *                                        insertion event, then it processed.
 *                                        Else, it is ignored
 *
 *      add_oa_inv_area()               - Adds the OA inventory area for newly
 *                                        inserted OA.  This function is called
 *                                        while processing the OA info event.
 *
 *      build_inserted_oa_rdr()         - Builds the RDRs for the newly
 *                                        inserted OA
 *
 *      build_inserted_oa_inv_rdr()     - Builds the bare minimum inventory
 *                                        RDR for newly inserted OA
 */

#include "oa_soap_oa_event.h"

/**
 * process_oa_extraction_event
 *      @oh_handler: Pointer to openhpi handler structure
 *      @oa_event:   Pointer to oa event response structure
 *
 * Purpose:
 *      Gets the OA extraction event.
 *      Removes OA information from RPT and RDR table
 *      Creates the hot swap event
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure
 **/
SaErrorT process_oa_extraction_event(struct oh_handler_state *oh_handler,
                                     struct eventInfo *oa_event)
{
        SaErrorT rv = SA_OK;
        SaHpiInt32T bay_number;

        if (oh_handler == NULL || oa_event == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* The OA is sending the wrong bay_number for the removed OA
         * Hence if the bay number in the oa_event is 1,
         * process the event for bay number 2, or vice versa
         * TODO: Remove this workaround when the fix is made to OA firmware
         */
        switch (oa_event->eventData.oaStatus.bayNumber) {
                case 1:
                        bay_number = 2;
                        break;
                case 2:
                        bay_number = 1;
                        break;
                default:
                        err("Wrong OA bay number %d detected",
                             oa_event->eventData.oaStatus.bayNumber);
                        return SA_ERR_HPI_INTERNAL_ERROR;
        }

        rv = remove_oa(oh_handler, bay_number);
        if (rv != SA_OK) {
                err("Remove OA has failed");
                return rv;
        }

        return SA_OK;
}

/**
 * process_oa_failover_event
 *      @oh_handler: Pointer to openhpi handler structure
 *      @oa:         Pointer to the OA structure
 *
 * Purpose:
 *      Gets the OA Failover event.
 *      Waits till the OA Transition Complete event is recieved from OA
 *      Gets the OA status of the other OA and updates the other OA data
 *      structure
 *
 * Detailed Description:
 *      - If the OA failover event is recieved just after the discovery on
 *        active OA, then the OA failoer event will be ignored
 *      - The active_con field of oa_handler is pointed to current active OA
 *      - Till OA_TRANSITION_COMPLETE event is recieved or maximum 90 seconds,
 *        all the events are ignored
 *      - OA needs some time to stabilize, after getting the OA failover event,
 *        plug-in starts checking for events after 90 seconds
 *      - Since there are high chances for missing the information of changes
 *        in the resources, re-discovery will be done before start listening
 *        for events.
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure
 **/
SaErrorT process_oa_failover_event(struct oh_handler_state *oh_handler,
                                   struct oa_info *oa)
{
        SaErrorT rv = SA_OK;
        SaHpiBoolT is_transition_complete = SAHPI_FALSE;
        SaHpiInt32T sleep_time = 0;
        struct oa_soap_handler *oa_handler = NULL;
        struct getAllEvents request;
        struct getAllEventsResponse response;
        struct eventInfo event;
        GTimer *timer = NULL;
        gulong micro_seconds;
        gdouble time_elapsed = 0;

        if (oh_handler == NULL || oa == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* If the switchover happens during discovery,
         * then, we will get failover event on Active OA
         * Discovery error recovery mechanism has handled the switchover.
         * Hence, ignore the failover event
         */
        if (oa->oa_status == ACTIVE) {
                dbg("OA failover event is recieved in active OA");
                dbg("Ignoring the OA failover event");
                return SA_OK;
        }

        err("OA got switched over");
        oa_handler = (struct oa_soap_handler *) oh_handler->data;

        /* Always lock the oa_handler mutex and then oa_info mutex
         * This is to avoid the deadlock
         */
        g_mutex_lock(oa_handler->mutex);
        g_mutex_lock(oa->mutex);

        /* Point the active_con to the current active OA's hpi_con */
        oa_handler->active_con = oa->hpi_con;
        /* This OA has become ACTIVE from STANDBY */
        oa->oa_status = ACTIVE;
        g_mutex_unlock(oa->mutex);

        /* Set the other OA status as STANDBY. If the other OA is extracted,
         * then the other OA status will be set to ABSENT during re-discovery.
         */
        if (oa_handler->oa_1 == oa) {
                g_mutex_lock(oa_handler->oa_2->mutex);
                oa_handler->oa_2->oa_status = STANDBY;
                g_mutex_unlock(oa_handler->oa_2->mutex);
        } else {
                g_mutex_lock(oa_handler->oa_1->mutex);
                oa_handler->oa_1->oa_status = STANDBY;
                g_mutex_unlock(oa_handler->oa_1->mutex);
        }

        request.pid = oa->event_pid;
        request.waitTilEventHappens = HPOA_TRUE;
        request.lcdEvents = HPOA_FALSE;

        /* Start the timer */
        timer = g_timer_new();
        /* Loop through till the TRANSITION_COMPLETE event is recieved
         * Or OA stabilization (90 seconds) time has reached
         */
        while (is_transition_complete != SAHPI_TRUE &&
              time_elapsed < OA_STABILIZE_MAX_TIME) {

                g_mutex_lock(oa->mutex);
                rv = soap_getAllEvents(oa->event_con, &request, &response);
                g_mutex_unlock(oa->mutex);
                if (rv != SOAP_OK) {
                        err("Get all events failed during OA switchover"
                             "processing for OA %s", oa->server);
                        /* Unlock the oa_handler mutex*/
                        g_mutex_unlock(oa_handler->mutex);
                        /* Cleanup the timer */
                        g_timer_destroy(timer);

                        /* May be OA is out of network or
                         * consecutive switch over has happened
                         * Try to recover from the problem
                         */
                        oa_soap_error_handling(oh_handler, oa);

                        /* Re-discovery is done in error handling
                         * hence return success
                         */
                        return SA_OK;
                }

                /* OA returns empty event response payload for LCD status
                 * change events.  Ignore empty event response.
                 */
                if (response.eventInfoArray == NULL) {
                        dbg("Ignoring empty event response");
                        time_elapsed = g_timer_elapsed(timer, &micro_seconds);
                        continue;
                }

                /* Check for transition complete event */
                while (response.eventInfoArray) {
                       soap_getEventInfo(response.eventInfoArray, &event);
                       if (event.event == EVENT_OA_TRANSITION_COMPLETE) {
                                   is_transition_complete = SAHPI_TRUE;
                                   break;
                       }
                       response.eventInfoArray =
                                soap_next_node(response.eventInfoArray);
                 }
                /* Get the time (in seconds) since the timer has been started */
                time_elapsed = g_timer_elapsed(timer, &micro_seconds);
        }

        /* Unlock the oa_handler mutex */
        g_mutex_unlock(oa_handler->mutex);

        /* Get the time (in seconds) since the timer has been started */
        time_elapsed = g_timer_elapsed(timer, &micro_seconds);
        g_timer_destroy(timer);

        /* OA requires some time to Stabilize. Wait for max 90 seconds */
        sleep_time = OA_STABILIZE_MAX_TIME - time_elapsed;
        dbg("Sleeping for %d seconds", sleep_time);
        if (sleep_time > 0) {
               sleep(sleep_time);
        }

        /* Check the OA staus there may be change in OA state */
        rv = check_oa_status(oa_handler, oa, oa->event_con);
        if (rv != SA_OK) {
                err("Check OA staus failed for OA %s", oa->server);
                oa_soap_error_handling(oh_handler, oa);
                /* Re-discovery is done in error handling hence
                 * return success
                 */
                return SA_OK;
        }

        /* Check the OA status, if it is not ACTIVE (switchover might have
         * happened while waiting for OA stabilization)
         * Return without doing re-discovery
         */
        g_mutex_lock(oa->mutex);
        if (oa->oa_status != ACTIVE) {
                g_mutex_unlock(oa->mutex);
                return SA_OK;
        }
        g_mutex_unlock(oa->mutex);

        g_mutex_lock(oa_handler->mutex);
        g_mutex_lock(oa->mutex);
        /* Call getAllEvents to flush the OA event queue
         * Any resource state change will be handled as part of the re-discovery
         */
        rv = soap_getAllEvents(oa->event_con, &request, &response);

        /* Re-discover the resources as there is a high chances
         * that we might have missed some events
         */
        rv = oa_soap_re_discover_resources(oh_handler, oa->event_con);
        g_mutex_unlock(oa->mutex);
        g_mutex_unlock(oa_handler->mutex);

        if (rv != SA_OK) {
                err("Re-discovery failed for OA %s", oa->server);
                oa_soap_error_handling(oh_handler, oa);
        }

        return SA_OK;
}

/**
 * process_oa_info_event
 *      @oh_handler: Pointer to openhpi handler structure
 *      @con:        Pointer to SOAP_CON structure
 *      @oa_event:   Pointer to the OA event structure
 *
 * Purpose:
 *      Gets the OA Info event.
 *      Adds the newly inserted OA information into RPT and RDR table
 *      Creates the hot swap event
 *
 * Detailed Description:
 *      - If the OA_INFO event is recived after the OA insertion event,
 *        then it is processed, else it is ignored
 *      - The OA_INFO event (after the OA insertion event) indicates
 *        the stabilization of OA.
 *      - When OA insertion event recieved, OA will not be stabilized
 *        On recieving this event, ACTIVE hot swap event will be generated
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure
 **/
SaErrorT process_oa_info_event(struct oh_handler_state *oh_handler,
                               SOAP_CON *con,
                               struct eventInfo *oa_event)
{
        SaErrorT rv = SA_OK;
        SaHpiInt32T bay_number;
        struct oa_soap_handler *oa_handler = NULL;

        if (oh_handler == NULL || con == NULL || oa_event == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        oa_handler = (struct oa_soap_handler *) oh_handler->data;
        bay_number = oa_event->eventData.oaInfo.bayNumber;

        if (oa_handler->oa_soap_resources.oa.presence[bay_number - 1] ==
                            RES_PRESENT) {
                dbg("OA is present. Ignore event");
                return SA_OK;
        }

        rv = add_oa(oh_handler, con, bay_number);
        if (rv != SA_OK) {
                err("Failed to add the newly-discovered OA");
        }

        return rv;
}
