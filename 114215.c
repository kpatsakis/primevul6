set_interrupt_cause(E1000State *s, int index, uint32_t val)
{
    PCIDevice *d = PCI_DEVICE(s);
    uint32_t pending_ints;
    uint32_t mit_delay;

    s->mac_reg[ICR] = val;

    /*
     * Make sure ICR and ICS registers have the same value.
     * The spec says that the ICS register is write-only.  However in practice,
     * on real hardware ICS is readable, and for reads it has the same value as
     * ICR (except that ICS does not have the clear on read behaviour of ICR).
     *
     * The VxWorks PRO/1000 driver uses this behaviour.
     */
    s->mac_reg[ICS] = val;

    pending_ints = (s->mac_reg[IMS] & s->mac_reg[ICR]);
    if (!s->mit_irq_level && pending_ints) {
        /*
         * Here we detect a potential raising edge. We postpone raising the
         * interrupt line if we are inside the mitigation delay window
         * (s->mit_timer_on == 1).
         * We provide a partial implementation of interrupt mitigation,
         * emulating only RADV, TADV and ITR (lower 16 bits, 1024ns units for
         * RADV and TADV, 256ns units for ITR). RDTR is only used to enable
         * RADV; relative timers based on TIDV and RDTR are not implemented.
         */
        if (s->mit_timer_on) {
            return;
        }
        if (chkflag(MIT)) {
            /* Compute the next mitigation delay according to pending
             * interrupts and the current values of RADV (provided
             * RDTR!=0), TADV and ITR.
             * Then rearm the timer.
             */
            mit_delay = 0;
            if (s->mit_ide &&
                    (pending_ints & (E1000_ICR_TXQE | E1000_ICR_TXDW))) {
                mit_update_delay(&mit_delay, s->mac_reg[TADV] * 4);
            }
            if (s->mac_reg[RDTR] && (pending_ints & E1000_ICS_RXT0)) {
                mit_update_delay(&mit_delay, s->mac_reg[RADV] * 4);
            }
            mit_update_delay(&mit_delay, s->mac_reg[ITR]);

            /*
             * According to e1000 SPEC, the Ethernet controller guarantees
             * a maximum observable interrupt rate of 7813 interrupts/sec.
             * Thus if mit_delay < 500 then the delay should be set to the
             * minimum delay possible which is 500.
             */
            mit_delay = (mit_delay < 500) ? 500 : mit_delay;

            s->mit_timer_on = 1;
            timer_mod(s->mit_timer, qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) +
                      mit_delay * 256);
            s->mit_ide = 0;
        }
    }

    s->mit_irq_level = (pending_ints != 0);
    pci_set_irq(d, s->mit_irq_level);
}