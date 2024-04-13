e1000_autoneg_timer(void *opaque)
{
    E1000State *s = opaque;
    if (!qemu_get_queue(s->nic)->link_down) {
        e1000_autoneg_done(s);
        set_ics(s, 0, E1000_ICS_LSC); /* signal link status change to guest */
    }
}