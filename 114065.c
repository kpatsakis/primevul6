static inline struct ist htx_sl_req_vsn(const struct htx_sl *sl)
{
	return htx_sl_p3(sl);
}