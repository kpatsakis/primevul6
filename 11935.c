save_number(const char *fmt, int number, int len)
{
    size_t s_len = (size_t) len + 30 + strlen(fmt);
    get_space(s_len + 1);

    _nc_SPRINTF(TPS(out_buff) + TPS(out_used),
		_nc_SLIMIT(TPS(out_size) - TPS(out_used))
		fmt, number);
    TPS(out_used) += strlen(TPS(out_buff) + TPS(out_used));
}