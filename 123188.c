storagePoolGetAutostart(virStoragePoolPtr pool,
                        int *autostart)
{
    virStoragePoolObj *obj;
    int ret = -1;

    if (!(obj = virStoragePoolObjFromStoragePool(pool)))
        return -1;

    if (virStoragePoolGetAutostartEnsureACL(pool->conn,
                                            virStoragePoolObjGetDef(obj)) < 0)
        goto cleanup;

    *autostart = virStoragePoolObjIsAutostart(obj) ? 1 : 0;

    ret = 0;

 cleanup:
    virStoragePoolObjEndAPI(&obj);
    return ret;
}