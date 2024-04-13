static inline struct ist htx_sl_res_reason(const struct htx_sl *sl)
{
	return htx_sl_p3(sl);
}