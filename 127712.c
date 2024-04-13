setint_n(struct tbl *vq, mksh_ari_t num, int newbase)
{
	if (!(vq->flag & INTEGER) && (vq->flag & ALLOC)) {
		vq->flag &= ~ALLOC;
		vq->type = 0;
		afree(vq->val.s, vq->areap);
	}
	vq->val.i = num;
	if (newbase != 0)
		vq->type = newbase;
	vq->flag |= ISSET|INTEGER;
	if (vq->flag&SPECIAL)
		setspec(vq);
}