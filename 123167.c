storageVolGetInfoFlags(virStorageVolPtr vol,
                       virStorageVolInfoPtr info,
                       unsigned int flags)
{
    virStoragePoolObj *obj;
    virStorageBackend *backend;
    virStorageVolDef *voldef;
    int ret = -1;

    virCheckFlags(VIR_STORAGE_VOL_GET_PHYSICAL, -1);

    if (!(voldef = virStorageVolDefFromVol(vol, &obj, &backend)))
        return -1;

    if (virStorageVolGetInfoFlagsEnsureACL(vol->conn,
                                           virStoragePoolObjGetDef(obj),
                                           voldef) < 0)
        goto cleanup;

    if (backend->refreshVol &&
        backend->refreshVol(obj, voldef) < 0)
        goto cleanup;

    memset(info, 0, sizeof(*info));
    info->type = voldef->type;
    info->capacity = voldef->target.capacity;
    if (flags & VIR_STORAGE_VOL_GET_PHYSICAL)
        info->allocation = voldef->target.physical;
    else
        info->allocation = voldef->target.allocation;
    ret = 0;

 cleanup:
    virStoragePoolObjEndAPI(&obj);
    return ret;
}