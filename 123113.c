storageConnectOpen(virConnectPtr conn,
                   virConnectAuthPtr auth G_GNUC_UNUSED,
                   virConf *conf G_GNUC_UNUSED,
                   unsigned int flags)
{
    virCheckFlags(VIR_CONNECT_RO, VIR_DRV_OPEN_ERROR);

    if (driver == NULL) {
        virReportError(VIR_ERR_INTERNAL_ERROR, "%s",
                       _("storage state driver is not active"));
        return VIR_DRV_OPEN_ERROR;
    }

    if (!virConnectValidateURIPath(conn->uri->path,
                                   "storage",
                                   driver->privileged))
        return VIR_DRV_OPEN_ERROR;

    if (virConnectOpenEnsureACL(conn) < 0)
        return VIR_DRV_OPEN_ERROR;

    return VIR_DRV_OPEN_SUCCESS;
}