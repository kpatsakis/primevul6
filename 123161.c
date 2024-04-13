storagePoolObjFindByUUID(const unsigned char *uuid,
                         const char *name)
{
    virStoragePoolObj *obj;
    char uuidstr[VIR_UUID_STRING_BUFLEN];

    if (!(obj = virStoragePoolObjFindByUUID(driver->pools, uuid))) {
        virUUIDFormat(uuid, uuidstr);
        if (name)
            virReportError(VIR_ERR_NO_STORAGE_POOL,
                           _("no storage pool with matching uuid '%s' (%s)"),
                           uuidstr, name);
        else
            virReportError(VIR_ERR_NO_STORAGE_POOL,
                           _("no storage pool with matching uuid '%s'"),
                           uuidstr);
    }

    return obj;
}