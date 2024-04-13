storagePoolDefineXML(virConnectPtr conn,
                     const char *xml,
                     unsigned int flags)
{
    virStoragePoolObj *obj = NULL;
    virStoragePoolDef *def;
    virStoragePoolPtr pool = NULL;
    virObjectEvent *event = NULL;
    g_autoptr(virStoragePoolDef) newDef = NULL;

    virCheckFlags(0, NULL);

    if (!(newDef = virStoragePoolDefParseString(xml)))
        goto cleanup;

    if (virXMLCheckIllegalChars("name", newDef->name, "\n") < 0)
        goto cleanup;

    if (virStoragePoolDefineXMLEnsureACL(conn, newDef) < 0)
        goto cleanup;

    if (virStorageBackendForType(newDef->type) == NULL)
        goto cleanup;

    if (!(obj = virStoragePoolObjListAdd(driver->pools, newDef, 0)))
        goto cleanup;
    newDef = virStoragePoolObjGetNewDef(obj);
    def = virStoragePoolObjGetDef(obj);

    if (virStoragePoolObjSaveDef(driver, obj, newDef ? newDef : def) < 0) {
        virStoragePoolObjRemove(driver->pools, obj);
        newDef = NULL;
        goto cleanup;
    }

    event = virStoragePoolEventLifecycleNew(def->name, def->uuid,
                                            VIR_STORAGE_POOL_EVENT_DEFINED,
                                            0);

    VIR_INFO("Defining storage pool '%s'", def->name);
    pool = virGetStoragePool(conn, def->name, def->uuid, NULL, NULL);
    newDef = NULL;

 cleanup:
    virObjectEventStateQueue(driver->storageEventState, event);
    virStoragePoolObjEndAPI(&obj);
    return pool;
}