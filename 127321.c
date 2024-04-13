static inline p4d_t __p4d(p4dval_t val)
{
	p4dval_t ret = PVOP_CALLEE1(p4dval_t, mmu.make_p4d, val);

	return (p4d_t) { ret };
}