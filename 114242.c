e1000_set_link_status(NetClientState *nc)
{
    E1000State *s = qemu_get_nic_opaque(nc);
    uint32_t old_status = s->mac_reg[STATUS];

    if (nc->link_down) {
        e1000x_update_regs_on_link_down(s->mac_reg, s->phy_reg);
    } else {
        if (have_autoneg(s) &&
            !(s->phy_reg[PHY_STATUS] & MII_SR_AUTONEG_COMPLETE)) {
            e1000x_restart_autoneg(s->mac_reg, s->phy_reg, s->autoneg_timer);
        } else {
            e1000_link_up(s);
        }
    }

    if (s->mac_reg[STATUS] != old_status)
        set_ics(s, 0, E1000_ICR_LSC);
}