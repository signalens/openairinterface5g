/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this file
 * except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.openairinterface.org/?page_id=698
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */

/*! \file ngap_gNB_overload.c
 * \brief ngap procedures for overload messages within gNB
 * \author Yoshio INOUE, Masayuki HARADA
 * \date 2020
 * \version 0.1
 * \email: yoshio.inoue@fujitsu.com,masayuki.harada@fujitsu.com (yoshio.inoue%40fujitsu.com%2cmasayuki.harada%40fujitsu.com)
 */

#include "ngap_gNB_overload.h"
#include <stdint.h>
#include <stdio.h>
#include "assertions.h"
#include "ngap_gNB_defs.h"
#include "ngap_gNB_management_procedures.h"

int ngap_gNB_handle_overload_start(sctp_assoc_t assoc_id, uint32_t stream, NGAP_NGAP_PDU_t *pdu)
{
    //TODO
    return 0;
}

int ngap_gNB_handle_overload_stop(sctp_assoc_t assoc_id, uint32_t stream, NGAP_NGAP_PDU_t *pdu)
{
    /* We received Overload stop message, meaning that the AMF is no more
     * overloaded. This is an empty message, with only message header and no
     * Information Element.
     */
    DevAssert(pdu != NULL);

    ngap_gNB_amf_data_t *amf_desc_p;

    /* Non UE-associated signalling -> stream 0 */
    DevCheck(stream == 0, stream, 0, 0);

    if ((amf_desc_p = ngap_gNB_get_AMF(NULL, assoc_id, 0)) == NULL) {
        /* No AMF context associated */
        return -1;
    }

    amf_desc_p->state = NGAP_GNB_STATE_CONNECTED;
    amf_desc_p->overload_state = NGAP_NO_OVERLOAD;
    return 0;
}
