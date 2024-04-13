storageStateInitialize(bool privileged,
                       const char *root,
                       virStateInhibitCallback callback G_GNUC_UNUSED,
                       void *opaque G_GNUC_UNUSED)
{
    g_autofree char *configdir = NULL;
    g_autofree char *rundir = NULL;
    bool autostart = true;

    if (root != NULL) {
        virReportError(VIR_ERR_INVALID_ARG, "%s",
                       _("Driver does not support embedded mode"));
        return -1;
    }

    driver = g_new0(virStorageDriverState, 1);

    driver->lockFD = -1;
    if (virMutexInit(&driver->lock) < 0) {
        VIR_FREE(driver);
        return VIR_DRV_STATE_INIT_ERROR;
    }
    storageDriverLock();

    if (!(driver->pools = virStoragePoolObjListNew()))
        goto error;

    if (privileged) {
        driver->configDir = g_strdup(SYSCONFDIR "/libvirt/storage");
        driver->autostartDir = g_strdup(SYSCONFDIR "/libvirt/storage/autostart");
        driver->stateDir = g_strdup(RUNSTATEDIR "/libvirt/storage");
    } else {
        configdir = virGetUserConfigDirectory();
        rundir = virGetUserRuntimeDirectory();

        driver->configDir = g_strdup_printf("%s/storage", configdir);
        driver->autostartDir = g_strdup_printf("%s/storage/autostart", configdir);
        driver->stateDir = g_strdup_printf("%s/storage/run", rundir);
    }
    driver->privileged = privileged;

    if (g_mkdir_with_parents(driver->stateDir, 0777) < 0) {
        virReportError(errno,
                       _("cannot create directory %s"),
                       driver->stateDir);
        goto error;
    }

    if ((driver->lockFD =
         virPidFileAcquire(driver->stateDir, "driver",
                           false, getpid())) < 0)
        goto error;

    if (virStoragePoolObjLoadAllState(driver->pools,
                                      driver->stateDir) < 0)
        goto error;

    if (virStoragePoolObjLoadAllConfigs(driver->pools,
                                        driver->configDir,
                                        driver->autostartDir) < 0)
        goto error;

    storagePoolUpdateAllState();

    if (virDriverShouldAutostart(driver->stateDir, &autostart) < 0)
        goto error;

    if (autostart)
        storageDriverAutostart();

    driver->storageEventState = virObjectEventStateNew();

    /* Only one load of storage driver plus backends exists. Unlike
     * domains where new binaries could change the capabilities. A
     * new/changed backend requires a reinitialization. */
    if (!(driver->caps = virStorageBackendGetCapabilities()))
        goto error;

    storageDriverUnlock();

    return VIR_DRV_STATE_INIT_COMPLETE;

 error:
    storageDriverUnlock();
    storageStateCleanup();
    return VIR_DRV_STATE_INIT_ERROR;
}