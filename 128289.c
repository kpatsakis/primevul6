void cpu_exec_unrealizefn(CPUState *cpu)
{
    CPUClass *cc = CPU_GET_CLASS(cpu);

    cpu_list_remove(cpu);

    if (cc->vmsd != NULL) {
        vmstate_unregister(NULL, cc->vmsd, cpu);
    }
    if (qdev_get_vmsd(DEVICE(cpu)) == NULL) {
        vmstate_unregister(NULL, &vmstate_cpu_common, cpu);
    }
}