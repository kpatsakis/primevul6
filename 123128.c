storageVolLookupByPathCallback(virStoragePoolObj *obj,
                               const void *opaque)
{
    struct storageVolLookupData *data = (struct storageVolLookupData *)opaque;
    virStoragePoolDef *def;
    g_autofree char *stable_path = NULL;

    if (!virStoragePoolObjIsActive(obj))
        return false;

    def = virStoragePoolObjGetDef(obj);

    switch ((virStoragePoolType)def->type) {
        case VIR_STORAGE_POOL_DIR:
        case VIR_STORAGE_POOL_FS:
        case VIR_STORAGE_POOL_NETFS:
        case VIR_STORAGE_POOL_LOGICAL:
        case VIR_STORAGE_POOL_DISK:
        case VIR_STORAGE_POOL_ISCSI:
        case VIR_STORAGE_POOL_ISCSI_DIRECT:
        case VIR_STORAGE_POOL_SCSI:
        case VIR_STORAGE_POOL_MPATH:
        case VIR_STORAGE_POOL_VSTORAGE:
            stable_path = virStorageBackendStablePath(obj, data->cleanpath,
                                                      false);
            break;

        case VIR_STORAGE_POOL_GLUSTER:
        case VIR_STORAGE_POOL_RBD:
        case VIR_STORAGE_POOL_SHEEPDOG:
        case VIR_STORAGE_POOL_ZFS:
        case VIR_STORAGE_POOL_LAST:
            stable_path = g_strdup(data->path);
            break;
    }

    /* Don't break the whole lookup process if it fails on
     * getting the stable path for some of the pools. */
    if (!stable_path) {
        VIR_WARN("Failed to get stable path for pool '%s'", def->name);
        return false;
    }

    data->voldef = virStorageVolDefFindByPath(obj, stable_path);

    return !!data->voldef;
}