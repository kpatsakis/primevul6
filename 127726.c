global(const char *n)
{
	struct block *l = e->loc;
	struct tbl *vp;
	int c;
	bool array;
	uint32_t h, val;

	/*
	 * check to see if this is an array;
	 * dereference namerefs; must come first
	 */
	n = array_index_calc(n, &array, &val);
	h = hash(n);
	c = (unsigned char)n[0];
	if (!ksh_isalphx(c)) {
		if (array)
			errorf("bad substitution");
		vp = &vtemp;
		vp->flag = DEFINED;
		vp->type = 0;
		vp->areap = ATEMP;
		*vp->name = c;
		if (ksh_isdigit(c)) {
			if (getn(n, &c) && (c <= l->argc))
				/* setstr can't fail here */
				setstr(vp, l->argv[c], KSH_RETURN_ERROR);
			vp->flag |= RDONLY;
			return (vp);
		}
		vp->flag |= RDONLY;
		if (n[1] != '\0')
			return (vp);
		vp->flag |= ISSET|INTEGER;
		switch (c) {
		case '$':
			vp->val.i = kshpid;
			break;
		case '!':
			/* if no job, expand to nothing */
			if ((vp->val.i = j_async()) == 0)
				vp->flag &= ~(ISSET|INTEGER);
			break;
		case '?':
			vp->val.i = exstat & 0xFF;
			break;
		case '#':
			vp->val.i = l->argc;
			break;
		case '-':
			vp->flag &= ~INTEGER;
			vp->val.s = getoptions();
			break;
		default:
			vp->flag &= ~(ISSET|INTEGER);
		}
		return (vp);
	}
	l = varsearch(e->loc, &vp, n, h);
	if (vp != NULL)
		return (array ? arraysearch(vp, val) : vp);
	vp = ktenter(&l->vars, n, h);
	if (array)
		vp = arraysearch(vp, val);
	vp->flag |= DEFINED;
	if (special(n))
		vp->flag |= SPECIAL;
	return (vp);
}