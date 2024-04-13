static __always_inline void pv_wait(u8 *ptr, u8 val)
{
	PVOP_VCALL2(lock.wait, ptr, val);
}