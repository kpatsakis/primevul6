storageVolWipePattern(virStorageVolPtr vol,
                      unsigned int algorithm,
                      unsigned int flags)
{
    virStorageBackend *backend;
    virStoragePoolObj *obj = NULL;
    virStorageVolDef *voldef = NULL;
    int rc;
    int ret = -1;

    virCheckFlags(0, -1);

    if (algorithm >= VIR_STORAGE_VOL_WIPE_ALG_LAST) {
        virReportError(VIR_ERR_INVALID_ARG,
                       _("wiping algorithm %d not supported"),
                       algorithm);
        return -1;
    }

    if (!(voldef = virStorageVolDefFromVol(vol, &obj, &backend)))
        return -1;

    if (virStorageVolWipePatternEnsureACL(vol->conn,
                                          virStoragePoolObjGetDef(obj),
                                          voldef) < 0)
        goto cleanup;

    if (voldef->in_use) {
        virReportError(VIR_ERR_OPERATION_INVALID,
                       _("volume '%s' is still in use."),
                       voldef->name);
        goto cleanup;
    }

    if (voldef->building) {
        virReportError(VIR_ERR_OPERATION_INVALID,
                       _("volume '%s' is still being allocated."),
                       voldef->name);
        goto cleanup;
    }

    if (!backend->wipeVol) {
        virReportError(VIR_ERR_NO_SUPPORT, "%s",
                       _("storage pool doesn't support volume wiping"));
        goto cleanup;
    }

    virStoragePoolObjIncrAsyncjobs(obj);
    voldef->in_use++;
    virObjectUnlock(obj);

    rc = backend->wipeVol(obj, voldef, algorithm, flags);

    virObjectLock(obj);
    voldef->in_use--;
    virStoragePoolObjDecrAsyncjobs(obj);

    if (rc < 0)
        goto cleanup;

    /* For local volumes, Instead of using the refreshVol, since
     * much changes on the target volume, let's update using the
     * same function as refreshPool would use when it discovers a
     * volume. The only failure to capture is -1, we can ignore
     * -2. */
    if ((backend->type == VIR_STORAGE_POOL_DIR ||
         backend->type == VIR_STORAGE_POOL_FS ||
         backend->type == VIR_STORAGE_POOL_NETFS ||
         backend->type == VIR_STORAGE_POOL_VSTORAGE) &&
        virStorageBackendRefreshVolTargetUpdate(voldef) == -1)
        goto cleanup;

    ret = 0;

 cleanup:
    virStoragePoolObjEndAPI(&obj);

    return ret;
}