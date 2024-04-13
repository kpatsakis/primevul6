storageVolDeleteInternal(virStorageBackend *backend,
                         virStoragePoolObj *obj,
                         virStorageVolDef *voldef,
                         unsigned int flags,
                         bool updateMeta)
{
    virStoragePoolDef *def = virStoragePoolObjGetDef(obj);

    if (!backend->deleteVol) {
        virReportError(VIR_ERR_NO_SUPPORT,
                       "%s", _("storage pool does not support vol deletion"));

        return -1;
    }

    if (backend->deleteVol(obj, voldef, flags) < 0)
        return -1;

    /* The disk backend updated the pool data including removing the
     * voldef from the pool (for both the deleteVol and the createVol
     * failure path. */
    if (def->type == VIR_STORAGE_POOL_DISK)
        return 0;

    /* Update pool metadata - don't update meta data from error paths
     * in this module since the allocation/available weren't adjusted yet.
     * Ignore the disk backend since it updates the pool values.
     */
    if (updateMeta) {
        def->allocation -= voldef->target.allocation;
        def->available += voldef->target.allocation;
    }

    virStoragePoolObjRemoveVol(obj, voldef);

    return 0;
}