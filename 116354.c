atol8(const char *p, unsigned char_cnt)
{
	int64_t l;
	int digit;

	l = 0;
	while (char_cnt-- > 0) {
		if (*p >= '0' && *p <= '7')
			digit = *p - '0';
		else
			return (l);
		p++;
		l <<= 3;
		l |= digit;
	}
	return (l);
}