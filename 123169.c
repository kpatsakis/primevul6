storagePoolLookupByUUID(virConnectPtr conn,
                        const unsigned char *uuid)
{
    virStoragePoolObj *obj;
    virStoragePoolDef *def;
    virStoragePoolPtr pool = NULL;

    obj = storagePoolObjFindByUUID(uuid, NULL);
    if (!obj)
        return NULL;
    def = virStoragePoolObjGetDef(obj);

    if (virStoragePoolLookupByUUIDEnsureACL(conn, def) < 0)
        goto cleanup;

    pool = virGetStoragePool(conn, def->name, def->uuid, NULL, NULL);

 cleanup:
    virStoragePoolObjEndAPI(&obj);
    return pool;
}