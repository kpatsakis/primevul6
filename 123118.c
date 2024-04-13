storagePoolDestroy(virStoragePoolPtr pool)
{
    virStoragePoolObj *obj;
    virStoragePoolDef *def;
    virStorageBackend *backend;
    virObjectEvent *event = NULL;
    int ret = -1;
    g_autofree char *stateFile = NULL;

    if (!(obj = storagePoolObjFindByUUID(pool->uuid, pool->name)))
        goto cleanup;
    def = virStoragePoolObjGetDef(obj);

    if (virStoragePoolDestroyEnsureACL(pool->conn, def) < 0)
        goto cleanup;

    if ((backend = virStorageBackendForType(def->type)) == NULL)
        goto cleanup;

    VIR_INFO("Destroying storage pool '%s'", def->name);

    if (!virStoragePoolObjIsActive(obj)) {
        virReportError(VIR_ERR_OPERATION_INVALID,
                       _("storage pool '%s' is not active"), def->name);
        goto cleanup;
    }

    if (virStoragePoolObjIsStarting(obj)) {
        virReportError(VIR_ERR_OPERATION_INVALID,
                       _("storage pool '%s' is starting up"),
                       def->name);
        goto cleanup;
    }

    if (virStoragePoolObjGetAsyncjobs(obj) > 0) {
        virReportError(VIR_ERR_INTERNAL_ERROR,
                       _("pool '%s' has asynchronous jobs running."),
                       def->name);
        goto cleanup;
    }

    if (!(stateFile = virFileBuildPath(driver->stateDir, def->name, ".xml")))
        goto cleanup;

    unlink(stateFile);

    if (backend->stopPool &&
        backend->stopPool(obj) < 0)
        goto cleanup;

    virStoragePoolObjClearVols(obj);

    event = virStoragePoolEventLifecycleNew(def->name,
                                            def->uuid,
                                            VIR_STORAGE_POOL_EVENT_STOPPED,
                                            0);

    virStoragePoolObjSetActive(obj, false);

    virStoragePoolUpdateInactive(obj);

    ret = 0;

 cleanup:
    virObjectEventStateQueue(driver->storageEventState, event);
    virStoragePoolObjEndAPI(&obj);
    return ret;
}