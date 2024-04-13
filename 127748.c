formatstr(struct tbl *vp, const char *s)
{
	int olen, nlen;
	char *p, *q;
	size_t psiz;

	olen = (int)utf_mbswidth(s);

	if (vp->flag & (RJUST|LJUST)) {
		if (!vp->u2.field)
			/* default field width */
			vp->u2.field = olen;
		nlen = vp->u2.field;
	} else
		nlen = olen;

	p = alloc((psiz = nlen * /* MB_LEN_MAX */ 3 + 1), ATEMP);
	if (vp->flag & (RJUST|LJUST)) {
		int slen = olen, i = 0;

		if (vp->flag & RJUST) {
			const char *qq = s;
			int n = 0;

			while (i < slen)
				i += utf_widthadj(qq, &qq);
			/* strip trailing spaces (AT&T uses qq[-1] == ' ') */
			while (qq > s && ksh_isspace(qq[-1])) {
				--qq;
				--slen;
			}
			if (vp->flag & ZEROFIL && vp->flag & INTEGER) {
				if (!s[0] || !s[1])
					goto uhm_no;
				if (s[1] == '#')
					n = 2;
				else if (s[2] == '#')
					n = 3;
 uhm_no:
				if (vp->u2.field <= n)
					n = 0;
			}
			if (n) {
				memcpy(p, s, n);
				s += n;
			}
			while (slen > vp->u2.field)
				slen -= utf_widthadj(s, &s);
			if (vp->u2.field - slen)
				memset(p + n, (vp->flag & ZEROFIL) ? '0' : ' ',
				    vp->u2.field - slen);
			slen -= n;
			shf_snprintf(p + vp->u2.field - slen,
			    psiz - (vp->u2.field - slen),
			    "%.*s", slen, s);
		} else {
			/* strip leading spaces/zeros */
			while (ksh_isspace(*s))
				s++;
			if (vp->flag & ZEROFIL)
				while (*s == '0')
					s++;
			shf_snprintf(p, nlen + 1, "%-*.*s",
				vp->u2.field, vp->u2.field, s);
		}
	} else
		memcpy(p, s, strlen(s) + 1);

	if (vp->flag & UCASEV_AL) {
		for (q = p; *q; q++)
			*q = ksh_toupper(*q);
	} else if (vp->flag & LCASEV) {
		for (q = p; *q; q++)
			*q = ksh_tolower(*q);
	}

	return (p);
}