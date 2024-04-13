storageVolGetPath(virStorageVolPtr vol)
{
    virStoragePoolObj *obj;
    virStorageVolDef *voldef;
    char *ret = NULL;

    if (!(voldef = virStorageVolDefFromVol(vol, &obj, NULL)))
        return NULL;

    if (virStorageVolGetPathEnsureACL(vol->conn, virStoragePoolObjGetDef(obj),
                                      voldef) < 0)
        goto cleanup;

    ret = g_strdup(voldef->target.path);

 cleanup:
    virStoragePoolObjEndAPI(&obj);
    return ret;
}