decode_entities(pTHX_ SV* sv, HV* entity2char, bool expand_prefix)
{
    STRLEN len;
    char *s = SvPV_force(sv, len);
    char *t = s;
    char *end = s + len;
    char *ent_start;

    char *repl;
    STRLEN repl_len;
#ifdef UNICODE_HTML_PARSER
    char buf[UTF8_MAXLEN];
    int repl_utf8;
    int high_surrogate = 0;
#else
    char buf[1];
#endif

#if defined(__GNUC__) && defined(UNICODE_HTML_PARSER)
    /* gcc -Wall reports this variable as possibly used uninitialized */
    repl_utf8 = 0;
#endif

    while (s < end) {
	assert(t <= s);

	if ((*t++ = *s++) != '&')
	    continue;

	ent_start = s;
	repl = 0;

	if (*s == '#') {
	    UV num = 0;
	    UV prev = 0;
	    int ok = 0;
	    s++;
	    if (*s == 'x' || *s == 'X') {
		s++;
		while (*s) {
		    char *tmp = strchr(PL_hexdigit, *s);
		    if (!tmp)
			break;
		    num = num << 4 | ((tmp - PL_hexdigit) & 15);
		    if (prev && num <= prev) {
			/* overflow */
			ok = 0;
			break;
		    }
		    prev = num;
		    s++;
		    ok = 1;
		}
	    }
	    else {
		while (isDIGIT(*s)) {
		    num = num * 10 + (*s - '0');
		    if (prev && num < prev) {
			/* overflow */
			ok = 0;
			break;
		    }
		    prev = num;
		    s++;
		    ok = 1;
		}
	    }
	    if (ok) {
#ifdef UNICODE_HTML_PARSER
		if (!SvUTF8(sv) && num <= 255) {
		    buf[0] = (char) num;
		    repl = buf;
		    repl_len = 1;
		    repl_utf8 = 0;
		}
		else {
		    char *tmp;
		    if ((num & 0xFFFFFC00) == 0xDC00) {  /* low-surrogate */
			if (high_surrogate != 0) {
			    t -= 3; /* Back up past 0xFFFD */
			    num = ((high_surrogate - 0xD800) << 10) +
				(num - 0xDC00) + 0x10000;
			    high_surrogate = 0;
			} else {
			    num = 0xFFFD;
			}
		    }
		    else if ((num & 0xFFFFFC00) == 0xD800) { /* high-surrogate */
			high_surrogate = num;
			num = 0xFFFD;
		    }
		    else {
			high_surrogate = 0;
			/* otherwise invalid? */
			if ((num >= 0xFDD0 && num <= 0xFDEF) ||
			    ((num & 0xFFFE) == 0xFFFE) ||
			    num > 0x10FFFF)
			{
			    num = 0xFFFD;
			}
		    }

		    tmp = (char*)uvuni_to_utf8((U8*)buf, num);
		    repl = buf;
		    repl_len = tmp - buf;
		    repl_utf8 = 1;
		}
#else
		if (num <= 255) {
		    buf[0] = (char) num & 0xFF;
		    repl = buf;
		    repl_len = 1;
		}
#endif
	    }
	}
	else {
	    char *ent_name = s;
	    while (isALNUM(*s))
		s++;
	    if (ent_name != s && entity2char) {
		SV** svp;
		if (              (svp = hv_fetch(entity2char, ent_name, s - ent_name, 0)) ||
		    (*s == ';' && (svp = hv_fetch(entity2char, ent_name, s - ent_name + 1, 0)))
		   )
		{
		    repl = SvPV(*svp, repl_len);
#ifdef UNICODE_HTML_PARSER
		    repl_utf8 = SvUTF8(*svp);
#endif
		}
		else if (expand_prefix) {
		    char *ss = s - 1;
		    while (ss > ent_name) {
			svp = hv_fetch(entity2char, ent_name, ss - ent_name, 0);
			if (svp) {
			    repl = SvPV(*svp, repl_len);
#ifdef UNICODE_HTML_PARSER
			    repl_utf8 = SvUTF8(*svp);
#endif
			    s = ss;
			    break;
			}
			ss--;
		    }
		}
	    }
#ifdef UNICODE_HTML_PARSER
	    high_surrogate = 0;
#endif
	}

	if (repl) {
	    char *repl_allocated = 0;
	    if (*s == ';')
		s++;
	    t--;  /* '&' already copied, undo it */

#ifdef UNICODE_HTML_PARSER
	    if (*s != '&') {
		high_surrogate = 0;
	    }

	    if (!SvUTF8(sv) && repl_utf8) {
		/* need to upgrade sv before we continue */
		STRLEN before_gap_len = t - SvPVX(sv);
		char *before_gap = (char*)bytes_to_utf8((U8*)SvPVX(sv), &before_gap_len);
		STRLEN after_gap_len = end - s;
		char *after_gap = (char*)bytes_to_utf8((U8*)s, &after_gap_len);

		sv_setpvn(sv, before_gap, before_gap_len);
		sv_catpvn(sv, after_gap, after_gap_len);
		SvUTF8_on(sv);

		Safefree(before_gap);
		Safefree(after_gap);

		s = t = SvPVX(sv) + before_gap_len;
		end = SvPVX(sv) + before_gap_len + after_gap_len;
	    }
	    else if (SvUTF8(sv) && !repl_utf8) {
		repl = (char*)bytes_to_utf8((U8*)repl, &repl_len);
		repl_allocated = repl;
	    }
#endif

	    if (t + repl_len > s) {
		/* need to grow the string */
		grow_gap(aTHX_ sv, repl_len - (s - t), &t, &s, &end);
	    }

	    /* copy replacement string into string */
	    while (repl_len--)
		*t++ = *repl++;

	    if (repl_allocated)
		Safefree(repl_allocated);
	}
	else {
	    while (ent_start < s)
		*t++ = *ent_start++;
	}
    }

    *t = '\0';
    SvCUR_set(sv, t - SvPVX(sv));

    return sv;
}