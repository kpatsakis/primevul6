e1000_link_up(E1000State *s)
{
    e1000x_update_regs_on_link_up(s->mac_reg, s->phy_reg);

    /* E1000_STATUS_LU is tested by e1000_can_receive() */
    qemu_flush_queued_packets(qemu_get_queue(s->nic));
}