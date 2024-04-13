void ogs_sbi_message_init(int num_of_request_pool, int num_of_response_pool)
{
    ogs_pool_init(&request_pool, num_of_request_pool);
    ogs_pool_init(&response_pool, num_of_response_pool);
}