storagePoolCreateXML(virConnectPtr conn,
                     const char *xml,
                     unsigned int flags)
{
    virStoragePoolObj *obj = NULL;
    virStoragePoolDef *def;
    virStoragePoolPtr pool = NULL;
    virStorageBackend *backend;
    virObjectEvent *event = NULL;
    unsigned int build_flags = 0;
    g_autoptr(virStoragePoolDef) newDef = NULL;
    g_autofree char *stateFile = NULL;

    virCheckFlags(VIR_STORAGE_POOL_CREATE_WITH_BUILD |
                  VIR_STORAGE_POOL_CREATE_WITH_BUILD_OVERWRITE |
                  VIR_STORAGE_POOL_CREATE_WITH_BUILD_NO_OVERWRITE, NULL);

    VIR_EXCLUSIVE_FLAGS_RET(VIR_STORAGE_POOL_BUILD_OVERWRITE,
                            VIR_STORAGE_POOL_BUILD_NO_OVERWRITE, NULL);

    if (!(newDef = virStoragePoolDefParseString(xml)))
        goto cleanup;

    if (virStoragePoolCreateXMLEnsureACL(conn, newDef) < 0)
        goto cleanup;

    if ((backend = virStorageBackendForType(newDef->type)) == NULL)
        goto cleanup;

    if (!(obj = virStoragePoolObjListAdd(driver->pools, newDef,
                                         VIR_STORAGE_POOL_OBJ_LIST_ADD_LIVE |
                                         VIR_STORAGE_POOL_OBJ_LIST_ADD_CHECK_LIVE)))
        goto cleanup;
    newDef = NULL;
    def = virStoragePoolObjGetDef(obj);

    virStoragePoolObjSetStarting(obj, true);

    if (backend->buildPool) {
        if (flags & VIR_STORAGE_POOL_CREATE_WITH_BUILD_OVERWRITE)
            build_flags |= VIR_STORAGE_POOL_BUILD_OVERWRITE;
        else if (flags & VIR_STORAGE_POOL_CREATE_WITH_BUILD_NO_OVERWRITE)
            build_flags |= VIR_STORAGE_POOL_BUILD_NO_OVERWRITE;

        if (build_flags ||
            (flags & VIR_STORAGE_POOL_CREATE_WITH_BUILD)) {
            if (backend->buildPool(obj, build_flags) < 0)
                goto error;
        }
    }

    if (backend->startPool &&
        backend->startPool(obj) < 0)
        goto error;

    stateFile = virFileBuildPath(driver->stateDir, def->name, ".xml");

    if (!stateFile ||
        virStoragePoolSaveState(stateFile, def) < 0 ||
        storagePoolRefreshImpl(backend, obj, stateFile) < 0) {
        goto error;
    }

    event = virStoragePoolEventLifecycleNew(def->name,
                                            def->uuid,
                                            VIR_STORAGE_POOL_EVENT_STARTED,
                                            0);

    VIR_INFO("Creating storage pool '%s'", def->name);
    virStoragePoolObjSetActive(obj, true);

    pool = virGetStoragePool(conn, def->name, def->uuid, NULL, NULL);

 cleanup:
    if (obj && virStoragePoolObjIsStarting(obj)) {
        if (!virStoragePoolObjIsActive(obj))
            virStoragePoolUpdateInactive(obj);
        virStoragePoolObjSetStarting(obj, false);
    }
    virObjectEventStateQueue(driver->storageEventState, event);
    virStoragePoolObjEndAPI(&obj);
    return pool;

 error:
    virStoragePoolUpdateInactive(obj);
    goto cleanup;
}