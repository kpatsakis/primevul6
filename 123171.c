storagePoolDelete(virStoragePoolPtr pool,
                  unsigned int flags)
{
    virStoragePoolObj *obj;
    virStoragePoolDef *def;
    virStorageBackend *backend;
    virObjectEvent *event = NULL;
    int ret = -1;
    g_autofree char *stateFile = NULL;

    if (!(obj = virStoragePoolObjFromStoragePool(pool)))
        return -1;
    def = virStoragePoolObjGetDef(obj);

    if (virStoragePoolDeleteEnsureACL(pool->conn, def) < 0)
        goto cleanup;

    if ((backend = virStorageBackendForType(def->type)) == NULL)
        goto cleanup;

    VIR_INFO("Deleting storage pool '%s'", def->name);

    if (virStoragePoolObjIsActive(obj)) {
        virReportError(VIR_ERR_OPERATION_INVALID,
                       _("storage pool '%s' is still active"),
                       def->name);
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

    if (!backend->deletePool) {
        virReportError(VIR_ERR_NO_SUPPORT,
                       "%s", _("pool does not support pool deletion"));
        goto cleanup;
    }
    if (backend->deletePool(obj, flags) < 0)
        goto cleanup;

    event = virStoragePoolEventLifecycleNew(def->name,
                                            def->uuid,
                                            VIR_STORAGE_POOL_EVENT_DELETED,
                                            0);

    ret = 0;

 cleanup:
    virObjectEventStateQueue(driver->storageEventState, event);
    virStoragePoolObjEndAPI(&obj);
    return ret;
}