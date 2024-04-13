static void load_TLS_descriptor(struct thread_struct *t,
				unsigned int cpu, unsigned int i)
{
	struct desc_struct *shadow = &per_cpu(shadow_tls_desc, cpu).desc[i];
	struct desc_struct *gdt;
	xmaddr_t maddr;
	struct multicall_space mc;

	if (desc_equal(shadow, &t->tls_array[i]))
		return;

	*shadow = t->tls_array[i];

	gdt = get_cpu_gdt_rw(cpu);
	maddr = arbitrary_virt_to_machine(&gdt[GDT_ENTRY_TLS_MIN+i]);
	mc = __xen_mc_entry(0);

	MULTI_update_descriptor(mc.mc, maddr.maddr, t->tls_array[i]);
}