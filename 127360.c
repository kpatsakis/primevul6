static inline pgd_t __pgd(pgdval_t val)
{
	pgdval_t ret;

	if (sizeof(pgdval_t) > sizeof(long))
		ret = PVOP_CALLEE2(pgdval_t, mmu.make_pgd, val, (u64)val >> 32);
	else
		ret = PVOP_CALLEE1(pgdval_t, mmu.make_pgd, val);

	return (pgd_t) { ret };
}