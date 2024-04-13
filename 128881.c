libxlDomainUpdateDiskParams(virDomainDef *def, libxl_ctx *ctx)
{
    libxl_device_disk *disks;
    int num_disks = 0;
    size_t i;
    int idx;

    disks = libxl_device_disk_list(ctx, def->id, &num_disks);
    if (!disks)
        return;

    for (i = 0; i < num_disks; i++) {
        if ((idx = virDomainDiskIndexByName(def, disks[i].vdev, false)) < 0)
            continue;

        libxlUpdateDiskDef(def->disks[idx], &disks[i]);
    }

    for (i = 0; i < num_disks; i++)
        libxl_device_disk_dispose(&disks[i]);
    VIR_FREE(disks);
}