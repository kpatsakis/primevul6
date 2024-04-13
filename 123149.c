storageVolDownload(virStorageVolPtr vol,
                   virStreamPtr stream,
                   unsigned long long offset,
                   unsigned long long length,
                   unsigned int flags)
{
    virStorageBackend *backend;
    virStoragePoolObj *obj = NULL;
    virStorageVolDef *voldef = NULL;
    int ret = -1;

    virCheckFlags(VIR_STORAGE_VOL_DOWNLOAD_SPARSE_STREAM, -1);

    if (!(voldef = virStorageVolDefFromVol(vol, &obj, &backend)))
        return -1;

    if (virStorageVolDownloadEnsureACL(vol->conn, virStoragePoolObjGetDef(obj),
                                       voldef) < 0)
        goto cleanup;

    if (voldef->building) {
        virReportError(VIR_ERR_OPERATION_INVALID,
                       _("volume '%s' is still being allocated."),
                       voldef->name);
        goto cleanup;
    }

    if (!backend->downloadVol) {
        virReportError(VIR_ERR_NO_SUPPORT, "%s",
                       _("storage pool doesn't support volume download"));
        goto cleanup;
    }

    virStoragePoolObjIncrAsyncjobs(obj);
    voldef->in_use++;
    virObjectUnlock(obj);

    ret = backend->downloadVol(obj, voldef, stream, offset, length, flags);

    virObjectLock(obj);
    voldef->in_use--;
    virStoragePoolObjDecrAsyncjobs(obj);

 cleanup:
    virStoragePoolObjEndAPI(&obj);

    return ret;
}