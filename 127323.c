static inline void set_pmd(pmd_t *pmdp, pmd_t pmd)
{
	pmdval_t val = native_pmd_val(pmd);

	if (sizeof(pmdval_t) > sizeof(long))
		PVOP_VCALL3(mmu.set_pmd, pmdp, val, (u64)val >> 32);
	else
		PVOP_VCALL2(mmu.set_pmd, pmdp, val);
}