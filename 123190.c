storagePoolGetInfo(virStoragePoolPtr pool,
                   virStoragePoolInfoPtr info)
{
    virStoragePoolObj *obj;
    virStoragePoolDef *def;
    int ret = -1;

    if (!(obj = virStoragePoolObjFromStoragePool(pool)))
        return -1;
    def = virStoragePoolObjGetDef(obj);

    if (virStoragePoolGetInfoEnsureACL(pool->conn, def) < 0)
        goto cleanup;

    if (virStorageBackendForType(def->type) == NULL)
        goto cleanup;

    memset(info, 0, sizeof(virStoragePoolInfo));
    if (virStoragePoolObjIsActive(obj))
        info->state = VIR_STORAGE_POOL_RUNNING;
    else
        info->state = VIR_STORAGE_POOL_INACTIVE;
    info->capacity = def->capacity;
    info->allocation = def->allocation;
    info->available = def->available;
    ret = 0;

 cleanup:
    virStoragePoolObjEndAPI(&obj);
    return ret;
}