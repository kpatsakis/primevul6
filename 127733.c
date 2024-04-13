makenv(void)
{
	ssize_t i;
	struct block *l;
	XPtrV denv;
	struct tbl *vp, **vpp;

	XPinit(denv, 64);
	for (l = e->loc; l != NULL; l = l->next) {
		vpp = l->vars.tbls;
		i = 1 << (l->vars.tshift);
		while (--i >= 0)
			if ((vp = *vpp++) != NULL &&
			    (vp->flag&(ISSET|EXPORT)) == (ISSET|EXPORT)) {
				struct block *l2;
				struct tbl *vp2;
				uint32_t h = hash(vp->name);

				/* unexport any redefined instances */
				for (l2 = l->next; l2 != NULL; l2 = l2->next) {
					vp2 = ktsearch(&l2->vars, vp->name, h);
					if (vp2 != NULL)
						vp2->flag &= ~EXPORT;
				}
				if ((vp->flag&INTEGER)) {
					/* integer to string */
					char *val;
					val = str_val(vp);
					vp->flag &= ~(INTEGER|RDONLY|SPECIAL);
					/* setstr can't fail here */
					setstr(vp, val, KSH_RETURN_ERROR);
				}
				XPput(denv, vp->val.s);
			}
	}
	XPput(denv, NULL);
	return ((char **)XPclose(denv));
}