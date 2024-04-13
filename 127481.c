static inline void write_gdt_entry(struct desc_struct *dt, int entry,
				   void *desc, int type)
{
	PVOP_VCALL4(cpu.write_gdt_entry, dt, entry, desc, type);
}