storageConnectListStoragePools(virConnectPtr conn,
                               char **const names,
                               int maxnames)
{
    if (virConnectListStoragePoolsEnsureACL(conn) < 0)
        return -1;

    return virStoragePoolObjGetNames(driver->pools, conn, true,
                                     virConnectListStoragePoolsCheckACL,
                                     names, maxnames);
}