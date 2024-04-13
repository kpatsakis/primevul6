void ogs_sbi_response_free(ogs_sbi_response_t *response)
{
    ogs_assert(response);

    if (response->h.uri)
        ogs_free(response->h.uri);

    ogs_sbi_header_free(&response->h);
    http_message_free(&response->http);

    ogs_pool_free(&response_pool, response);
}