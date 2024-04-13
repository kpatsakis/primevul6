static inline void write_ldt_entry(struct desc_struct *dt, int entry,
				   const void *desc)
{
	PVOP_VCALL3(cpu.write_ldt_entry, dt, entry, desc);
}