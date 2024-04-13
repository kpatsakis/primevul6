libxlDomainHookRun(libxlDriverPrivate *driver,
                   virDomainDef *def,
                   unsigned int def_fmtflags,
                   int hookop,
                   int hooksubop,
                   char **output)
{
    g_autofree char *xml = NULL;

    if (!virHookPresent(VIR_HOOK_DRIVER_LIBXL))
        return 0;

    xml = virDomainDefFormat(def, driver->xmlopt, def_fmtflags);
    return virHookCall(VIR_HOOK_DRIVER_LIBXL, def->name,
                       hookop, hooksubop,
                       NULL, xml, output);
}