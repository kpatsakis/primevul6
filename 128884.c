libxlNetworkPrepareDevices(virDomainDef *def)
{
    size_t i;
    g_autoptr(virConnect) conn = NULL;

    for (i = 0; i < def->nnets; i++) {
        virDomainNetDef *net = def->nets[i];
        virDomainNetType actualType;

        /* If appropriate, grab a physical device from the configured
         * network's pool of devices, or resolve bridge device name
         * to the one defined in the network definition.
         */
        if (net->type == VIR_DOMAIN_NET_TYPE_NETWORK) {
            if (!conn && !(conn = virGetConnectNetwork()))
                return -1;
            if (virDomainNetAllocateActualDevice(conn, def, net) < 0)
                return -1;
        }

        /* final validation now that actual type is known */
        if (virDomainActualNetDefValidate(net) < 0)
            return -1;

        actualType = virDomainNetGetActualType(net);
        if (actualType == VIR_DOMAIN_NET_TYPE_HOSTDEV &&
            net->type == VIR_DOMAIN_NET_TYPE_NETWORK) {
            /* Each type='hostdev' network device must also have a
             * corresponding entry in the hostdevs array. For netdevs
             * that are hardcoded as type='hostdev', this is already
             * done by the parser, but for those allocated from a
             * network / determined at runtime, we need to do it
             * separately.
             */
            virDomainHostdevDef *hostdev = virDomainNetGetActualHostdev(net);
            virDomainHostdevSubsysPCI *pcisrc = &hostdev->source.subsys.u.pci;

            if (hostdev->mode == VIR_DOMAIN_HOSTDEV_MODE_SUBSYS &&
                hostdev->source.subsys.type == VIR_DOMAIN_HOSTDEV_SUBSYS_TYPE_PCI)
                pcisrc->backend = VIR_DOMAIN_HOSTDEV_PCI_BACKEND_XEN;

            if (virDomainHostdevInsert(def, hostdev) < 0)
                return -1;
        }
    }

    return 0;
}