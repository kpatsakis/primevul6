libxlDomainStartPrepare(libxlDriverPrivate *driver,
                        virDomainObj *vm)
{
    virHostdevManager *hostdev_mgr = driver->hostdevMgr;
    unsigned int hostdev_flags = VIR_HOSTDEV_SP_PCI | VIR_HOSTDEV_SP_USB;

    if (virDomainObjSetDefTransient(driver->xmlopt, vm, NULL) < 0)
        return -1;

    /* Run an early hook to set-up missing devices */
    if (libxlDomainHookRun(driver, vm->def, 0,
                           VIR_HOOK_LIBXL_OP_PREPARE,
                           VIR_HOOK_SUBOP_BEGIN, NULL) < 0)
        goto error;

    if (virDomainLockProcessStart(driver->lockManager,
                                  "xen:///system",
                                  vm,
                                  true,
                                  NULL) < 0)
        goto error;

    if (libxlNetworkPrepareDevices(vm->def) < 0)
        goto error;

    if (virHostdevPrepareDomainDevices(hostdev_mgr, LIBXL_DRIVER_INTERNAL_NAME,
                                       vm->def, hostdev_flags) < 0)
        goto error;

    return 0;

 error:
    libxlNetworkUnwindDevices(vm->def);
    virHostdevReAttachDomainDevices(hostdev_mgr, LIBXL_DRIVER_INTERNAL_NAME,
                                    vm->def, hostdev_flags);
    virDomainObjRemoveTransientDef(vm);
    return -1;
}