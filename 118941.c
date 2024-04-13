ogs_sbi_request_t *ogs_sbi_build_request(ogs_sbi_message_t *message)
{
    ogs_sbi_request_t *request = NULL;

    ogs_assert(message);

    request = ogs_sbi_request_new();
    ogs_expect_or_return_val(request, NULL);

    ogs_expect_or_return_val(message->h.method, NULL);
    request->h.method = ogs_strdup(message->h.method);
    if (message->h.uri) {
        ogs_expect_or_return_val(message->h.uri, NULL);
        request->h.uri = ogs_strdup(message->h.uri);
        ogs_expect_or_return_val(request->h.uri, NULL);
    } else {
        int i;

        ogs_expect_or_return_val(message->h.service.name, NULL);
        request->h.service.name = ogs_strdup(message->h.service.name);
        ogs_expect_or_return_val(message->h.api.version, NULL);
        request->h.api.version = ogs_strdup(message->h.api.version);
        ogs_expect_or_return_val(request->h.api.version, NULL);

        ogs_expect_or_return_val(message->h.resource.component[0], NULL);
        for (i = 0; i < OGS_SBI_MAX_NUM_OF_RESOURCE_COMPONENT &&
                            message->h.resource.component[i]; i++)
            request->h.resource.component[i] = ogs_strdup(
                    message->h.resource.component[i]);
    }

    /* URL Param */
    if (message->param.nf_id) {
        ogs_sbi_header_set(request->http.params,
                OGS_SBI_PARAM_NF_ID, message->param.nf_id);
    }
    if (message->param.nf_type) {
        char *v = OpenAPI_nf_type_ToString(message->param.nf_type);
        ogs_expect_or_return_val(v, NULL);
        ogs_sbi_header_set(request->http.params, OGS_SBI_PARAM_NF_TYPE, v);
    }
    if (message->param.requester_nf_type) {
        char *v = OpenAPI_nf_type_ToString(message->param.requester_nf_type);
        ogs_expect_or_return_val(v, NULL);
        ogs_sbi_header_set(request->http.params,
                OGS_SBI_PARAM_REQUESTER_NF_TYPE, v);
    }
    if (message->param.target_nf_type) {
        char *v = OpenAPI_nf_type_ToString(message->param.target_nf_type);
        ogs_expect_or_return_val(v, NULL);
        ogs_sbi_header_set(request->http.params,
                OGS_SBI_PARAM_TARGET_NF_TYPE, v);
    }
    if (message->param.limit) {
        char *v = ogs_msprintf("%d", message->param.limit);
        ogs_expect_or_return_val(v, NULL);
        ogs_sbi_header_set(request->http.params, OGS_SBI_PARAM_LIMIT, v);
        ogs_free(v);
    }
    if (message->param.dnn) {
        ogs_sbi_header_set(request->http.params,
                OGS_SBI_PARAM_DNN, message->param.dnn);
    }
    if (message->param.plmn_id_presence) {
        OpenAPI_plmn_id_t plmn_id;

        plmn_id.mnc = ogs_plmn_id_mnc_string(&message->param.plmn_id);
        plmn_id.mcc = ogs_plmn_id_mcc_string(&message->param.plmn_id);

        if (plmn_id.mnc && plmn_id.mcc) {
            char *v = NULL;
            cJSON *item = NULL;

            item = OpenAPI_plmn_id_convertToJSON(&plmn_id);
            ogs_expect_or_return_val(item, NULL);
            if (plmn_id.mnc) ogs_free(plmn_id.mnc);
            if (plmn_id.mcc) ogs_free(plmn_id.mcc);

            v = cJSON_Print(item);
            ogs_expect_or_return_val(v, NULL);
            cJSON_Delete(item);

            ogs_sbi_header_set(request->http.params, OGS_SBI_PARAM_PLMN_ID, v);
            ogs_free(v);
        }
    }
    if (message->param.single_nssai_presence) {
        char *v = ogs_sbi_s_nssai_to_string(&message->param.s_nssai);
        ogs_expect_or_return_val(v, NULL);
        ogs_sbi_header_set(request->http.params, OGS_SBI_PARAM_SINGLE_NSSAI, v);
        ogs_free(v);
    }
    if (message->param.snssai_presence) {
        char *v = ogs_sbi_s_nssai_to_string(&message->param.s_nssai);
        ogs_expect_or_return_val(v, NULL);
        ogs_sbi_header_set(request->http.params, OGS_SBI_PARAM_SNSSAI, v);
        ogs_free(v);
    }
    if (message->param.plmn_id_presence) {
        OpenAPI_plmn_id_t plmn_id;

        plmn_id.mnc = ogs_plmn_id_mnc_string(&message->param.plmn_id);
        plmn_id.mcc = ogs_plmn_id_mcc_string(&message->param.plmn_id);

        if (plmn_id.mnc && plmn_id.mcc) {
            char *v = NULL;
            cJSON *item = NULL;

            item = OpenAPI_plmn_id_convertToJSON(&plmn_id);
            ogs_expect_or_return_val(item, NULL);
            if (plmn_id.mnc) ogs_free(plmn_id.mnc);
            if (plmn_id.mcc) ogs_free(plmn_id.mcc);

            v = cJSON_Print(item);
            ogs_expect_or_return_val(v, NULL);
            cJSON_Delete(item);

            ogs_sbi_header_set(request->http.params, OGS_SBI_PARAM_PLMN_ID, v);
            ogs_free(v);
        }
    }
    if (message->param.slice_info_request_for_pdu_session_presence) {
        OpenAPI_slice_info_for_pdu_session_t SliceInfoForPDUSession;
        OpenAPI_snssai_t sNSSAI;

        char *v = NULL;
        cJSON *item = NULL;

        ogs_expect_or_return_val(message->param.s_nssai.sst, NULL);
        ogs_expect_or_return_val(message->param.roaming_indication, NULL);

        memset(&sNSSAI, 0, sizeof(sNSSAI));
        sNSSAI.sst = message->param.s_nssai.sst;
        sNSSAI.sd = ogs_s_nssai_sd_to_string(message->param.s_nssai.sd);

        memset(&SliceInfoForPDUSession, 0, sizeof(SliceInfoForPDUSession));

        SliceInfoForPDUSession.s_nssai = &sNSSAI;
        SliceInfoForPDUSession.roaming_indication =
            message->param.roaming_indication;

        item = OpenAPI_slice_info_for_pdu_session_convertToJSON(
                &SliceInfoForPDUSession);
        ogs_expect_or_return_val(item, NULL);

        v = cJSON_Print(item);
        ogs_expect_or_return_val(v, NULL);
        cJSON_Delete(item);

        ogs_sbi_header_set(request->http.params,
                OGS_SBI_PARAM_SLICE_INFO_REQUEST_FOR_PDU_SESSION, v);
        ogs_free(v);

        if (sNSSAI.sd)
            ogs_free(sNSSAI.sd);
    }
    if (message->param.ipv4addr) {
        ogs_sbi_header_set(request->http.params,
                OGS_SBI_PARAM_IPV4ADDR, message->param.ipv4addr);
    }
    if (message->param.ipv6prefix) {
        ogs_sbi_header_set(request->http.params,
                OGS_SBI_PARAM_IPV6PREFIX, message->param.ipv6prefix);
    }

    ogs_expect_or_return_val(true ==
            build_content(&request->http, message), NULL);

    if (message->http.accept) {
        ogs_sbi_header_set(request->http.headers, OGS_SBI_ACCEPT,
                message->http.accept);
    } else {
        SWITCH(message->h.method)
        CASE(OGS_SBI_HTTP_METHOD_DELETE)
            ogs_sbi_header_set(request->http.headers, OGS_SBI_ACCEPT,
                OGS_SBI_CONTENT_PROBLEM_TYPE);
            break;
        DEFAULT
            ogs_sbi_header_set(request->http.headers, OGS_SBI_ACCEPT,
                OGS_SBI_CONTENT_JSON_TYPE "," OGS_SBI_CONTENT_PROBLEM_TYPE);
            break;
        END
    }

    if (message->http.content_encoding)
        ogs_sbi_header_set(request->http.headers,
                OGS_SBI_ACCEPT_ENCODING, message->http.content_encoding);

    return request;
}