unset(struct tbl *vp, int flags)
{
	if (vp->flag & ALLOC)
		afree(vp->val.s, vp->areap);
	if ((vp->flag & ARRAY) && (flags & 1)) {
		struct tbl *a, *tmp;

		/* free up entire array */
		for (a = vp->u.array; a; ) {
			tmp = a;
			a = a->u.array;
			if (tmp->flag & ALLOC)
				afree(tmp->val.s, tmp->areap);
			afree(tmp, tmp->areap);
		}
		vp->u.array = NULL;
	}
	if (flags & 2) {
		vp->flag &= ~(ALLOC|ISSET);
		return;
	}
	/* if foo[0] is being unset, the remainder of the array is kept... */
	vp->flag &= SPECIAL | ((flags & 1) ? 0 : ARRAY|DEFINED);
	if (vp->flag & SPECIAL)
		/* responsible for 'unspecial'ing var */
		unsetspec(vp);
}