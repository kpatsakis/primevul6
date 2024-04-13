static bool cpu_common_crash_occurred_needed(void *opaque)
{
    CPUState *cpu = opaque;

    return cpu->crash_occurred;
}