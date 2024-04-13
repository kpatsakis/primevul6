static int xen_cpu_dead_pv(unsigned int cpu)
{
	xen_smp_intr_free(cpu);
	xen_smp_intr_free_pv(cpu);

	xen_teardown_timer(cpu);

	return 0;
}