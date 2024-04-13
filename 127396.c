static void xen_load_idt(const struct desc_ptr *desc)
{
	static DEFINE_SPINLOCK(lock);
	static struct trap_info traps[257];

	trace_xen_cpu_load_idt(desc);

	spin_lock(&lock);

	memcpy(this_cpu_ptr(&idt_desc), desc, sizeof(idt_desc));

	xen_convert_trap_info(desc, traps);

	xen_mc_flush();
	if (HYPERVISOR_set_trap_table(traps))
		BUG();

	spin_unlock(&lock);
}