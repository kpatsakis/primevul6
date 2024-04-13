static inline struct ist htx_sl_res_vsn(const struct htx_sl *sl)
{
	return htx_sl_p1(sl);
}