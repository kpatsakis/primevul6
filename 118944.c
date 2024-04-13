static int on_header_field(
        multipart_parser *parser, const char *at, size_t length)
{
    multipart_parser_data_t *data = NULL;

    ogs_assert(parser);
    data = multipart_parser_get_data(parser);
    ogs_assert(data);

    if (at && length) {
        if (data->header_field)
            ogs_free(data->header_field);
        data->header_field = ogs_strndup(at, length);
        ogs_assert(data->header_field);
    }
    return 0;
}