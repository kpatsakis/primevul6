static ut32 msrk(ut16 v) {
	ut32 r = 0;
	ut32 a = ((v >> 12) & 0xf) << 1;
	ut32 b = ((v & 0xfff) >> 3) & 0xff;
	r |= a << 8;
	r |= b << 16;
	return r;
}