libxlDomainCreateIfaceNames(virDomainDef *def, libxl_domain_config *d_config)
{
    size_t i;

    for (i = 0; i < def->nnets && i < d_config->num_nics; i++) {
        virDomainNetDef *net = def->nets[i];
        libxl_device_nic *x_nic = &d_config->nics[i];
        const char *suffix =
            x_nic->nictype != LIBXL_NIC_TYPE_VIF ? "-emu" : "";

        if (net->ifname)
            continue;

        net->ifname = g_strdup_printf(LIBXL_GENERATED_PREFIX_XEN "%d.%d%s",
                                      def->id, x_nic->devid, suffix);
    }
}