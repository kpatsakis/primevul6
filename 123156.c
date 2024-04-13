storageRegisterFull(bool allbackends)
{
    if (virRegisterConnectDriver(&storageConnectDriver, false) < 0)
        return -1;
    if (virStorageBackendDriversRegister(allbackends) < 0)
        return -1;
    if (virSetSharedStorageDriver(&storageDriver) < 0)
        return -1;
    if (virRegisterStateDriver(&stateDriver) < 0)
        return -1;
    return 0;
}