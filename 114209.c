pci_e1000_uninit(PCIDevice *dev)
{
    E1000State *d = E1000(dev);

    timer_free(d->autoneg_timer);
    timer_free(d->mit_timer);
    timer_free(d->flush_queue_timer);
    qemu_del_nic(d->nic);
}