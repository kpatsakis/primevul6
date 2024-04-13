static bool build_multipart(
        ogs_sbi_http_message_t *http, ogs_sbi_message_t *message)
{
    int i;

    char boundary[32];
    unsigned char digest[16];
    char *p = NULL, *last;

    char *content_type = NULL;
    char *json = NULL;

    ogs_assert(message);
    ogs_assert(http);

    ogs_random(digest, 16);
    strcpy(boundary, "=-");
    ogs_base64_encode_binary(boundary + 2, digest, 16);

    p = http->content = ogs_calloc(1, OGS_HUGE_LEN);
    ogs_expect_or_return_val(p, false);
    last = p + OGS_HUGE_LEN;

    /* First boundary */
    p = ogs_slprintf(p, last, "--%s\r\n", boundary);

    /* Encapsulated multipart part (application/json) */
    json = build_json(message);
    ogs_expect_or_return_val(json, false);

    p = ogs_slprintf(p, last, "%s\r\n\r\n%s",
            OGS_SBI_CONTENT_TYPE ": " OGS_SBI_CONTENT_JSON_TYPE, json);

    ogs_free(json);

    /* Add part */
    for (i = 0; i < message->num_of_part; i++) {
        p = ogs_slprintf(p, last, "\r\n--%s\r\n", boundary);
        p = ogs_slprintf(p, last, "%s: %s\r\n",
                OGS_SBI_CONTENT_ID, message->part[i].content_id);
        p = ogs_slprintf(p, last, "%s: %s\r\n\r\n",
                OGS_SBI_CONTENT_TYPE, message->part[i].content_type);
        memcpy(p, message->part[i].pkbuf->data, message->part[i].pkbuf->len);
        p += message->part[i].pkbuf->len;
    }

    /* Last boundary */
    p = ogs_slprintf(p, last, "\r\n--%s--\r\n", boundary);

    http->content_length = p - http->content;

    content_type = ogs_msprintf("%s; boundary=\"%s\"",
            OGS_SBI_CONTENT_MULTIPART_TYPE, boundary);
    ogs_expect_or_return_val(content_type, false);

    ogs_sbi_header_set(http->headers, OGS_SBI_CONTENT_TYPE, content_type);

    ogs_free(content_type);

    return true;
}