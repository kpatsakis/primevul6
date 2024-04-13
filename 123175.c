storageVolDelete(virStorageVolPtr vol,
                 unsigned int flags)
{
    virStoragePoolObj *obj;
    virStorageBackend *backend;
    virStorageVolDef *voldef = NULL;
    int ret = -1;

    if (!(voldef = virStorageVolDefFromVol(vol, &obj, &backend)))
        return -1;

    if (virStorageVolDeleteEnsureACL(vol->conn, virStoragePoolObjGetDef(obj),
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

    if (storageVolDeleteInternal(backend, obj, voldef, flags, true) < 0)
        goto cleanup;

    ret = 0;

 cleanup:
    virStoragePoolObjEndAPI(&obj);
    return ret;
}