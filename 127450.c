static void xen_load_tls(struct thread_struct *t, unsigned int cpu)
{
	/*
	 * XXX sleazy hack: If we're being called in a lazy-cpu zone
	 * and lazy gs handling is enabled, it means we're in a
	 * context switch, and %gs has just been saved.  This means we
	 * can zero it out to prevent faults on exit from the
	 * hypervisor if the next process has no %gs.  Either way, it
	 * has been saved, and the new value will get loaded properly.
	 * This will go away as soon as Xen has been modified to not
	 * save/restore %gs for normal hypercalls.
	 *
	 * On x86_64, this hack is not used for %gs, because gs points
	 * to KERNEL_GS_BASE (and uses it for PDA references), so we
	 * must not zero %gs on x86_64
	 *
	 * For x86_64, we need to zero %fs, otherwise we may get an
	 * exception between the new %fs descriptor being loaded and
	 * %fs being effectively cleared at __switch_to().
	 */
	if (paravirt_get_lazy_mode() == PARAVIRT_LAZY_CPU) {
#ifdef CONFIG_X86_32
		lazy_load_gs(0);
#else
		loadsegment(fs, 0);
#endif
	}

	xen_mc_batch();

	load_TLS_descriptor(t, cpu, 0);
	load_TLS_descriptor(t, cpu, 1);
	load_TLS_descriptor(t, cpu, 2);

	xen_mc_issue(PARAVIRT_LAZY_CPU);
}