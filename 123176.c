virStorageVolPoolRefreshDataFree(void *opaque)
{
    virStorageVolStreamInfo *cbdata = opaque;

    g_free(cbdata->pool_name);
    g_free(cbdata);
}