libxlNetworkUnwindDevices(virDomainDef *def)
{
    if (def->nnets) {
        size_t i;

        for (i = 0; i < def->nnets; i++) {
            virDomainNetDef *net = def->nets[i];

            if (net->ifname &&
                STRPREFIX(net->ifname, LIBXL_GENERATED_PREFIX_XEN))
                VIR_FREE(net->ifname);

            /* cleanup actual device */
            virDomainNetRemoveHostdev(def, net);
            if (net->type == VIR_DOMAIN_NET_TYPE_NETWORK) {
                g_autoptr(virConnect) conn = virGetConnectNetwork();

                if (conn)
                    virDomainNetReleaseActualDevice(conn, def, net);
                else
                    VIR_WARN("Unable to release network device '%s'", NULLSTR(net->ifname));
            }
        }
    }
}