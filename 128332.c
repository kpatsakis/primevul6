static int cpu_common_post_load(void *opaque, int version_id)
{
    CPUState *cpu = opaque;

    /* 0x01 was CPU_INTERRUPT_EXIT. This line can be removed when the
       version_id is increased. */
    cpu->interrupt_request &= ~0x01;
    tlb_flush(cpu);

    return 0;
}