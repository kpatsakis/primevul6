storagePoolListAllVolumes(virStoragePoolPtr pool,
                          virStorageVolPtr **vols,
                          unsigned int flags)
{
    virStoragePoolObj *obj;
    virStoragePoolDef *def;
    int ret = -1;

    virCheckFlags(0, -1);

    if (!(obj = virStoragePoolObjFromStoragePool(pool)))
        return -1;
    def = virStoragePoolObjGetDef(obj);

    if (virStoragePoolListAllVolumesEnsureACL(pool->conn, def) < 0)
        goto cleanup;

    if (!virStoragePoolObjIsActive(obj)) {
        virReportError(VIR_ERR_OPERATION_INVALID,
                       _("storage pool '%s' is not active"), def->name);
        goto cleanup;
    }

    ret = virStoragePoolObjVolumeListExport(pool->conn, obj, vols,
                                            virStoragePoolListAllVolumesCheckACL);


 cleanup:
    virStoragePoolObjEndAPI(&obj);

    return ret;
}