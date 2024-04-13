static bool build_content(
        ogs_sbi_http_message_t *http, ogs_sbi_message_t *message)
{
    ogs_assert(message);
    ogs_assert(http);

    if (message->num_of_part) {
        ogs_expect_or_return_val(true ==
                build_multipart(http, message), false);
    } else {
        http->content = build_json(message);
        if (http->content) {
            http->content_length = strlen(http->content);
            if (message->http.content_type) {
                ogs_sbi_header_set(http->headers,
                        OGS_SBI_CONTENT_TYPE, message->http.content_type);
            } else {
                ogs_sbi_header_set(http->headers,
                        OGS_SBI_CONTENT_TYPE, OGS_SBI_CONTENT_JSON_TYPE);
            }
        }
    }

    return true;
}