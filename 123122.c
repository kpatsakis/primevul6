storagePoolLookupByTargetPathCallback(virStoragePoolObj *obj,
                                      const void *opaque)
{
    const char *path = opaque;
    virStoragePoolDef *def;

    if (!virStoragePoolObjIsActive(obj))
        return false;

    def = virStoragePoolObjGetDef(obj);
    return STREQ_NULLABLE(path, def->target.path);
}