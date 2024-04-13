getint(struct tbl *vp, mksh_ari_u *nump, bool arith)
{
	mksh_uari_t c, num, base;
	const char *s;
	bool have_base = false, neg = false;

	if (vp->flag&SPECIAL)
		getspec(vp);
	/* XXX is it possible for ISSET to be set and val.s to be NULL? */
	if (!(vp->flag&ISSET) || (!(vp->flag&INTEGER) && vp->val.s == NULL))
		return (-1);
	if (vp->flag&INTEGER) {
		nump->i = vp->val.i;
		return (vp->type);
	}
	s = vp->val.s + vp->type;
	base = 10;
	num = 0;
	if (arith && s[0] == '0' && (s[1] | 0x20) == 'x') {
		s += 2;
		base = 16;
		have_base = true;
	}
	if (Flag(FPOSIX) && arith && s[0] == '0' && ksh_isdigit(s[1]) &&
	    !(vp->flag & ZEROFIL)) {
		/* interpret as octal (deprecated) */
		base = 8;
		have_base = true;
	}
	while ((c = (unsigned char)*s++)) {
		if (c == '-') {
			neg = true;
			continue;
		} else if (c == '#') {
			if (have_base || num < 1 || num > 36)
				return (-1);
			if ((base = num) == 1) {
				unsigned int wc;

				if (!UTFMODE)
					wc = *(const unsigned char *)s;
				else if (utf_mbtowc(&wc, s) == (size_t)-1)
					/* OPTU-8 -> OPTU-16 */
					/*
					 * (with a twist: 1#\uEF80 converts
					 * the same as 1#\x80 does, thus is
					 * not round-tripping correctly XXX)
					 */
					wc = 0xEF00 + *(const unsigned char *)s;
				nump->u = (mksh_uari_t)wc;
				return (1);
			}
			num = 0;
			have_base = true;
			continue;
		} else if (ksh_isdigit(c))
			c -= '0';
		else if (ksh_islower(c))
			c -= 'a' - 10;
		else if (ksh_isupper(c))
			c -= 'A' - 10;
		else
			return (-1);
		if (c >= base)
			return (-1);
		num = num * base + c;
	}
	if (neg)
		num = -num;
	nump->u = num;
	return (base);
}