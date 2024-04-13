receive_filter(E1000State *s, const uint8_t *buf, int size)
{
    uint32_t rctl = s->mac_reg[RCTL];
    int isbcast = !memcmp(buf, bcast, sizeof bcast), ismcast = (buf[0] & 1);

    if (e1000x_is_vlan_packet(buf, le16_to_cpu(s->mac_reg[VET])) &&
        e1000x_vlan_rx_filter_enabled(s->mac_reg)) {
        uint16_t vid = lduw_be_p(buf + 14);
        uint32_t vfta = ldl_le_p((uint32_t*)(s->mac_reg + VFTA) +
                                 ((vid >> 5) & 0x7f));
        if ((vfta & (1 << (vid & 0x1f))) == 0)
            return 0;
    }

    if (!isbcast && !ismcast && (rctl & E1000_RCTL_UPE)) { /* promiscuous ucast */
        return 1;
    }

    if (ismcast && (rctl & E1000_RCTL_MPE)) {          /* promiscuous mcast */
        e1000x_inc_reg_if_not_full(s->mac_reg, MPRC);
        return 1;
    }

    if (isbcast && (rctl & E1000_RCTL_BAM)) {          /* broadcast enabled */
        e1000x_inc_reg_if_not_full(s->mac_reg, BPRC);
        return 1;
    }

    return e1000x_rx_group_filter(s->mac_reg, buf);
}