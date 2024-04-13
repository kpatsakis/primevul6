static void __init xen_write_gdt_entry_boot(struct desc_struct *dt, int entry,
					    const void *desc, int type)
{
	trace_xen_cpu_write_gdt_entry(dt, entry, desc, type);

	switch (type) {
	case DESC_LDT:
	case DESC_TSS:
		/* ignore */
		break;

	default: {
		xmaddr_t maddr = virt_to_machine(&dt[entry]);

		if (HYPERVISOR_update_descriptor(maddr.maddr, *(u64 *)desc))
			dt[entry] = *(struct desc_struct *)desc;
	}

	}
}