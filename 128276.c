static int cpu_common_pre_load(void *opaque)
{
    CPUState *cpu = opaque;

    cpu->exception_index = -1;

    return 0;
}