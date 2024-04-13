static inline void set_p4d(p4d_t *p4dp, p4d_t p4d)
{
	p4dval_t val = native_p4d_val(p4d);

	PVOP_VCALL2(mmu.set_p4d, p4dp, val);
}