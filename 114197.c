e1000_flush_queue_timer(void *opaque)
{
    E1000State *s = opaque;

    qemu_flush_queued_packets(qemu_get_queue(s->nic));
}