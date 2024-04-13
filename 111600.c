uc_err uc_free(void *mem)
{
    g_free(mem);
    return UC_ERR_OK;
}