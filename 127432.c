static inline p4dval_t p4d_val(p4d_t p4d)
{
	return PVOP_CALLEE1(p4dval_t, mmu.p4d_val, p4d.p4d);
}