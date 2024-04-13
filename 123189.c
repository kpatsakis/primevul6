storageStateReload(void)
{
    if (!driver)
        return -1;

    storageDriverLock();
    virStoragePoolObjLoadAllState(driver->pools,
                                  driver->stateDir);
    virStoragePoolObjLoadAllConfigs(driver->pools,
                                    driver->configDir,
                                    driver->autostartDir);
    storageDriverAutostart();
    storageDriverUnlock();

    return 0;
}