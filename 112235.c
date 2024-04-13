read_old_length(cdk_stream_t inp, int ctb, size_t * r_len, size_t * r_size)
{
	int llen = ctb & 0x03;
	int c;

	if (llen == 0) {
		c = cdk_stream_getc(inp);
		if (c == EOF)
			goto fail;

		*r_len = c;
		(*r_size)++;
	} else if (llen == 1) {
		*r_len = read_16(inp);
		if (*r_len == (u16)-1)
			goto fail;
		(*r_size) += 2;
	} else if (llen == 2) {
		*r_len = read_32(inp);
		if (*r_len == (u32)-1) {
			goto fail;
		}

		(*r_size) += 4;
	} else {
 fail:
		*r_len = 0;
		*r_size = 0;
	}
}