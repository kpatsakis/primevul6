e1000_receiver_overrun(E1000State *s, size_t size)
{
    trace_e1000_receiver_overrun(size, s->mac_reg[RDH], s->mac_reg[RDT]);
    e1000x_inc_reg_if_not_full(s->mac_reg, RNBC);
    e1000x_inc_reg_if_not_full(s->mac_reg, MPC);
    set_ics(s, 0, E1000_ICS_RXO);
}