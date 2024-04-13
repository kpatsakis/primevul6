xfs_ioc_attr_put_listent(
	struct xfs_attr_list_context *context,
	int			flags,
	unsigned char		*name,
	int			namelen,
	int			valuelen)
{
	struct xfs_attrlist	*alist = context->buffer;
	struct xfs_attrlist_ent	*aep;
	int			arraytop;

	ASSERT(!context->seen_enough);
	ASSERT(context->count >= 0);
	ASSERT(context->count < (ATTR_MAX_VALUELEN/8));
	ASSERT(context->firstu >= sizeof(*alist));
	ASSERT(context->firstu <= context->bufsize);

	/*
	 * Only list entries in the right namespace.
	 */
	if (context->attr_filter != (flags & XFS_ATTR_NSP_ONDISK_MASK))
		return;

	arraytop = sizeof(*alist) +
			context->count * sizeof(alist->al_offset[0]);

	/* decrement by the actual bytes used by the attr */
	context->firstu -= round_up(offsetof(struct xfs_attrlist_ent, a_name) +
			namelen + 1, sizeof(uint32_t));
	if (context->firstu < arraytop) {
		trace_xfs_attr_list_full(context);
		alist->al_more = 1;
		context->seen_enough = 1;
		return;
	}

	aep = context->buffer + context->firstu;
	aep->a_valuelen = valuelen;
	memcpy(aep->a_name, name, namelen);
	aep->a_name[namelen] = 0;
	alist->al_offset[context->count++] = context->firstu;
	alist->al_count = context->count;
	trace_xfs_attr_list_add(context);
}