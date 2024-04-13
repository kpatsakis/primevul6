static inline void pmd_clear(pmd_t *pmdp)
{
	PVOP_VCALL1(mmu.pmd_clear, pmdp);
}