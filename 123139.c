storageVolGetXMLDesc(virStorageVolPtr vol,
                     unsigned int flags)
{
    virStoragePoolObj *obj;
    virStoragePoolDef *def;
    virStorageBackend *backend;
    virStorageVolDef *voldef;
    char *ret = NULL;

    virCheckFlags(0, NULL);

    if (!(voldef = virStorageVolDefFromVol(vol, &obj, &backend)))
        return NULL;
    def = virStoragePoolObjGetDef(obj);

    if (virStorageVolGetXMLDescEnsureACL(vol->conn, def, voldef) < 0)
        goto cleanup;

    if (backend->refreshVol &&
        backend->refreshVol(obj, voldef) < 0)
        goto cleanup;

    ret = virStorageVolDefFormat(def, voldef);

 cleanup:
    virStoragePoolObjEndAPI(&obj);

    return ret;
}