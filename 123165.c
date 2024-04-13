storagePoolListVolumes(virStoragePoolPtr pool,
                       char **const names,
                       int maxnames)
{
    virStoragePoolObj *obj;
    virStoragePoolDef *def;
    int n = -1;

    if (!(obj = virStoragePoolObjFromStoragePool(pool)))
        return -1;
    def = virStoragePoolObjGetDef(obj);

    if (virStoragePoolListVolumesEnsureACL(pool->conn, def) < 0)
        goto cleanup;

    if (!virStoragePoolObjIsActive(obj)) {
        virReportError(VIR_ERR_OPERATION_INVALID,
                       _("storage pool '%s' is not active"), def->name);
        goto cleanup;
    }

    n = virStoragePoolObjVolumeGetNames(obj, pool->conn,
                                        virStoragePoolListVolumesCheckACL,
                                        names, maxnames);
 cleanup:
    virStoragePoolObjEndAPI(&obj);
    return n;
}