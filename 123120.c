storageVolResize(virStorageVolPtr vol,
                 unsigned long long capacity,
                 unsigned int flags)
{
    virStorageBackend *backend;
    virStoragePoolObj *obj = NULL;
    virStoragePoolDef *def;
    virStorageVolDef *voldef = NULL;
    unsigned long long abs_capacity, delta = 0;
    int ret = -1;

    virCheckFlags(VIR_STORAGE_VOL_RESIZE_ALLOCATE |
                  VIR_STORAGE_VOL_RESIZE_DELTA |
                  VIR_STORAGE_VOL_RESIZE_SHRINK, -1);

    if (!(voldef = virStorageVolDefFromVol(vol, &obj, &backend)))
        return -1;
    def = virStoragePoolObjGetDef(obj);

    if (virStorageVolResizeEnsureACL(vol->conn, def, voldef) < 0)
        goto cleanup;

    if (voldef->in_use) {
        virReportError(VIR_ERR_OPERATION_INVALID,
                       _("volume '%s' is still in use."),
                       voldef->name);
        goto cleanup;
    }

    if (voldef->building) {
        virReportError(VIR_ERR_OPERATION_INVALID,
                       _("volume '%s' is still being allocated."),
                       voldef->name);
        goto cleanup;
    }

    if (flags & VIR_STORAGE_VOL_RESIZE_DELTA) {
        if (flags & VIR_STORAGE_VOL_RESIZE_SHRINK)
            abs_capacity = voldef->target.capacity - MIN(capacity, voldef->target.capacity);
        else
            abs_capacity = voldef->target.capacity + capacity;
        flags &= ~VIR_STORAGE_VOL_RESIZE_DELTA;
    } else {
        abs_capacity = capacity;
    }

    if (abs_capacity < voldef->target.allocation) {
        virReportError(VIR_ERR_INVALID_ARG, "%s",
                       _("can't shrink capacity below "
                         "existing allocation"));
        goto cleanup;
    }

    if (abs_capacity < voldef->target.capacity &&
        !(flags & VIR_STORAGE_VOL_RESIZE_SHRINK)) {
        virReportError(VIR_ERR_INVALID_ARG, "%s",
                       _("Can't shrink capacity below current "
                         "capacity unless shrink flag explicitly specified"));
        goto cleanup;
    }

    if (flags & VIR_STORAGE_VOL_RESIZE_ALLOCATE)
        delta = abs_capacity - voldef->target.allocation;

    if (delta > def->available) {
        virReportError(VIR_ERR_OPERATION_FAILED, "%s",
                       _("Not enough space left in storage pool"));
        goto cleanup;
    }

    if (!backend->resizeVol) {
        virReportError(VIR_ERR_NO_SUPPORT, "%s",
                       _("storage pool does not support changing of "
                         "volume capacity"));
        goto cleanup;
    }

    if (backend->resizeVol(obj, voldef, abs_capacity, flags) < 0)
        goto cleanup;

    voldef->target.capacity = abs_capacity;
    /* Only update the allocation and pool values if we actually did the
     * allocation; otherwise, this is akin to a create operation with a
     * capacity value different and potentially much larger than available
     */
    if (flags & VIR_STORAGE_VOL_RESIZE_ALLOCATE) {
        voldef->target.allocation = abs_capacity;
        def->allocation += delta;
        def->available -= delta;
    }

    ret = 0;

 cleanup:
    virStoragePoolObjEndAPI(&obj);

    return ret;
}