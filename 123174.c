storagePoolRefresh(virStoragePoolPtr pool,
                   unsigned int flags)
{
    virStoragePoolObj *obj;
    virStoragePoolDef *def;
    virStorageBackend *backend;
    g_autofree char *stateFile = NULL;
    int ret = -1;
    virObjectEvent *event = NULL;

    virCheckFlags(0, -1);

    if (!(obj = storagePoolObjFindByUUID(pool->uuid, pool->name)))
        goto cleanup;
    def = virStoragePoolObjGetDef(obj);

    if (virStoragePoolRefreshEnsureACL(pool->conn, def) < 0)
        goto cleanup;

    if ((backend = virStorageBackendForType(def->type)) == NULL)
        goto cleanup;

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

    stateFile = virFileBuildPath(driver->stateDir, def->name, ".xml");
    if (storagePoolRefreshImpl(backend, obj, stateFile) < 0) {
        event = virStoragePoolEventLifecycleNew(def->name,
                                                def->uuid,
                                                VIR_STORAGE_POOL_EVENT_STOPPED,
                                                0);
        virStoragePoolObjSetActive(obj, false);

        virStoragePoolUpdateInactive(obj);

        goto cleanup;
    }

    event = virStoragePoolEventRefreshNew(def->name,
                                          def->uuid);
    ret = 0;

 cleanup:
    virObjectEventStateQueue(driver->storageEventState, event);
    virStoragePoolObjEndAPI(&obj);
    return ret;
}