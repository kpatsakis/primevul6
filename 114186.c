e1000_can_receive(NetClientState *nc)
{
    E1000State *s = qemu_get_nic_opaque(nc);

    return e1000x_rx_ready(&s->parent_obj, s->mac_reg) &&
        e1000_has_rxbufs(s, 1) && !timer_pending(s->flush_queue_timer);
}