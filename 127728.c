varsearch(struct block *l, struct tbl **vpp, const char *vn, uint32_t h)
{
	register struct tbl *vp;

	if (l) {
 varsearch_loop:
		if ((vp = ktsearch(&l->vars, vn, h)) != NULL)
			goto varsearch_out;
		if (l->next != NULL) {
			l = l->next;
			goto varsearch_loop;
		}
	}
	vp = NULL;
 varsearch_out:
	*vpp = vp;
	return (l);
}