storageVolLookupByName(virStoragePoolPtr pool,
                       const char *name)
{
    virStoragePoolObj *obj;
    virStoragePoolDef *def;
    virStorageVolDef *voldef;
    virStorageVolPtr vol = NULL;

    if (!(obj = virStoragePoolObjFromStoragePool(pool)))
        return NULL;
    def = virStoragePoolObjGetDef(obj);

    if (!virStoragePoolObjIsActive(obj)) {
        virReportError(VIR_ERR_OPERATION_INVALID,
                       _("storage pool '%s' is not active"), def->name);
        goto cleanup;
    }

    voldef = virStorageVolDefFindByName(obj, name);

    if (!voldef) {
        virReportError(VIR_ERR_NO_STORAGE_VOL,
                       _("no storage vol with matching name '%s'"),
                       name);
        goto cleanup;
    }

    if (virStorageVolLookupByNameEnsureACL(pool->conn, def, voldef) < 0)
        goto cleanup;

    vol = virGetStorageVol(pool->conn, def->name, voldef->name,
                           voldef->key, NULL, NULL);

 cleanup:
    virStoragePoolObjEndAPI(&obj);
    return vol;
}