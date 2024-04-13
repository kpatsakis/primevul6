static inline unsigned long paravirt_store_tr(void)
{
	return PVOP_CALL0(unsigned long, cpu.store_tr);
}