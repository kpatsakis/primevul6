setint_v(struct tbl *vq, struct tbl *vp, bool arith)
{
	int base;
	mksh_ari_u num;

	if ((base = getint(vp, &num, arith)) == -1)
		return (NULL);
	setint_n(vq, num.i, 0);
	if (vq->type == 0)
		/* default base */
		vq->type = base;
	return (vq);
}