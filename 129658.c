static void ehci_register_companion(USBBus *bus, USBPort *ports[],
                                    uint32_t portcount, uint32_t firstport,
                                    Error **errp)
{
    EHCIState *s = container_of(bus, EHCIState, bus);
    uint32_t i;

    if (firstport + portcount > NB_PORTS) {
        error_setg(errp, "firstport must be between 0 and %u",
                   NB_PORTS - portcount);
        return;
    }

    for (i = 0; i < portcount; i++) {
        if (s->companion_ports[firstport + i]) {
            error_setg(errp, "firstport %u asks for ports %u-%u,"
                       " but port %u has a companion assigned already",
                       firstport, firstport, firstport + portcount - 1,
                       firstport + i);
            return;
        }
    }

    for (i = 0; i < portcount; i++) {
        s->companion_ports[firstport + i] = ports[i];
        s->ports[firstport + i].speedmask |=
            USB_SPEED_MASK_LOW | USB_SPEED_MASK_FULL;
        /* Ensure devs attached before the initial reset go to the companion */
        s->portsc[firstport + i] = PORTSC_POWNER;
    }

    s->companion_count++;
    s->caps[0x05] = (s->companion_count << 4) | portcount;
}