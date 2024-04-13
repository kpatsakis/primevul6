ogs_sbi_request_t *ogs_sbi_request_new(void)
{
    ogs_sbi_request_t *request = NULL;

    ogs_pool_alloc(&request_pool, &request);
    ogs_expect_or_return_val(request, NULL);
    memset(request, 0, sizeof(ogs_sbi_request_t));

    request->http.params = ogs_hash_make();
    ogs_expect_or_return_val(request->http.params, NULL);
    request->http.headers = ogs_hash_make();
    ogs_expect_or_return_val(request->http.headers, NULL);

    return request;
}