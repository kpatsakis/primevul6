static inline void load_gdt(const struct desc_ptr *dtr)
{
	PVOP_VCALL1(cpu.load_gdt, dtr);
}