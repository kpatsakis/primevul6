virStorageVolDefFromVol(virStorageVolPtr vol,
                        virStoragePoolObj **obj,
                        virStorageBackend **backend)
{
    virStorageVolDef *voldef = NULL;
    virStoragePoolDef *def;

    if (!(*obj = storagePoolObjFindByName(vol->pool)))
        return NULL;
    def = virStoragePoolObjGetDef(*obj);

    if (!virStoragePoolObjIsActive(*obj)) {
        virReportError(VIR_ERR_OPERATION_INVALID,
                       _("storage pool '%s' is not active"),
                       def->name);
        goto error;
    }

    if (!(voldef = virStorageVolDefFindByName(*obj, vol->name))) {
        virReportError(VIR_ERR_NO_STORAGE_VOL,
                       _("no storage vol with matching name '%s'"),
                       vol->name);
        goto error;
    }

    if (backend) {
        if (!(*backend = virStorageBackendForType(def->type)))
            goto error;
    }

    return voldef;

 error:
    virStoragePoolObjEndAPI(obj);

    return NULL;
}