str_val(struct tbl *vp)
{
	char *s;

	if ((vp->flag&SPECIAL))
		getspec(vp);
	if (!(vp->flag&ISSET))
		/* special to dollar() */
		s = null;
	else if (!(vp->flag&INTEGER))
		/* string source */
		s = vp->val.s + vp->type;
	else {
		/* integer source */
		mksh_uari_t n;
		unsigned int base;
		/**
		 * worst case number length is when base == 2:
		 *	1 (minus) + 2 (base, up to 36) + 1 ('#') +
		 *	number of bits in the mksh_uari_t + 1 (NUL)
		 */
		char strbuf[1 + 2 + 1 + 8 * sizeof(mksh_uari_t) + 1];
		const char *digits = (vp->flag & UCASEV_AL) ?
		    digits_uc : digits_lc;

		s = strbuf + sizeof(strbuf);
		if (vp->flag & INT_U)
			n = vp->val.u;
		else
			n = (vp->val.i < 0) ? -vp->val.u : vp->val.u;
		base = (vp->type == 0) ? 10U : (unsigned int)vp->type;

		if (base == 1 && n == 0)
			base = 2;
		if (base == 1) {
			size_t sz = 1;

			*(s = strbuf) = '1';
			s[1] = '#';
			if (!UTFMODE || ((n & 0xFF80) == 0xEF80))
				/* OPTU-16 -> raw octet */
				s[2] = n & 0xFF;
			else
				sz = utf_wctomb(s + 2, n);
			s[2 + sz] = '\0';
		} else {
			*--s = '\0';
			do {
				*--s = digits[n % base];
				n /= base;
			} while (n != 0);
			if (base != 10) {
				*--s = '#';
				*--s = digits[base % 10];
				if (base >= 10)
					*--s = digits[base / 10];
			}
			if (!(vp->flag & INT_U) && vp->val.i < 0)
				*--s = '-';
		}
		if (vp->flag & (RJUST|LJUST))
			/* case already dealt with */
			s = formatstr(vp, s);
		else
			strdupx(s, s, ATEMP);
	}
	return (s);
}