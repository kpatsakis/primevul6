storageStateCleanup(void)
{
    if (!driver)
        return -1;

    storageDriverLock();

    virObjectUnref(driver->caps);
    virObjectUnref(driver->storageEventState);

    /* free inactive pools */
    virObjectUnref(driver->pools);

    if (driver->lockFD != -1)
        virPidFileRelease(driver->stateDir, "driver",
                          driver->lockFD);

    VIR_FREE(driver->configDir);
    VIR_FREE(driver->autostartDir);
    VIR_FREE(driver->stateDir);
    storageDriverUnlock();
    virMutexDestroy(&driver->lock);
    VIR_FREE(driver);

    return 0;
}