get_space(size_t need)
{
    need += TPS(out_used);
    if (need > TPS(out_size)) {
	TPS(out_size) = need * 2;
	TYPE_REALLOC(char, TPS(out_size), TPS(out_buff));
    }
}