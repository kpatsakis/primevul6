static inline void paravirt_free_ldt(struct desc_struct *ldt, unsigned entries)
{
	PVOP_VCALL2(cpu.free_ldt, ldt, entries);
}