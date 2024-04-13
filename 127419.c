void __init xen_setup_vcpu_info_placement(void)
{
	int cpu;

	for_each_possible_cpu(cpu) {
		/* Set up direct vCPU id mapping for PV guests. */
		per_cpu(xen_vcpu_id, cpu) = cpu;

		/*
		 * xen_vcpu_setup(cpu) can fail  -- in which case it
		 * falls back to the shared_info version for cpus
		 * where xen_vcpu_nr(cpu) < MAX_VIRT_CPUS.
		 *
		 * xen_cpu_up_prepare_pv() handles the rest by failing
		 * them in hotplug.
		 */
		(void) xen_vcpu_setup(cpu);
	}

	/*
	 * xen_vcpu_setup managed to place the vcpu_info within the
	 * percpu area for all cpus, so make use of it.
	 */
	if (xen_have_vcpu_info_placement) {
		pv_ops.irq.save_fl = __PV_IS_CALLEE_SAVE(xen_save_fl_direct);
		pv_ops.irq.restore_fl =
			__PV_IS_CALLEE_SAVE(xen_restore_fl_direct);
		pv_ops.irq.irq_disable =
			__PV_IS_CALLEE_SAVE(xen_irq_disable_direct);
		pv_ops.irq.irq_enable =
			__PV_IS_CALLEE_SAVE(xen_irq_enable_direct);
		pv_ops.mmu.read_cr2 =
			__PV_IS_CALLEE_SAVE(xen_read_cr2_direct);
	}
}