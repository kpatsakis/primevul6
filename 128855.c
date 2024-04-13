libxlDomainStart(libxlDriverPrivate *driver,
                 virDomainObj *vm,
                 bool start_paused,
                 int restore_fd,
                 uint32_t restore_ver)
{
    virObjectEvent *event = NULL;

    if (libxlDomainStartPrepare(driver, vm) < 0)
        return -1;

    if (libxlDomainStartPerform(driver, vm, start_paused,
                                restore_fd, restore_ver) < 0) {
        libxlDomainCleanup(driver, vm);
        return -1;
    }

    if (g_atomic_int_add(&driver->nactive, 1) == 0 && driver->inhibitCallback)
        driver->inhibitCallback(true, driver->inhibitOpaque);

    event = virDomainEventLifecycleNewFromObj(vm, VIR_DOMAIN_EVENT_STARTED,
                                              restore_fd < 0 ?
                                              VIR_DOMAIN_EVENT_STARTED_BOOTED :
                                              VIR_DOMAIN_EVENT_STARTED_RESTORED);
    virObjectEventStateQueue(driver->domainEventState, event);

    return 0;
}