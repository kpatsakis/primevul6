virStorageBackendPloopRestoreDesc(char *path)
{
    g_autoptr(virCommand) cmd = NULL;
    g_autofree char *refresh_tool = NULL;
    g_autofree char *desc = NULL;

    desc = g_strdup_printf("%s/DiskDescriptor.xml", path);

    if (virFileRemove(desc, 0, 0) < 0) {
        virReportError(VIR_ERR_INTERNAL_ERROR, "%s",
                       _("refresh ploop failed:"
                         " unable to delete DiskDescriptor.xml"));
        return -1;
    }

    refresh_tool = virFindFileInPath("ploop");
    if (!refresh_tool) {
        virReportError(VIR_ERR_INTERNAL_ERROR, "%s",
                       _("unable to find ploop, please install ploop tools"));
        return -1;
    }

    cmd = virCommandNewArgList(refresh_tool, "restore-descriptor",
                               path, NULL);
    virCommandAddArgFormat(cmd, "%s/root.hds", path);
    return virCommandRun(cmd, NULL);
}