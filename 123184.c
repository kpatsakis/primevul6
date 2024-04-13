storagePoolLookupByVolume(virStorageVolPtr vol)
{
    virStoragePoolObj *obj;
    virStoragePoolDef *def;
    virStoragePoolPtr pool = NULL;

    if (!(obj = storagePoolObjFindByName(vol->pool)))
        return NULL;
    def = virStoragePoolObjGetDef(obj);

    if (virStoragePoolLookupByVolumeEnsureACL(vol->conn, def) < 0)
        goto cleanup;

    pool = virGetStoragePool(vol->conn, def->name, def->uuid, NULL, NULL);

 cleanup:
    virStoragePoolObjEndAPI(&obj);
    return pool;
}