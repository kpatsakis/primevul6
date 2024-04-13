static inline void wbinvd(void)
{
	PVOP_VCALL0(cpu.wbinvd);
}