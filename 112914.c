static int buffer_want_with_caps(const git_remote_head *head, transport_smart_caps *caps, git_buf *buf)
{
	git_buf str = GIT_BUF_INIT;
	char oid[GIT_OID_HEXSZ +1] = {0};
	size_t len;

	/* Prefer multi_ack_detailed */
	if (caps->multi_ack_detailed)
		git_buf_puts(&str, GIT_CAP_MULTI_ACK_DETAILED " ");
	else if (caps->multi_ack)
		git_buf_puts(&str, GIT_CAP_MULTI_ACK " ");

	/* Prefer side-band-64k if the server supports both */
	if (caps->side_band_64k)
		git_buf_printf(&str, "%s ", GIT_CAP_SIDE_BAND_64K);
	else if (caps->side_band)
		git_buf_printf(&str, "%s ", GIT_CAP_SIDE_BAND);

	if (caps->include_tag)
		git_buf_puts(&str, GIT_CAP_INCLUDE_TAG " ");

	if (caps->thin_pack)
		git_buf_puts(&str, GIT_CAP_THIN_PACK " ");

	if (caps->ofs_delta)
		git_buf_puts(&str, GIT_CAP_OFS_DELTA " ");

	if (git_buf_oom(&str))
		return -1;

	len = strlen("XXXXwant ") + GIT_OID_HEXSZ + 1 /* NUL */ +
		 git_buf_len(&str) + 1 /* LF */;

	if (len > 0xffff) {
		giterr_set(GITERR_NET,
			"Tried to produce packet with invalid length %" PRIuZ, len);
		return -1;
	}

	git_buf_grow_by(buf, len);
	git_oid_fmt(oid, &head->oid);
	git_buf_printf(buf,
		"%04xwant %s %s\n", (unsigned int)len, oid, git_buf_cstr(&str));
	git_buf_free(&str);

	GITERR_CHECK_ALLOC_BUF(buf);

	return 0;
}