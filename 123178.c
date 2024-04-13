storagePoolLookupByName(virConnectPtr conn,
                        const char *name)
{
    virStoragePoolObj *obj;
    virStoragePoolDef *def;
    virStoragePoolPtr pool = NULL;

    if (!(obj = storagePoolObjFindByName(name)))
        return NULL;
    def = virStoragePoolObjGetDef(obj);

    if (virStoragePoolLookupByNameEnsureACL(conn, def) < 0)
        goto cleanup;

    pool = virGetStoragePool(conn, def->name, def->uuid, NULL, NULL);

 cleanup:
    virStoragePoolObjEndAPI(&obj);
    return pool;
}