rndpush(const void *s)
{
	register uint32_t h = qh_state;

	BAFHUpdateStr_reg(h, s);
	BAFHUpdateOctet_reg(h, 0);
	qh_state = h;
}