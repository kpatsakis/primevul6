libxlDomainShutdownHandleDestroy(libxlDriverPrivate *driver,
                                 virDomainObj *vm)
{
    libxlDomainDestroyInternal(driver, vm);
    libxlDomainCleanup(driver, vm);
    if (!vm->persistent)
        virDomainObjListRemove(driver->domains, vm);
}