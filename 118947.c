int ogs_sbi_parse_header(ogs_sbi_message_t *message, ogs_sbi_header_t *header)
{
    struct yuarel yuarel;
    char *saveptr = NULL;
    char *uri = NULL, *p = NULL;;

    char *component = NULL;
    int i = 0;

    ogs_assert(message);
    ogs_assert(header);

    memset(message, 0, sizeof(*message));

    message->h.method = header->method;
    message->h.uri = header->uri;
    ogs_assert(message->h.uri);

    uri = ogs_strdup(header->uri);
    ogs_assert(uri);
    p = uri;

    if (p[0] != '/') {
        int rv = yuarel_parse(&yuarel, p);
        if (rv != OGS_OK) {
            ogs_error("yuarel_parse() failed");
            ogs_free(uri);
            return OGS_ERROR;
        }

        p = yuarel.path;
    }

    header->service.name = ogs_sbi_parse_uri(p, "/", &saveptr);
    if (!header->service.name) {
        ogs_error("ogs_sbi_parse_uri() failed");
        ogs_free(uri);
        return OGS_ERROR;
    }
    message->h.service.name = header->service.name;

    header->api.version = ogs_sbi_parse_uri(NULL, "/", &saveptr);
    if (!header->api.version) {
        ogs_error("ogs_sbi_parse_uri() failed");
        ogs_free(uri);
        return OGS_ERROR;
    }
    message->h.api.version = header->api.version;

    for (i = 0; i < OGS_SBI_MAX_NUM_OF_RESOURCE_COMPONENT &&
            (component = ogs_sbi_parse_uri(NULL, "/", &saveptr)) != NULL;
         i++) {
        header->resource.component[i] = component;
        message->h.resource.component[i] = component;
    }

    ogs_free(uri);

    return OGS_OK;
}