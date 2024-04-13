set_array(const char *var, bool reset, const char **vals)
{
	struct tbl *vp, *vq;
	mksh_uari_t i = 0, j = 0;
	const char *ccp = var;
	char *cp = NULL;
	size_t n;

	/* to get local array, use "local foo; set -A foo" */
	n = strlen(var);
	if (n > 0 && var[n - 1] == '+') {
		/* append mode */
		reset = false;
		strndupx(cp, var, n - 1, ATEMP);
		ccp = cp;
	}
	vp = global(ccp);

	/* Note: AT&T ksh allows set -A but not set +A of a read-only var */
	if ((vp->flag&RDONLY))
		errorfx(2, "read-only: %s", ccp);
	/* This code is quite non-optimal */
	if (reset) {
		/* trash existing values and attributes */
		unset(vp, 1);
		/* allocate-by-access the [0] element to keep in scope */
		arraysearch(vp, 0);
	}
	/*
	 * TODO: would be nice for assignment to completely succeed or
	 * completely fail. Only really effects integer arrays:
	 * evaluation of some of vals[] may fail...
	 */
	if (cp != NULL) {
		/* find out where to set when appending */
		for (vq = vp; vq; vq = vq->u.array) {
			if (!(vq->flag & ISSET))
				continue;
			if (arrayindex(vq) >= j)
				j = arrayindex(vq) + 1;
		}
		afree(cp, ATEMP);
	}
	while ((ccp = vals[i])) {
#if 0 /* temporarily taken out due to regression */
		if (*ccp == '[') {
			int level = 0;

			while (*ccp) {
				if (*ccp == ']' && --level == 0)
					break;
				if (*ccp == '[')
					++level;
				++ccp;
			}
			if (*ccp == ']' && level == 0 && ccp[1] == '=') {
				strndupx(cp, vals[i] + 1, ccp - (vals[i] + 1),
				    ATEMP);
				evaluate(substitute(cp, 0), (mksh_ari_t *)&j,
				    KSH_UNWIND_ERROR, true);
				afree(cp, ATEMP);
				ccp += 2;
			} else
				ccp = vals[i];
		}
#endif

		vq = arraysearch(vp, j);
		/* would be nice to deal with errors here... (see above) */
		setstr(vq, ccp, KSH_RETURN_ERROR);
		i++;
		j++;
	}

	return (i);
}