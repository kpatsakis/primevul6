int ogs_sbi_parse_response(
        ogs_sbi_message_t *message, ogs_sbi_response_t *response)
{
    int rv;
    ogs_hash_index_t *hi;

    ogs_assert(response);
    ogs_assert(message);

    rv = ogs_sbi_parse_header(message, &response->h);
    if (rv != OGS_OK) {
        ogs_error("ogs_sbi_parse_header() failed");
        return OGS_ERROR;
    }

    for (hi = ogs_hash_first(response->http.headers);
            hi; hi = ogs_hash_next(hi)) {
        if (!ogs_strcasecmp(ogs_hash_this_key(hi), OGS_SBI_CONTENT_TYPE)) {
            message->http.content_type = ogs_hash_this_val(hi);
        } else if (!ogs_strcasecmp(ogs_hash_this_key(hi), OGS_SBI_LOCATION)) {
            message->http.location = ogs_hash_this_val(hi);
        }
    }

    message->res_status = response->status;

    if (parse_content(message, &response->http) != OGS_OK) {
        ogs_error("parse_content() failed");
        return OGS_ERROR;
    }

    return OGS_OK;
}