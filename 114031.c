static inline struct ist htx_sl_req_uri(const struct htx_sl *sl)
{
	return htx_sl_p2(sl);
}