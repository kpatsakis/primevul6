static inline void load_idt(const struct desc_ptr *dtr)
{
	PVOP_VCALL1(cpu.load_idt, dtr);
}