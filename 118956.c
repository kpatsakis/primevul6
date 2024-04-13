void ogs_sbi_request_free(ogs_sbi_request_t *request)
{
    ogs_assert(request);

    if (request->h.uri)
        ogs_free(request->h.uri);

    ogs_sbi_header_free(&request->h);
    http_message_free(&request->http);

    ogs_pool_free(&request_pool, request);
}