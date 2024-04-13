int ogs_sbi_parse_request(
        ogs_sbi_message_t *message, ogs_sbi_request_t *request)
{
    int rv;
    ogs_hash_index_t *hi;

    ogs_assert(request);
    ogs_assert(message);

    rv = ogs_sbi_parse_header(message, &request->h);
    if (rv != OGS_OK) {
        ogs_error("ogs_sbi_parse_header() failed");
        return OGS_ERROR;
    }

    for (hi = ogs_hash_first(request->http.params);
            hi; hi = ogs_hash_next(hi)) {
        if (!strcmp(ogs_hash_this_key(hi), OGS_SBI_PARAM_NF_ID)) {
            message->param.nf_id = ogs_hash_this_val(hi);
        } else if (!strcmp(ogs_hash_this_key(hi), OGS_SBI_PARAM_NF_TYPE)) {
            message->param.nf_type =
                OpenAPI_nf_type_FromString(ogs_hash_this_val(hi));
        } else if (!strcmp(ogs_hash_this_key(hi),
                    OGS_SBI_PARAM_TARGET_NF_TYPE)) {
            message->param.target_nf_type =
                OpenAPI_nf_type_FromString(ogs_hash_this_val(hi));
        } else if (!strcmp(ogs_hash_this_key(hi),
                    OGS_SBI_PARAM_REQUESTER_NF_TYPE)) {
            message->param.requester_nf_type =
                OpenAPI_nf_type_FromString(ogs_hash_this_val(hi));
        } else if (!strcmp(ogs_hash_this_key(hi), OGS_SBI_PARAM_LIMIT)) {
            message->param.limit = atoi(ogs_hash_this_val(hi));
        } else if (!strcmp(ogs_hash_this_key(hi), OGS_SBI_PARAM_DNN)) {
            message->param.dnn = ogs_hash_this_val(hi);
        } else if (!strcmp(ogs_hash_this_key(hi), OGS_SBI_PARAM_PLMN_ID)) {
            char *v = NULL;
            cJSON *item = NULL;
            OpenAPI_plmn_id_t *plmn_id = NULL;

            v = ogs_hash_this_val(hi);
            if (v) {
                item = cJSON_Parse(v);
                if (item) {
                    plmn_id = OpenAPI_plmn_id_parseFromJSON(item);
                    if (plmn_id && plmn_id->mnc && plmn_id->mcc) {
                        ogs_plmn_id_build(&message->param.plmn_id,
                            atoi(plmn_id->mcc),
                            atoi(plmn_id->mnc), strlen(plmn_id->mnc));
                        message->param.plmn_id_presence = true;
                        OpenAPI_plmn_id_free(plmn_id);
                    }
                    cJSON_Delete(item);
                }
            }
        } else if (!strcmp(ogs_hash_this_key(hi), OGS_SBI_PARAM_SINGLE_NSSAI)) {
            char *v = ogs_hash_this_val(hi);
            if (v) {
                bool rc = ogs_sbi_s_nssai_from_string(
                        &message->param.s_nssai, v);
                if (rc == true)
                    message->param.single_nssai_presence = true;
            }
        } else if (!strcmp(ogs_hash_this_key(hi), OGS_SBI_PARAM_SNSSAI)) {
            char *v = ogs_hash_this_val(hi);
            if (v) {
                bool rc = ogs_sbi_s_nssai_from_string(
                        &message->param.s_nssai, v);
                if (rc == true)
                    message->param.snssai_presence = true;
            }
        } else if (!strcmp(ogs_hash_this_key(hi),
                    OGS_SBI_PARAM_SLICE_INFO_REQUEST_FOR_PDU_SESSION)) {
            char *v = NULL;
            cJSON *item = NULL;
            OpenAPI_slice_info_for_pdu_session_t *SliceInfoForPduSession = NULL;

            v = ogs_hash_this_val(hi);
            if (v) {
                item = cJSON_Parse(v);
                if (item) {
                    SliceInfoForPduSession =
                        OpenAPI_slice_info_for_pdu_session_parseFromJSON(item);
                    if (SliceInfoForPduSession) {
                        OpenAPI_snssai_t *s_nssai =
                            SliceInfoForPduSession->s_nssai;
                        if (s_nssai) {
                            message->param.s_nssai.sst = s_nssai->sst;
                            message->param.s_nssai.sd =
                                ogs_s_nssai_sd_from_string(s_nssai->sd);
                        }
                        message->param.roaming_indication =
                            SliceInfoForPduSession->roaming_indication;
                        message->param.
                            slice_info_request_for_pdu_session_presence = true;

                        OpenAPI_slice_info_for_pdu_session_free(
                                SliceInfoForPduSession);

                    }
                    cJSON_Delete(item);
                }
            }
        } else if (!strcmp(ogs_hash_this_key(hi), OGS_SBI_PARAM_IPV4ADDR)) {
            message->param.ipv4addr = ogs_hash_this_val(hi);
        } else if (!strcmp(ogs_hash_this_key(hi), OGS_SBI_PARAM_IPV6PREFIX)) {
            message->param.ipv6prefix = ogs_hash_this_val(hi);
        }
    }

    for (hi = ogs_hash_first(request->http.headers);
            hi; hi = ogs_hash_next(hi)) {
        if (!ogs_strcasecmp(ogs_hash_this_key(hi), OGS_SBI_ACCEPT_ENCODING)) {
            message->http.content_encoding = ogs_hash_this_val(hi);
        } else if (!ogs_strcasecmp(
                    ogs_hash_this_key(hi), OGS_SBI_CONTENT_TYPE)) {
            message->http.content_type = ogs_hash_this_val(hi);
        } else if (!ogs_strcasecmp(ogs_hash_this_key(hi), OGS_SBI_ACCEPT)) {
            message->http.accept = ogs_hash_this_val(hi);
        }
    }

    if (parse_content(message, &request->http) != OGS_OK) {
        ogs_error("parse_content() failed");
        return OGS_ERROR;
    }

    return OGS_OK;
}