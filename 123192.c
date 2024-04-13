storageVolLookupByKey(virConnectPtr conn,
                      const char *key)
{
    virStoragePoolObj *obj;
    virStoragePoolDef *def;
    struct storageVolLookupData data = {
        .key = key, .voldef = NULL };
    virStorageVolPtr vol = NULL;

    if ((obj = virStoragePoolObjListSearch(driver->pools,
                                           storageVolLookupByKeyCallback,
                                           &data)) && data.voldef) {
        def = virStoragePoolObjGetDef(obj);
        if (virStorageVolLookupByKeyEnsureACL(conn, def, data.voldef) == 0) {
            vol = virGetStorageVol(conn, def->name,
                                   data.voldef->name, data.voldef->key,
                                   NULL, NULL);
        }
        virStoragePoolObjEndAPI(&obj);
    }

    if (!vol)
        virReportError(VIR_ERR_NO_STORAGE_VOL,
                       _("no storage vol with matching key %s"), key);

    return vol;
}