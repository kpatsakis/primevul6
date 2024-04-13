static int err_pkt(git_pkt **out, const char *line, size_t len)
{
	git_pkt_err *pkt;
	size_t alloclen;

	/* Remove "ERR " from the line */
	line += 4;
	len -= 4;

	GITERR_CHECK_ALLOC_ADD(&alloclen, sizeof(git_pkt_progress), len);
	GITERR_CHECK_ALLOC_ADD(&alloclen, alloclen, 1);
	pkt = git__malloc(alloclen);
	GITERR_CHECK_ALLOC(pkt);

	pkt->type = GIT_PKT_ERR;
	pkt->len = (int)len;
	memcpy(pkt->error, line, len);
	pkt->error[len] = '\0';

	*out = (git_pkt *) pkt;

	return 0;
}