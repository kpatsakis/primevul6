static inline void paravirt_alloc_ldt(struct desc_struct *ldt, unsigned entries)
{
	PVOP_VCALL2(cpu.alloc_ldt, ldt, entries);
}