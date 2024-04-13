storageConnectListAllStoragePools(virConnectPtr conn,
                                  virStoragePoolPtr **pools,
                                  unsigned int flags)
{
    virCheckFlags(VIR_CONNECT_LIST_STORAGE_POOLS_FILTERS_ALL, -1);

    if (virConnectListAllStoragePoolsEnsureACL(conn) < 0)
        return -1;

    return virStoragePoolObjListExport(conn, driver->pools, pools,
                                       virConnectListAllStoragePoolsCheckACL,
                                       flags);
}