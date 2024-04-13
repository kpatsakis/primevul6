storageVolUpload(virStorageVolPtr vol,
                 virStreamPtr stream,
                 unsigned long long offset,
                 unsigned long long length,
                 unsigned int flags)
{
    virStorageBackend *backend;
    virStoragePoolObj *obj = NULL;
    virStoragePoolDef *def;
    virStorageVolDef *voldef = NULL;
    virStorageVolStreamInfo *cbdata = NULL;
    int rc;
    int ret = -1;

    virCheckFlags(VIR_STORAGE_VOL_UPLOAD_SPARSE_STREAM, -1);

    if (!(voldef = virStorageVolDefFromVol(vol, &obj, &backend)))
        return -1;
    def = virStoragePoolObjGetDef(obj);

    if (virStorageVolUploadEnsureACL(vol->conn, def, voldef) < 0)
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

    if (!backend->uploadVol) {
        virReportError(VIR_ERR_NO_SUPPORT, "%s",
                       _("storage pool doesn't support volume upload"));
        goto cleanup;
    }

    /* Use the callback routine in order to
     * refresh the pool after the volume upload stream closes. This way
     * we make sure the volume and pool data are refreshed without user
     * interaction and we can just lookup the backend in the callback
     * routine in order to call the refresh API.
     */
    cbdata = g_new0(virStorageVolStreamInfo, 1);
    cbdata->pool_name = g_strdup(def->name);
    if (voldef->type == VIR_STORAGE_VOL_PLOOP)
        cbdata->vol_path = g_strdup(voldef->target.path);

    virStoragePoolObjIncrAsyncjobs(obj);
    voldef->in_use++;
    virObjectUnlock(obj);

    rc = backend->uploadVol(obj, voldef, stream, offset, length, flags);

    virObjectLock(obj);
    voldef->in_use--;
    virStoragePoolObjDecrAsyncjobs(obj);

    if (rc < 0)
        goto cleanup;

    /* Add cleanup callback - call after uploadVol since the stream
     * is then fully set up
     */
    virFDStreamSetInternalCloseCb(stream,
                                  virStorageVolFDStreamCloseCb,
                                  cbdata, NULL);
    cbdata = NULL;
    ret = 0;
 cleanup:
    virStoragePoolObjEndAPI(&obj);
    if (cbdata)
        virStorageVolPoolRefreshDataFree(cbdata);

    return ret;
}