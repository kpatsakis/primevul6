static inline void paravirt_release_p4d(unsigned long pfn)
{
	PVOP_VCALL1(mmu.release_p4d, pfn);
}