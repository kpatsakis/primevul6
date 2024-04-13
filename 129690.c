void usb_ehci_unrealize(EHCIState *s, DeviceState *dev, Error **errp)
{
    trace_usb_ehci_unrealize();

    if (s->frame_timer) {
        timer_del(s->frame_timer);
        timer_free(s->frame_timer);
        s->frame_timer = NULL;
    }
    if (s->async_bh) {
        qemu_bh_delete(s->async_bh);
    }

    ehci_queues_rip_all(s, 0);
    ehci_queues_rip_all(s, 1);

    memory_region_del_subregion(&s->mem, &s->mem_caps);
    memory_region_del_subregion(&s->mem, &s->mem_opreg);
    memory_region_del_subregion(&s->mem, &s->mem_ports);

    usb_bus_release(&s->bus);

    if (s->vmstate) {
        qemu_del_vm_change_state_handler(s->vmstate);
    }
}