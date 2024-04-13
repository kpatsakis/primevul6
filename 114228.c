have_autoneg(E1000State *s)
{
    return chkflag(AUTONEG) && (s->phy_reg[PHY_CTRL] & MII_CR_AUTO_NEG_EN);
}