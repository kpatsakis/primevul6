set_rx_control(E1000State *s, int index, uint32_t val)
{
    s->mac_reg[RCTL] = val;
    s->rxbuf_size = e1000x_rxbufsize(val);
    s->rxbuf_min_shift = ((val / E1000_RCTL_RDMTS_QUAT) & 3) + 1;
    DBGOUT(RX, "RCTL: %d, mac_reg[RCTL] = 0x%x\n", s->mac_reg[RDT],
           s->mac_reg[RCTL]);
    timer_mod(s->flush_queue_timer,
              qemu_clock_get_ms(QEMU_CLOCK_VIRTUAL) + 1000);
}