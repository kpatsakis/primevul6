storageConnectNumOfStoragePools(virConnectPtr conn)
{
    if (virConnectNumOfStoragePoolsEnsureACL(conn) < 0)
        return -1;

    return virStoragePoolObjNumOfStoragePools(driver->pools, conn, true,
                                              virConnectNumOfStoragePoolsCheckACL);
}