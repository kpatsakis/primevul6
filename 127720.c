arraysearch(struct tbl *vp, uint32_t val)
{
	struct tbl *prev, *curr, *news;
	size_t len;

	vp->flag = (vp->flag | (ARRAY | DEFINED)) & ~ASSOC;
	/* the table entry is always [0] */
	if (val == 0)
		return (vp);
	prev = vp;
	curr = vp->u.array;
	while (curr && curr->ua.index < val) {
		prev = curr;
		curr = curr->u.array;
	}
	if (curr && curr->ua.index == val) {
		if (curr->flag&ISSET)
			return (curr);
		news = curr;
	} else
		news = NULL;
	if (!news) {
		len = strlen(vp->name);
		checkoktoadd(len, 1 + offsetof(struct tbl, name[0]));
		news = alloc(offsetof(struct tbl, name[0]) + ++len, vp->areap);
		memcpy(news->name, vp->name, len);
	}
	news->flag = (vp->flag & ~(ALLOC|DEFINED|ISSET|SPECIAL)) | AINDEX;
	news->type = vp->type;
	news->areap = vp->areap;
	news->u2.field = vp->u2.field;
	news->ua.index = val;

	if (curr != news) {
		/* not reusing old array entry */
		prev->u.array = news;
		news->u.array = curr;
	}
	return (news);
}