static void __init xen_setup_gdt(int cpu)
{
	pv_ops.cpu.write_gdt_entry = xen_write_gdt_entry_boot;
	pv_ops.cpu.load_gdt = xen_load_gdt_boot;

	setup_stack_canary_segment(cpu);
	switch_to_new_gdt(cpu);

	pv_ops.cpu.write_gdt_entry = xen_write_gdt_entry;
	pv_ops.cpu.load_gdt = xen_load_gdt;
}