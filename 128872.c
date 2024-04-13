libxlDomainAutoCoreDump(libxlDriverPrivate *driver,
                        virDomainObj *vm)
{
    g_autoptr(libxlDriverConfig) cfg = libxlDriverConfigGet(driver);
    g_autoptr(GDateTime) now = g_date_time_new_now_local();
    g_autofree char *nowstr = NULL;
    g_autofree char *dumpfile = NULL;

    nowstr = g_date_time_format(now, "%Y-%m-%d-%H:%M:%S");

    dumpfile = g_strdup_printf("%s/%s-%s", cfg->autoDumpDir, vm->def->name,
                               nowstr);

    /* Unlock virDomainObj while dumping core */
    virObjectUnlock(vm);
    libxl_domain_core_dump(cfg->ctx, vm->def->id, dumpfile, NULL);
    virObjectLock(vm);

    return 0;
}