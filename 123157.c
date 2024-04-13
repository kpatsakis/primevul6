storageConnectGetStoragePoolCapabilities(virConnectPtr conn,
                                         unsigned int flags)
{
    virStoragePoolCaps *caps = NULL;
    char *ret;

    virCheckFlags(0, NULL);

    if (virConnectGetStoragePoolCapabilitiesEnsureACL(conn) < 0)
        return NULL;

    if (!(caps = virStoragePoolCapsNew(driver->caps)))
        return NULL;

    ret = virStoragePoolCapsFormat(caps);

    virObjectUnref(caps);
    return ret;
}