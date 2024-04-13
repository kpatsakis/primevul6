storagePoolGetXMLDesc(virStoragePoolPtr pool,
                      unsigned int flags)
{
    virStoragePoolObj *obj;
    virStoragePoolDef *def;
    virStoragePoolDef *newDef;
    virStoragePoolDef *curDef;
    char *ret = NULL;

    virCheckFlags(VIR_STORAGE_XML_INACTIVE, NULL);

    if (!(obj = virStoragePoolObjFromStoragePool(pool)))
        return NULL;
    def = virStoragePoolObjGetDef(obj);
    newDef = virStoragePoolObjGetNewDef(obj);

    if (virStoragePoolGetXMLDescEnsureACL(pool->conn, def) < 0)
        goto cleanup;

    if ((flags & VIR_STORAGE_XML_INACTIVE) && newDef)
        curDef = newDef;
    else
        curDef = def;

    ret = virStoragePoolDefFormat(curDef);

 cleanup:
    virStoragePoolObjEndAPI(&obj);
    return ret;
}