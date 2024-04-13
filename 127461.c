static int xen_cpu_up_prepare_pv(unsigned int cpu)
{
	int rc;

	if (per_cpu(xen_vcpu, cpu) == NULL)
		return -ENODEV;

	xen_setup_timer(cpu);

	rc = xen_smp_intr_init(cpu);
	if (rc) {
		WARN(1, "xen_smp_intr_init() for CPU %d failed: %d\n",
		     cpu, rc);
		return rc;
	}

	rc = xen_smp_intr_init_pv(cpu);
	if (rc) {
		WARN(1, "xen_smp_intr_init_pv() for CPU %d failed: %d\n",
		     cpu, rc);
		return rc;
	}

	return 0;
}