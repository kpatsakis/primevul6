static void xen_write_gdt_entry(struct desc_struct *dt, int entry,
				const void *desc, int type)
{
	trace_xen_cpu_write_gdt_entry(dt, entry, desc, type);

	preempt_disable();

	switch (type) {
	case DESC_LDT:
	case DESC_TSS:
		/* ignore */
		break;

	default: {
		xmaddr_t maddr = arbitrary_virt_to_machine(&dt[entry]);

		xen_mc_flush();
		if (HYPERVISOR_update_descriptor(maddr.maddr, *(u64 *)desc))
			BUG();
	}

	}

	preempt_enable();
}