storagePoolNumOfVolumes(virStoragePoolPtr pool)
{
    virStoragePoolObj *obj;
    virStoragePoolDef *def;
    int ret = -1;

    if (!(obj = virStoragePoolObjFromStoragePool(pool)))
        return -1;
    def = virStoragePoolObjGetDef(obj);

    if (virStoragePoolNumOfVolumesEnsureACL(pool->conn, def) < 0)
        goto cleanup;

    if (!virStoragePoolObjIsActive(obj)) {
        virReportError(VIR_ERR_OPERATION_INVALID,
                       _("storage pool '%s' is not active"), def->name);
        goto cleanup;
    }

    ret = virStoragePoolObjNumOfVolumes(obj, pool->conn,
                                        virStoragePoolNumOfVolumesCheckACL);

 cleanup:
    virStoragePoolObjEndAPI(&obj);
    return ret;
}