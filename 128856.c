libxlDomainManagedSavePath(libxlDriverPrivate *driver, virDomainObj *vm)
{
    char *ret;
    g_autoptr(libxlDriverConfig) cfg = libxlDriverConfigGet(driver);

    ret = g_strdup_printf("%s/%s.save", cfg->saveDir, vm->def->name);
    return ret;
}