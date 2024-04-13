setint(struct tbl *vq, mksh_ari_t n)
{
	if (!(vq->flag&INTEGER)) {
		struct tbl *vp = &vtemp;
		vp->flag = (ISSET|INTEGER);
		vp->type = 0;
		vp->areap = ATEMP;
		vp->val.i = n;
		/* setstr can't fail here */
		setstr(vq, str_val(vp), KSH_RETURN_ERROR);
	} else
		vq->val.i = n;
	vq->flag |= ISSET;
	if ((vq->flag&SPECIAL))
		setspec(vq);
}