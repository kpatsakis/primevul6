save_text(const char *fmt, const char *s, int len)
{
    size_t s_len = (size_t) len + strlen(s) + strlen(fmt);
    get_space(s_len + 1);

    _nc_SPRINTF(TPS(out_buff) + TPS(out_used),
		_nc_SLIMIT(TPS(out_size) - TPS(out_used))
		fmt, s);
    TPS(out_used) += strlen(TPS(out_buff) + TPS(out_used));
}