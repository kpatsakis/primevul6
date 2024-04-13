ogs_sbi_response_t *ogs_sbi_response_new(void)
{
    ogs_sbi_response_t *response = NULL;

    ogs_pool_alloc(&response_pool, &response);
    ogs_expect_or_return_val(response, NULL);
    memset(response, 0, sizeof(ogs_sbi_response_t));

    response->http.params = ogs_hash_make();
    ogs_expect_or_return_val(response->http.params, NULL);
    response->http.headers = ogs_hash_make();
    ogs_expect_or_return_val(response->http.headers, NULL);

    return response;
}