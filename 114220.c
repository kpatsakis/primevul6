set_phy_ctrl(E1000State *s, int index, uint16_t val)
{
    /* bits 0-5 reserved; MII_CR_[RESTART_AUTO_NEG,RESET] are self clearing */
    s->phy_reg[PHY_CTRL] = val & ~(0x3f |
                                   MII_CR_RESET |
                                   MII_CR_RESTART_AUTO_NEG);

    /*
     * QEMU 1.3 does not support link auto-negotiation emulation, so if we
     * migrate during auto negotiation, after migration the link will be
     * down.
     */
    if (have_autoneg(s) && (val & MII_CR_RESTART_AUTO_NEG)) {
        e1000x_restart_autoneg(s->mac_reg, s->phy_reg, s->autoneg_timer);
    }
}