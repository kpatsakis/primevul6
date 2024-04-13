storagePoolBuild(virStoragePoolPtr pool,
                 unsigned int flags)
{
    virStoragePoolObj *obj;
    virStoragePoolDef *def;
    virStorageBackend *backend;
    virObjectEvent *event = NULL;
    bool restoreStarting = false;
    int ret = -1;

    if (!(obj = virStoragePoolObjFromStoragePool(pool)))
        return -1;
    def = virStoragePoolObjGetDef(obj);

    if (virStoragePoolBuildEnsureACL(pool->conn, def) < 0)
        goto cleanup;

    if ((backend = virStorageBackendForType(def->type)) == NULL)
        goto cleanup;

    if (virStoragePoolObjIsActive(obj)) {
        virReportError(VIR_ERR_OPERATION_INVALID,
                       _("storage pool '%s' is already active"),
                       def->name);
        goto cleanup;
    }

    if (virStoragePoolObjIsStarting(obj)) {
        virReportError(VIR_ERR_OPERATION_INVALID,
                       _("storage pool '%s' is starting up"),
                       def->name);
        goto cleanup;
    }

    virStoragePoolObjSetStarting(obj, true);
    restoreStarting = true;

    if (backend->buildPool &&
        backend->buildPool(obj, flags) < 0)
        goto cleanup;

    event = virStoragePoolEventLifecycleNew(def->name,
                                            def->uuid,
                                            VIR_STORAGE_POOL_EVENT_CREATED,
                                            0);

    ret = 0;

 cleanup:
    if (restoreStarting &&
        virStoragePoolObjIsStarting(obj)) {
        virStoragePoolUpdateInactive(obj);
        virStoragePoolObjSetStarting(obj, false);
    }
    virObjectEventStateQueue(driver->storageEventState, event);
    virStoragePoolObjEndAPI(&obj);
    return ret;
}