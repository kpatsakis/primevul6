static bool cpu_common_exception_index_needed(void *opaque)
{
    CPUState *cpu = opaque;

    return tcg_enabled() && cpu->exception_index != -1;
}