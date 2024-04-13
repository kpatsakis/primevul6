void pcnet_set_link_status(NetClientState *nc)
{
    PCNetState *d = qemu_get_nic_opaque(nc);

    d->lnkst = nc->link_down ? 0 : 0x40;
}