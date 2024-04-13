static int parse_content(
        ogs_sbi_message_t *message, ogs_sbi_http_message_t *http)
{
    ogs_assert(message);
    ogs_assert(http);

    if (message->http.content_type &&
        !strncmp(message->http.content_type, OGS_SBI_CONTENT_MULTIPART_TYPE,
            strlen(OGS_SBI_CONTENT_MULTIPART_TYPE))) {
        return parse_multipart(message, http);
    } else {
        return parse_json(message, message->http.content_type, http->content);
    }
}