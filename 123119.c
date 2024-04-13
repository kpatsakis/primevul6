storageConnectNumOfDefinedStoragePools(virConnectPtr conn)
{
    if (virConnectNumOfDefinedStoragePoolsEnsureACL(conn) < 0)
        return -1;

    return virStoragePoolObjNumOfStoragePools(driver->pools, conn, false,
                                               virConnectNumOfDefinedStoragePoolsCheckACL);
}