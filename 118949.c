void ogs_sbi_message_final(void)
{
    ogs_pool_final(&request_pool);
    ogs_pool_final(&response_pool);
}