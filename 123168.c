storageConnectStoragePoolEventDeregisterAny(virConnectPtr conn,
                                            int callbackID)
{
    if (virConnectStoragePoolEventDeregisterAnyEnsureACL(conn) < 0)
        return -1;

    if (virObjectEventStateDeregisterID(conn,
                                        driver->storageEventState,
                                        callbackID, true) < 0)
        return -1;

    return 0;
}