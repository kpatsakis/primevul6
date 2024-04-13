setstr(struct tbl *vq, const char *s, int error_ok)
{
	char *salloc = NULL;
	bool no_ro_check = tobool(error_ok & 0x4);

	error_ok &= ~0x4;
	if ((vq->flag & RDONLY) && !no_ro_check) {
		warningf(true, "read-only: %s", vq->name);
		if (!error_ok)
			errorfxz(2);
		return (0);
	}
	if (!(vq->flag&INTEGER)) {
		/* string dest */
		if ((vq->flag&ALLOC)) {
#ifndef MKSH_SMALL
			/* debugging */
			if (s >= vq->val.s &&
			    s <= vq->val.s + strlen(vq->val.s)) {
				internal_errorf(
				    "setstr: %s=%s: assigning to self",
				    vq->name, s);
			}
#endif
			afree(vq->val.s, vq->areap);
		}
		vq->flag &= ~(ISSET|ALLOC);
		vq->type = 0;
		if (s && (vq->flag & (UCASEV_AL|LCASEV|LJUST|RJUST)))
			s = salloc = formatstr(vq, s);
		if ((vq->flag&EXPORT))
			exportprep(vq, s);
		else {
			strdupx(vq->val.s, s, vq->areap);
			vq->flag |= ALLOC;
		}
	} else {
		/* integer dest */
		if (!v_evaluate(vq, s, error_ok, true))
			return (0);
	}
	vq->flag |= ISSET;
	if ((vq->flag&SPECIAL))
		setspec(vq);
	afree(salloc, ATEMP);
	return (1);
}