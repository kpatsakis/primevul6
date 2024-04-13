storagePoolObjFindByName(const char *name)
{
    virStoragePoolObj *obj;

    if (!(obj = virStoragePoolObjFindByName(driver->pools, name)))
        virReportError(VIR_ERR_NO_STORAGE_POOL,
                       _("no storage pool with matching name '%s'"), name);
    return obj;
}