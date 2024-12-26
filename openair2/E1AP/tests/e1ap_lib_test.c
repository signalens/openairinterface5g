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

/*! \file e1ap_lib_test.c
 * \brief Unit tests for E1AP libraries
 * \author Guido Casati
 * \date 2024
 * \version 0.1
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "E1AP/lib/e1ap_bearer_context_management.h"
#include "E1AP/lib/e1ap_lib_includes.h"
#include "common/utils/assertions.h"

void exit_function(const char *file, const char *function, const int line, const char *s, const int assert)
{
  printf("detected error at %s:%d:%s: %s\n", file, line, function, s);
  abort();
}

static E1AP_E1AP_PDU_t *e1ap_encode_decode(const E1AP_E1AP_PDU_t *enc_pdu)
{
  // xer_fprint(stdout, &asn_DEF_E1AP_E1AP_PDU, enc_pdu);

  DevAssert(enc_pdu != NULL);
  char errbuf[2048];
  size_t errlen = sizeof(errbuf);
  int ret = asn_check_constraints(&asn_DEF_E1AP_E1AP_PDU, enc_pdu, errbuf, &errlen);
  AssertFatal(ret == 0, "asn_check_constraints() failed: %s\n", errbuf);

  uint8_t msgbuf[16384];
  asn_enc_rval_t enc = aper_encode_to_buffer(&asn_DEF_E1AP_E1AP_PDU, NULL, enc_pdu, msgbuf, sizeof(msgbuf));
  AssertFatal(enc.encoded > 0, "aper_encode_to_buffer() failed\n");

  E1AP_E1AP_PDU_t *dec_pdu = NULL;
  asn_codec_ctx_t st = {.max_stack_size = 100 * 1000};
  asn_dec_rval_t dec = aper_decode(&st, &asn_DEF_E1AP_E1AP_PDU, (void **)&dec_pdu, msgbuf, enc.encoded, 0, 0);
  AssertFatal(dec.code == RC_OK, "aper_decode() failed\n");

  // xer_fprint(stdout, &asn_DEF_E1AP_E1AP_PDU, dec_pdu);
  return dec_pdu;
}

static void e1ap_msg_free(E1AP_E1AP_PDU_t *pdu)
{
  ASN_STRUCT_FREE(asn_DEF_E1AP_E1AP_PDU, pdu);
}

/**
 * @brief Test E1AP Bearer Context Setup Request encoding/decoding
 */
static void test_bearer_context_setup_request(void)
{
  bearer_context_pdcp_config_t pdcp = {
    .discardTimer = E1AP_DiscardTimer_ms10,
    .pDCP_Reestablishment = E1AP_PDCP_Reestablishment_true,
    .pDCP_SN_Size_DL = E1AP_PDCP_SN_Size_s_12,
    .pDCP_SN_Size_UL = E1AP_PDCP_SN_Size_s_12,
    .reorderingTimer = 10,
    .rLC_Mode = E1AP_RLC_Mode_rlc_am,
  };

  bearer_context_sdap_config_t sdap = {
    .defaultDRB = 1,
    .sDAP_Header_DL = true,
    .sDAP_Header_UL = true,
  };

  qos_flow_to_setup_t qos = {
    .qfi = 1,
    .qos_params.alloc_reten_priority.preemption_capability = E1AP_Pre_emptionCapability_shall_not_trigger_pre_emption,
    .qos_params.alloc_reten_priority.preemption_vulnerability = E1AP_Pre_emptionVulnerability_not_pre_emptable,
    .qos_params.alloc_reten_priority.priority_level = E1AP_PriorityLevel_highest,
    .qos_params.qos_characteristics.non_dynamic.fiveqi = 9,
  };

  security_indication_t security = {
    .confidentialityProtectionIndication = E1AP_ConfidentialityProtectionIndication_required,
    .integrityProtectionIndication = E1AP_IntegrityProtectionIndication_required,
    .maxIPrate = E1AP_MaxIPrate_max_UErate,
  };

  // Step 1: Initialize the E1AP Bearer Context Setup Request
  e1ap_bearer_setup_req_t orig = {
      .gNB_cu_cp_ue_id = 1234,
      .cipheringAlgorithm = 0x01,
      .integrityProtectionAlgorithm = 0x01,
      .ueDlAggMaxBitRate = 1000000000,
      .bearerContextStatus = 0,
      .servingPLMNid.mcc = 001,
      .servingPLMNid.mnc = 01,
      .servingPLMNid.mnc_digit_length = 0x02,
      .activityNotificationLevel = ANL_PDU_SESSION,
      .numPDUSessions = 1,
      .pduSession[0].sessionId = 1,
      .pduSession[0].sessionType = E1AP_PDU_Session_Type_ipv4,
      .pduSession[0].nssai.sd = 0x01,
      .pduSession[0].nssai.sst = 0x01,
      .pduSession[0].securityIndication = security,
      .pduSession[0].numDRB2Setup = 1,
      .pduSession[0].UP_TL_information.tlAddress = 167772161,
      .pduSession[0].UP_TL_information.teId = 0x12345,
      .pduSession[0].DRBnGRanList[0].id = 1,
      .pduSession[0].DRBnGRanList[0].sdap_config = sdap,
      .pduSession[0].DRBnGRanList[0].pdcp_config = pdcp,
      .pduSession[0].DRBnGRanList[0].id = 1,
      .pduSession[0].DRBnGRanList[0].numCellGroups = 1,
      .pduSession[0].DRBnGRanList[0].cellGroupList[0] = MCG,
      .pduSession[0].DRBnGRanList[0].numQosFlow2Setup = 1,
      .pduSession[0].DRBnGRanList[0].qosFlows[0] = qos,
  };
  memset(orig.encryptionKey, 0xAB, sizeof(orig.encryptionKey));
  memset(orig.integrityProtectionKey, 0xCD, sizeof(orig.integrityProtectionKey));
  // E1AP encode the original message
  E1AP_E1AP_PDU_t *enc = encode_E1_bearer_context_setup_request(&orig);
  // E1AP decode the encoded message
  E1AP_E1AP_PDU_t *dec = e1ap_encode_decode(enc);
  // Free the E1AP encoded message
  e1ap_msg_free(enc);
  // E1 message decode
  e1ap_bearer_setup_req_t decoded = {0};
  bool ret = decode_E1_bearer_context_setup_request(dec, &decoded);
  AssertFatal(ret, "decode_E1_bearer_context_setup_request(): could not decode message\n");
  // Free the E1AP decoded message
  e1ap_msg_free(dec);
  // Equality check original/decoded
  ret = eq_bearer_context_setup_request(&orig, &decoded);
  AssertFatal(ret, "eq_bearer_context_setup_request(): decoded message doesn't match\n");
  // Free the memory for the decoded message
  free_e1ap_context_setup_request(&decoded);
  // Deep copy and equality check of the original message
  e1ap_bearer_setup_req_t cp = cp_bearer_context_setup_request(&orig);
  ret = eq_bearer_context_setup_request(&orig, &cp);
  AssertFatal(ret, "eq_bearer_context_setup_request(): copied message doesn't match\n");
  // Free the copied message and original
  free_e1ap_context_setup_request(&cp);
  free_e1ap_context_setup_request(&orig);
}

/**
 * @brief Test E1AP Bearer Context Setup Response encoding/decoding
 */
static void test_bearer_context_setup_response(void)
{
  // Step 1: Initialize the E1AP Bearer Context Setup Response
  e1ap_bearer_setup_resp_t orig = {
      .gNB_cu_cp_ue_id = 1234,
      .gNB_cu_up_ue_id = 5678,
      .numPDUSessions = 1,
      .pduSession[0].id = 1,
      .pduSession[0].tlAddress = 167772161,
      .pduSession[0].teId = 0x12345,
      .pduSession[0].numDRBSetup = 1,
      .pduSession[0].numDRBFailed = 0,
      .pduSession[0].DRBnGRanList[0].id = 1,
      .pduSession[0].DRBnGRanList[0].numUpParam = 1,
      .pduSession[0].DRBnGRanList[0].numQosFlowSetup = 1,
      .pduSession[0].DRBnGRanList[0].qosFlows[0].qfi = 1,
      .pduSession[0].DRBnGRanList[0].UpParamList[0].cell_group_id = MCG,
      .pduSession[0].DRBnGRanList[0].UpParamList[0].teId = 0x34345,
      .pduSession[0].DRBnGRanList[0].UpParamList[0].tlAddress = 167772161,
  };
  // E1AP encode the original message
  E1AP_E1AP_PDU_t *enc = encode_E1_bearer_context_setup_response(&orig);

  // E1AP decode the encoded message
  E1AP_E1AP_PDU_t *dec = e1ap_encode_decode(enc);

  // Free the E1AP encoded message
  e1ap_msg_free(enc);

  // E1 message decode
  e1ap_bearer_setup_resp_t decoded = {0};
  bool ret = decode_E1_bearer_context_setup_response(dec, &decoded);
  AssertFatal(ret, "decode_E1_bearer_context_setup_response(): could not decode message\n");

  // Free the E1AP decoded message
  e1ap_msg_free(dec);

  // Equality check original/decoded
  ret = eq_bearer_context_setup_response(&orig, &decoded);
  AssertFatal(ret, "eq_bearer_context_setup_response(): decoded message doesn't match\n");

  // Free the memory for the decoded message
  free_e1ap_context_setup_response(&decoded);

  // Deep copy and equality check of the original message
  e1ap_bearer_setup_resp_t cp = cp_bearer_context_setup_response(&orig);
  ret = eq_bearer_context_setup_response(&orig, &cp);
  AssertFatal(ret, "eq_bearer_context_setup_response(): copied message doesn't match\n");

  // Free the copied message and original
  free_e1ap_context_setup_response(&cp);
  free_e1ap_context_setup_response(&orig);
}

int main()
{
  // E1 Bearer Context Setup
  test_bearer_context_setup_request();
  test_bearer_context_setup_response();
  return 0;
}
