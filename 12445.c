static inline int FNAME(is_present_gpte)(unsigned long pte)
{
#if PTTYPE != PTTYPE_EPT
	return pte & PT_PRESENT_MASK;
#else
	return pte & 7;
#endif
}