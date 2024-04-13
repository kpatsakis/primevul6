storageConnectGetCapabilities(virConnectPtr conn)
{

    if (virConnectGetCapabilitiesEnsureACL(conn) < 0)
        return NULL;

    return virCapabilitiesFormatXML(driver->caps);
}