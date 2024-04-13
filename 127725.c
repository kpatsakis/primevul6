array_index_calc(const char *n, bool *arrayp, uint32_t *valp)
{
	const char *p;
	size_t len;
	char *ap = NULL;

	*arrayp = false;
 redo_from_ref:
	p = skip_varname(n, false);
	if (innermost_refflag == SRF_NOP && (p != n) && ksh_isalphx(n[0])) {
		struct tbl *vp;
		char *vn;

		strndupx(vn, n, p - n, ATEMP);
		/* check if this is a reference */
		varsearch(e->loc, &vp, vn, hash(vn));
		afree(vn, ATEMP);
		if (vp && (vp->flag & (DEFINED | ASSOC | ARRAY)) ==
		    (DEFINED | ASSOC)) {
			char *cp;

			/* gotcha! */
			cp = shf_smprintf("%s%s", str_val(vp), p);
			afree(ap, ATEMP);
			n = ap = cp;
			goto redo_from_ref;
		}
	}
	innermost_refflag = SRF_NOP;

	if (p != n && *p == '[' && (len = array_ref_len(p))) {
		char *sub, *tmp;
		mksh_ari_t rval;

		/* calculate the value of the subscript */
		*arrayp = true;
		strndupx(tmp, p + 1, len - 2, ATEMP);
		sub = substitute(tmp, 0);
		afree(tmp, ATEMP);
		strndupx(n, n, p - n, ATEMP);
		evaluate(sub, &rval, KSH_UNWIND_ERROR, true);
		*valp = (uint32_t)rval;
		afree(sub, ATEMP);
	}
	return (n);
}