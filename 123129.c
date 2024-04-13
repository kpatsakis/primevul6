storagePoolIsPersistent(virStoragePoolPtr pool)
{
    virStoragePoolObj *obj;
    virStoragePoolDef *def;
    int ret = -1;

    if (!(obj = virStoragePoolObjFromStoragePool(pool)))
        return -1;
    def = virStoragePoolObjGetDef(obj);

    if (virStoragePoolIsPersistentEnsureACL(pool->conn, def) < 0)
        goto cleanup;

    ret = virStoragePoolObjGetConfigFile(obj) ? 1 : 0;

 cleanup:
    virStoragePoolObjEndAPI(&obj);
    return ret;
}