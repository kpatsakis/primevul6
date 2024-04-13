static inline void p4d_clear(p4d_t *p4dp)
{
	set_p4d(p4dp, __p4d(0));
}