xfs_getbmap_format(
	struct kgetbmap		*p,
	struct getbmapx __user	*u,
	size_t			recsize)
{
	if (put_user(p->bmv_offset, &u->bmv_offset) ||
	    put_user(p->bmv_block, &u->bmv_block) ||
	    put_user(p->bmv_length, &u->bmv_length) ||
	    put_user(0, &u->bmv_count) ||
	    put_user(0, &u->bmv_entries))
		return false;
	if (recsize < sizeof(struct getbmapx))
		return true;
	if (put_user(0, &u->bmv_iflags) ||
	    put_user(p->bmv_oflags, &u->bmv_oflags) ||
	    put_user(0, &u->bmv_unused1) ||
	    put_user(0, &u->bmv_unused2))
		return false;
	return true;
}