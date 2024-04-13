local(const char *n, bool copy)
{
	struct block *l = e->loc;
	struct tbl *vp;
	bool array;
	uint32_t h, val;

	/*
	 * check to see if this is an array;
	 * dereference namerefs; must come first
	 */
	n = array_index_calc(n, &array, &val);
	mkssert(n != NULL);
	h = hash(n);
	if (!ksh_isalphx(*n)) {
		vp = &vtemp;
		vp->flag = DEFINED|RDONLY;
		vp->type = 0;
		vp->areap = ATEMP;
		return (vp);
	}
	vp = ktenter(&l->vars, n, h);
	if (copy && !(vp->flag & DEFINED)) {
		struct tbl *vq;

		varsearch(l->next, &vq, n, h);
		if (vq != NULL) {
			vp->flag |= vq->flag &
			    (EXPORT | INTEGER | RDONLY | LJUST | RJUST |
			    ZEROFIL | LCASEV | UCASEV_AL | INT_U | INT_L);
			if (vq->flag & INTEGER)
				vp->type = vq->type;
			vp->u2.field = vq->u2.field;
		}
	}
	if (array)
		vp = arraysearch(vp, val);
	vp->flag |= DEFINED;
	if (special(n))
		vp->flag |= SPECIAL;
	return (vp);
}