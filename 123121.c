storagePoolIsActive(virStoragePoolPtr pool)
{
    virStoragePoolObj *obj;
    virStoragePoolDef *def;
    int ret = -1;

    if (!(obj = virStoragePoolObjFromStoragePool(pool)))
        return -1;
    def = virStoragePoolObjGetDef(obj);

    if (virStoragePoolIsActiveEnsureACL(pool->conn, def) < 0)
        goto cleanup;

    ret = virStoragePoolObjIsActive(obj);

 cleanup:
    virStoragePoolObjEndAPI(&obj);
    return ret;
}