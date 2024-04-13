void xen_copy_trap_info(struct trap_info *traps)
{
	const struct desc_ptr *desc = this_cpu_ptr(&idt_desc);

	xen_convert_trap_info(desc, traps);
}