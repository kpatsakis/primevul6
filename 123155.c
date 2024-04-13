virStorageVolPoolRefreshThread(void *opaque)
{
    virStorageVolStreamInfo *cbdata = opaque;
    virStoragePoolObj *obj = NULL;
    virStoragePoolDef *def;
    virStorageBackend *backend;
    virObjectEvent *event = NULL;

    if (cbdata->vol_path) {
        if (virStorageBackendPloopRestoreDesc(cbdata->vol_path) < 0)
            goto cleanup;
    }
    if (!(obj = virStoragePoolObjFindByName(driver->pools,
                                            cbdata->pool_name)))
        goto cleanup;
    def = virStoragePoolObjGetDef(obj);

    /* If some thread is building a new volume in the pool, then we cannot
     * clear out all vols and refresh the pool. So we'll just pass. */
    if (virStoragePoolObjGetAsyncjobs(obj) > 0) {
        VIR_DEBUG("Asyncjob in process, cannot refresh storage pool");
        goto cleanup;
    }

    if (!(backend = virStorageBackendForType(def->type)))
        goto cleanup;

    if (storagePoolRefreshImpl(backend, obj, NULL) < 0)
        VIR_DEBUG("Failed to refresh storage pool");

    event = virStoragePoolEventRefreshNew(def->name, def->uuid);

 cleanup:
    virObjectEventStateQueue(driver->storageEventState, event);
    virStoragePoolObjEndAPI(&obj);
    virStorageVolPoolRefreshDataFree(cbdata);
}