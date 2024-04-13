storageConnectListDefinedStoragePools(virConnectPtr conn,
                                      char **const names,
                                      int maxnames)
{
    if (virConnectListDefinedStoragePoolsEnsureACL(conn) < 0)
        return -1;

    return virStoragePoolObjGetNames(driver->pools, conn, false,
                                     virConnectListDefinedStoragePoolsCheckACL,
                                     names, maxnames);
}