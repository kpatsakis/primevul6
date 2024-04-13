libxlDomainDefValidate(const virDomainDef *def,
                       void *opaque,
                       void *parseOpaque G_GNUC_UNUSED)
{
    libxlDriverPrivate *driver = opaque;
    g_autoptr(libxlDriverConfig) cfg = libxlDriverConfigGet(driver);
    bool reqSecureBoot = false;

    if (!virCapabilitiesDomainSupported(cfg->caps, def->os.type,
                                        def->os.arch,
                                        def->virtType))
        return -1;

    /* Xen+ovmf does not support secure boot */
    if (def->os.firmware == VIR_DOMAIN_OS_DEF_FIRMWARE_EFI) {
        if (def->os.firmwareFeatures &&
            def->os.firmwareFeatures[VIR_DOMAIN_OS_DEF_FIRMWARE_FEATURE_SECURE_BOOT])
            reqSecureBoot = true;
    }
    if (virDomainDefHasOldStyleUEFI(def)) {
        if (def->os.loader &&
            def->os.loader->secure == VIR_TRISTATE_BOOL_YES)
            reqSecureBoot = true;
    }
    if (reqSecureBoot) {
        virReportError(VIR_ERR_CONFIG_UNSUPPORTED, "%s",
                       _("Secure boot is not supported on Xen"));
        return -1;
    }

    return 0;
}