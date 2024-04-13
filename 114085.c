static inline int htx_copy_msg(struct htx *htx, const struct buffer *msg)
{
	/* The destination HTX message is empty, we can do a raw copy */
	if (htx_is_empty(htx)) {
		memcpy(htx, msg->area, msg->size);
		return 1;
	}

	/* Otherwise, we need to append the HTX message */
	return htx_append_msg(htx, htxbuf(msg));
}