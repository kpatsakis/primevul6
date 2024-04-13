ogs_sbi_response_t *ogs_sbi_build_response(
        ogs_sbi_message_t *message, int status)
{
    ogs_sbi_response_t *response = NULL;

    ogs_assert(message);

    response = ogs_sbi_response_new();
    ogs_expect_or_return_val(response, NULL);

    response->status = status;

    if (response->status != OGS_SBI_HTTP_STATUS_NO_CONTENT) {
        ogs_expect_or_return_val(true ==
            build_content(&response->http, message), NULL);
    }

    if (message->http.location) {
        ogs_sbi_header_set(response->http.headers, "Location",
                message->http.location);
    }
    if (message->http.cache_control)
        ogs_sbi_header_set(response->http.headers, "Cache-Control",
                message->http.cache_control);

    return response;
}