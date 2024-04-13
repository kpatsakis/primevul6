storagePoolUndefine(virStoragePoolPtr pool)
{
    virStoragePoolObj *obj;
    virStoragePoolDef *def;
    const char *autostartLink;
    virObjectEvent *event = NULL;
    int ret = -1;

    if (!(obj = storagePoolObjFindByUUID(pool->uuid, pool->name)))
        goto cleanup;
    def = virStoragePoolObjGetDef(obj);

    if (virStoragePoolUndefineEnsureACL(pool->conn, def) < 0)
        goto cleanup;

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

    autostartLink = virStoragePoolObjGetAutostartLink(obj);
    if (virStoragePoolObjDeleteDef(obj) < 0)
        goto cleanup;

    if (autostartLink && unlink(autostartLink) < 0 &&
        errno != ENOENT && errno != ENOTDIR) {
        VIR_ERROR(_("Failed to delete autostart link '%s': %s"),
                  autostartLink, g_strerror(errno));
    }

    event = virStoragePoolEventLifecycleNew(def->name,
                                            def->uuid,
                                            VIR_STORAGE_POOL_EVENT_UNDEFINED,
                                            0);

    VIR_INFO("Undefining storage pool '%s'", def->name);
    virStoragePoolObjRemove(driver->pools, obj);
    ret = 0;

 cleanup:
    virObjectEventStateQueue(driver->storageEventState, event);
    virStoragePoolObjEndAPI(&obj);
    return ret;
}