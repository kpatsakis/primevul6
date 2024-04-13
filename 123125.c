storagePoolSetAutostart(virStoragePoolPtr pool,
                        int autostart)
{
    virStoragePoolObj *obj;
    const char *configFile;
    const char *autostartLink;
    bool new_autostart;
    bool cur_autostart;
    int ret = -1;

    if (!(obj = storagePoolObjFindByUUID(pool->uuid, pool->name)))
        goto cleanup;

    if (virStoragePoolSetAutostartEnsureACL(pool->conn,
                                            virStoragePoolObjGetDef(obj)) < 0)
        goto cleanup;

    if (!(configFile = virStoragePoolObjGetConfigFile(obj))) {
        virReportError(VIR_ERR_INTERNAL_ERROR,
                       "%s", _("pool has no config file"));
        goto cleanup;
    }

    autostartLink = virStoragePoolObjGetAutostartLink(obj);

    new_autostart = (autostart != 0);
    cur_autostart = virStoragePoolObjIsAutostart(obj);
    if (cur_autostart != new_autostart) {
        if (new_autostart) {
            if (g_mkdir_with_parents(driver->autostartDir, 0777) < 0) {
                virReportSystemError(errno,
                                     _("cannot create autostart directory %s"),
                                     driver->autostartDir);
                goto cleanup;
            }

            if (symlink(configFile, autostartLink) < 0) {
                virReportSystemError(errno,
                                     _("Failed to create symlink '%s' to '%s'"),
                                     autostartLink, configFile);
                goto cleanup;
            }
        } else {
            if (autostartLink && unlink(autostartLink) < 0 &&
                errno != ENOENT && errno != ENOTDIR) {
                virReportSystemError(errno,
                                     _("Failed to delete symlink '%s'"),
                                     autostartLink);
                goto cleanup;
            }
        }
        virStoragePoolObjSetAutostart(obj, new_autostart);
    }

    ret = 0;

 cleanup:
    virStoragePoolObjEndAPI(&obj);
    return ret;
}