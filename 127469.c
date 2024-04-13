static inline pgdval_t pgd_val(pgd_t pgd)
{
	pgdval_t ret;

	if (sizeof(pgdval_t) > sizeof(long))
		ret =  PVOP_CALLEE2(pgdval_t, mmu.pgd_val,
				    pgd.pgd, (u64)pgd.pgd >> 32);
	else
		ret =  PVOP_CALLEE1(pgdval_t, mmu.pgd_val, pgd.pgd);

	return ret;
}