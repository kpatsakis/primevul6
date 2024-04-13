static inline void set_pud(pud_t *pudp, pud_t pud)
{
	pudval_t val = native_pud_val(pud);

	if (sizeof(pudval_t) > sizeof(long))
		PVOP_VCALL3(mmu.set_pud, pudp, val, (u64)val >> 32);
	else
		PVOP_VCALL2(mmu.set_pud, pudp, val);
}