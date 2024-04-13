storageVolLookupByPath(virConnectPtr conn,
                       const char *path)
{
    virStoragePoolObj *obj;
    virStoragePoolDef *def;
    struct storageVolLookupData data = {
        .path = path, .voldef = NULL };
    virStorageVolPtr vol = NULL;

    if (!(data.cleanpath = virFileSanitizePath(path)))
        return NULL;

    if ((obj = virStoragePoolObjListSearch(driver->pools,
                                           storageVolLookupByPathCallback,
                                           &data)) && data.voldef) {
        def = virStoragePoolObjGetDef(obj);

        if (virStorageVolLookupByPathEnsureACL(conn, def, data.voldef) == 0) {
            vol = virGetStorageVol(conn, def->name,
                                   data.voldef->name, data.voldef->key,
                                   NULL, NULL);
        }
        virStoragePoolObjEndAPI(&obj);
    }

    if (!vol) {
        if (STREQ(path, data.cleanpath)) {
            virReportError(VIR_ERR_NO_STORAGE_VOL,
                           _("no storage vol with matching path '%s'"), path);
        } else {
            virReportError(VIR_ERR_NO_STORAGE_VOL,
                           _("no storage vol with matching path '%s' (%s)"),
                           path, data.cleanpath);
        }
    }

    VIR_FREE(data.cleanpath);
    return vol;
}