save_char(int c)
{
    if (c == 0)
	c = 0200;
    get_space((size_t) 1);
    TPS(out_buff)[TPS(out_used)++] = (char) c;
}