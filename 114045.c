static inline struct ist htx_sl_p1(const struct htx_sl *sl)
{
	return ist2(HTX_SL_P1_PTR(sl), HTX_SL_P1_LEN(sl));
}