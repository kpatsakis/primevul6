libxlDomainDeviceDefPostParse(virDomainDeviceDef *dev,
                              const virDomainDef *def,
                              unsigned int parseFlags G_GNUC_UNUSED,
                              void *opaque G_GNUC_UNUSED,
                              void *parseOpaque G_GNUC_UNUSED)
{
    if (dev->type == VIR_DOMAIN_DEVICE_CHR &&
        dev->data.chr->deviceType == VIR_DOMAIN_CHR_DEVICE_TYPE_CONSOLE &&
        dev->data.chr->targetType == VIR_DOMAIN_CHR_CONSOLE_TARGET_TYPE_NONE &&
        def->os.type != VIR_DOMAIN_OSTYPE_HVM)
        dev->data.chr->targetType = VIR_DOMAIN_CHR_CONSOLE_TARGET_TYPE_XEN;

    if (dev->type == VIR_DOMAIN_DEVICE_HOSTDEV ||
        (dev->type == VIR_DOMAIN_DEVICE_NET &&
         dev->data.net->type == VIR_DOMAIN_NET_TYPE_HOSTDEV)) {

        virDomainHostdevDef *hostdev;
        virDomainHostdevSubsysPCI *pcisrc;

        if (dev->type == VIR_DOMAIN_DEVICE_NET)
            hostdev = &dev->data.net->data.hostdev.def;
        else
            hostdev = dev->data.hostdev;
        pcisrc = &hostdev->source.subsys.u.pci;

        /* forbid capabilities mode hostdev in this kind of hypervisor */
        if (hostdev->mode == VIR_DOMAIN_HOSTDEV_MODE_CAPABILITIES) {
            virReportError(VIR_ERR_CONFIG_UNSUPPORTED,
                           _("hostdev mode 'capabilities' is not "
                             "supported in %s"),
                           virDomainVirtTypeToString(def->virtType));
            return -1;
        }

        if (hostdev->mode == VIR_DOMAIN_HOSTDEV_MODE_SUBSYS &&
            hostdev->source.subsys.type == VIR_DOMAIN_HOSTDEV_SUBSYS_TYPE_PCI &&
            pcisrc->backend == VIR_DOMAIN_HOSTDEV_PCI_BACKEND_DEFAULT)
            pcisrc->backend = VIR_DOMAIN_HOSTDEV_PCI_BACKEND_XEN;
    }

    if (dev->type == VIR_DOMAIN_DEVICE_VIDEO) {
        if (dev->data.video->type == VIR_DOMAIN_VIDEO_TYPE_DEFAULT) {
            if (def->os.type == VIR_DOMAIN_OSTYPE_XEN ||
                def->os.type == VIR_DOMAIN_OSTYPE_LINUX)
                dev->data.video->type = VIR_DOMAIN_VIDEO_TYPE_XEN;
            else if (ARCH_IS_PPC64(def->os.arch))
                dev->data.video->type = VIR_DOMAIN_VIDEO_TYPE_VGA;
            else
                dev->data.video->type = VIR_DOMAIN_VIDEO_TYPE_CIRRUS;
        }

        if (def->os.type == VIR_DOMAIN_OSTYPE_HVM) {
            int dm_type = libxlDomainGetEmulatorType(def);

            switch (dev->data.video->type) {
                case VIR_DOMAIN_VIDEO_TYPE_VGA:
                case VIR_DOMAIN_VIDEO_TYPE_XEN:
                    if (dev->data.video->vram == 0) {
                        if (dm_type == LIBXL_DEVICE_MODEL_VERSION_QEMU_XEN)
                            dev->data.video->vram = 16 * 1024;
                        else
                            dev->data.video->vram = 8 * 1024;
                    }
                    break;
                case VIR_DOMAIN_VIDEO_TYPE_CIRRUS:
                    if (dev->data.video->vram == 0) {
                        if (dm_type == LIBXL_DEVICE_MODEL_VERSION_QEMU_XEN)
                            dev->data.video->vram = 8 * 1024;
                        else
                            dev->data.video->vram = 4 * 1024;
                    }
                    break;
                case VIR_DOMAIN_VIDEO_TYPE_QXL:
                    if (dev->data.video->vram == 0)
                        dev->data.video->vram = 128 * 1024;
                    break;
            }
        }
    }

    if (dev->type == VIR_DOMAIN_DEVICE_DISK) {
        virDomainDiskDef *disk = dev->data.disk;
        int actual_type = virStorageSourceGetActualType(disk->src);
        int format = virDomainDiskGetFormat(disk);

        /* for network-based disks, set 'qemu' as the default driver */
        if (actual_type == VIR_STORAGE_TYPE_NETWORK) {
            if (!virDomainDiskGetDriver(disk))
                virDomainDiskSetDriver(disk, "qemu");
        }

        /* xl.cfg default format is raw. See xl-disk-configuration(5) */
        if (format == VIR_STORAGE_FILE_NONE)
            virDomainDiskSetFormat(disk, VIR_STORAGE_FILE_RAW);
    }

    return 0;
}