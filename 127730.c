popblock(void)
{
	ssize_t i;
	struct block *l = e->loc;
	struct tbl *vp, **vpp = l->vars.tbls, *vq;

	/* pop block */
	e->loc = l->next;

	i = 1 << (l->vars.tshift);
	while (--i >= 0)
		if ((vp = *vpp++) != NULL && (vp->flag&SPECIAL)) {
			if ((vq = global(vp->name))->flag & ISSET)
				setspec(vq);
			else
				unsetspec(vq);
		}
	if (l->flags & BF_DOGETOPTS)
		user_opt = l->getopts_state;
	afreeall(&l->area);
	afree(l, ATEMP);
}