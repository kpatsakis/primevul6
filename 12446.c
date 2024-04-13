static bool FNAME(is_bad_mt_xwr)(struct rsvd_bits_validate *rsvd_check, u64 gpte)
{
#if PTTYPE != PTTYPE_EPT
	return false;
#else
	return __is_bad_mt_xwr(rsvd_check, gpte);
#endif
}