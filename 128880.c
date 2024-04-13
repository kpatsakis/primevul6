libxlDomainShutdownHandleRestart(libxlDriverPrivate *driver,
                                 virDomainObj *vm)
{
    libxlDomainDestroyInternal(driver, vm);
    libxlDomainCleanup(driver, vm);
    if (libxlDomainStartNew(driver, vm, false) < 0) {
        VIR_ERROR(_("Failed to restart VM '%s': %s"),
                  vm->def->name, virGetLastErrorMessage());
    }
}