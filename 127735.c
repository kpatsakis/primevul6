exportprep(struct tbl *vp, const char *val)
{
	char *xp;
	char *op = (vp->flag&ALLOC) ? vp->val.s : NULL;
	size_t namelen, vallen;

	mkssert(val != NULL);

	namelen = strlen(vp->name);
	vallen = strlen(val) + 1;

	vp->flag |= ALLOC;
	/* since name+val are both in memory this can go unchecked */
	xp = alloc(namelen + 1 + vallen, vp->areap);
	memcpy(vp->val.s = xp, vp->name, namelen);
	xp += namelen;
	*xp++ = '=';
	/* offset to value */
	vp->type = xp - vp->val.s;
	memcpy(xp, val, vallen);
	if (op != NULL)
		afree(op, vp->areap);
}