virStoragePoolObjFromStoragePool(virStoragePoolPtr pool)
{
    return storagePoolObjFindByUUID(pool->uuid, pool->name);
}