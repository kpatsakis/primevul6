static inline struct ist htx_sl_p3(const struct htx_sl *sl)
{
	return ist2(HTX_SL_P3_PTR(sl), HTX_SL_P3_LEN(sl));
}