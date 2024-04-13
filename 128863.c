libxlDomainSaveImageOpen(libxlDriverPrivate *driver,
                         const char *from,
                         virDomainDef **ret_def,
                         libxlSavefileHeader *ret_hdr)
{
    int fd;
    virDomainDef *def = NULL;
    libxlSavefileHeader hdr;
    g_autofree char *xml = NULL;

    if ((fd = virFileOpenAs(from, O_RDONLY, 0, -1, -1, 0)) < 0) {
        virReportSystemError(-fd,
                             _("Failed to open domain image file '%s'"), from);
        goto error;
    }

    if (saferead(fd, &hdr, sizeof(hdr)) != sizeof(hdr)) {
        virReportError(VIR_ERR_OPERATION_FAILED,
                       "%s", _("failed to read libxl header"));
        goto error;
    }

    if (memcmp(hdr.magic, LIBXL_SAVE_MAGIC, sizeof(hdr.magic))) {
        virReportError(VIR_ERR_INVALID_ARG, "%s", _("image magic is incorrect"));
        goto error;
    }

    if (hdr.version > LIBXL_SAVE_VERSION) {
        virReportError(VIR_ERR_OPERATION_FAILED,
                       _("image version is not supported (%d > %d)"),
                       hdr.version, LIBXL_SAVE_VERSION);
        goto error;
    }

    if (hdr.xmlLen <= 0) {
        virReportError(VIR_ERR_OPERATION_FAILED,
                       _("invalid XML length: %d"), hdr.xmlLen);
        goto error;
    }

    xml = g_new0(char, hdr.xmlLen);

    if (saferead(fd, xml, hdr.xmlLen) != hdr.xmlLen) {
        virReportError(VIR_ERR_OPERATION_FAILED, "%s", _("failed to read XML"));
        goto error;
    }

    if (!(def = virDomainDefParseString(xml, driver->xmlopt, NULL,
                                        VIR_DOMAIN_DEF_PARSE_INACTIVE |
                                        VIR_DOMAIN_DEF_PARSE_SKIP_VALIDATE)))
        goto error;

    *ret_def = def;
    *ret_hdr = hdr;

    return fd;

 error:
    virDomainDefFree(def);
    VIR_FORCE_CLOSE(fd);
    return -1;
}