static inline struct ist htx_sl_p2(const struct htx_sl *sl)
{
	return ist2(HTX_SL_P2_PTR(sl), HTX_SL_P2_LEN(sl));
}