CPUState *qemu_get_cpu(int index)
{
    CPUState *cpu;

    CPU_FOREACH(cpu) {
        if (cpu->cpu_index == index) {
            return cpu;
        }
    }

    return NULL;
}