read_new_length(cdk_stream_t inp,
		size_t * r_len, size_t * r_size, size_t * r_partial)
{
	int c, c1;

	c = cdk_stream_getc(inp);
	if (c == EOF)
		return;

	(*r_size)++;
	if (c < 192)
		*r_len = c;
	else if (c >= 192 && c <= 223) {
		c1 = cdk_stream_getc(inp);
		if (c1 == EOF)
			return;

		(*r_size)++;
		*r_len = ((c - 192) << 8) + c1 + 192;
	} else if (c == 255) {
		c1 = read_32(inp);
		if (c1 == (u32)-1) {
			return;
		}

		*r_len = c1;
		(*r_size) += 4;
	} else {
		*r_len = 1 << (c & 0x1f);
		*r_partial = 1;
	}
}