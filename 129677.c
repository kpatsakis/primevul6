void usb_ehci_init(EHCIState *s, DeviceState *dev)
{
    /* 2.2 host controller interface version */
    s->caps[0x00] = (uint8_t)(s->opregbase - s->capsbase);
    s->caps[0x01] = 0x00;
    s->caps[0x02] = 0x00;
    s->caps[0x03] = 0x01;        /* HC version */
    s->caps[0x04] = s->portnr;   /* Number of downstream ports */
    s->caps[0x05] = 0x00;        /* No companion ports at present */
    s->caps[0x06] = 0x00;
    s->caps[0x07] = 0x00;
    s->caps[0x08] = 0x80;        /* We can cache whole frame, no 64-bit */
    s->caps[0x0a] = 0x00;
    s->caps[0x0b] = 0x00;

    QTAILQ_INIT(&s->aqueues);
    QTAILQ_INIT(&s->pqueues);
    usb_packet_init(&s->ipacket);

    memory_region_init(&s->mem, OBJECT(dev), "ehci", MMIO_SIZE);
    memory_region_init_io(&s->mem_caps, OBJECT(dev), &ehci_mmio_caps_ops, s,
                          "capabilities", CAPA_SIZE);
    memory_region_init_io(&s->mem_opreg, OBJECT(dev), &ehci_mmio_opreg_ops, s,
                          "operational", s->portscbase);
    memory_region_init_io(&s->mem_ports, OBJECT(dev), &ehci_mmio_port_ops, s,
                          "ports", 4 * s->portnr);

    memory_region_add_subregion(&s->mem, s->capsbase, &s->mem_caps);
    memory_region_add_subregion(&s->mem, s->opregbase, &s->mem_opreg);
    memory_region_add_subregion(&s->mem, s->opregbase + s->portscbase,
                                &s->mem_ports);
}