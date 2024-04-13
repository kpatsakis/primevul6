storagePoolCreate(virStoragePoolPtr pool,
                  unsigned int flags)
{
    virStoragePoolObj *obj;
    virStoragePoolDef *def;
    virStorageBackend *backend;
    virObjectEvent *event = NULL;
    int ret = -1;
    unsigned int build_flags = 0;
    g_autofree char *stateFile = NULL;
    bool restoreStarting = false;

    virCheckFlags(VIR_STORAGE_POOL_CREATE_WITH_BUILD |
                  VIR_STORAGE_POOL_CREATE_WITH_BUILD_OVERWRITE |
                  VIR_STORAGE_POOL_CREATE_WITH_BUILD_NO_OVERWRITE, -1);

    VIR_EXCLUSIVE_FLAGS_RET(VIR_STORAGE_POOL_BUILD_OVERWRITE,
                            VIR_STORAGE_POOL_BUILD_NO_OVERWRITE, -1);

    if (!(obj = virStoragePoolObjFromStoragePool(pool)))
        return -1;
    def = virStoragePoolObjGetDef(obj);

    if (virStoragePoolCreateEnsureACL(pool->conn, def) < 0)
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

    if (backend->buildPool) {
        if (flags & VIR_STORAGE_POOL_CREATE_WITH_BUILD_OVERWRITE)
            build_flags |= VIR_STORAGE_POOL_BUILD_OVERWRITE;
        else if (flags & VIR_STORAGE_POOL_CREATE_WITH_BUILD_NO_OVERWRITE)
            build_flags |= VIR_STORAGE_POOL_BUILD_NO_OVERWRITE;

        if (build_flags ||
            (flags & VIR_STORAGE_POOL_CREATE_WITH_BUILD)) {
            if (backend->buildPool(obj, build_flags) < 0)
                goto cleanup;
        }
    }

    VIR_INFO("Starting up storage pool '%s'", def->name);
    if (backend->startPool &&
        backend->startPool(obj) < 0)
        goto cleanup;

    stateFile = virFileBuildPath(driver->stateDir, def->name, ".xml");

    if (!stateFile ||
        virStoragePoolSaveState(stateFile, def) < 0 ||
        storagePoolRefreshImpl(backend, obj, stateFile) < 0) {
        goto cleanup;
    }

    event = virStoragePoolEventLifecycleNew(def->name,
                                            def->uuid,
                                            VIR_STORAGE_POOL_EVENT_STARTED,
                                            0);

    virStoragePoolObjSetActive(obj, true);
    ret = 0;

 cleanup:
    if (restoreStarting &&
        virStoragePoolObjIsStarting(obj)) {
        if (!virStoragePoolObjIsActive(obj))
            virStoragePoolUpdateInactive(obj);
        virStoragePoolObjSetStarting(obj, false);
    }
    virObjectEventStateQueue(driver->storageEventState, event);
    virStoragePoolObjEndAPI(&obj);
    return ret;
}