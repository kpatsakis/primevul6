static int sideband_progress_pkt(git_pkt **out, const char *line, size_t len)
{
	git_pkt_progress *pkt;
	size_t alloclen;

	line++;
	len--;

	GITERR_CHECK_ALLOC_ADD(&alloclen, sizeof(git_pkt_progress), len);
	pkt = git__malloc(alloclen);
	GITERR_CHECK_ALLOC(pkt);

	pkt->type = GIT_PKT_PROGRESS;
	pkt->len = (int) len;
	memcpy(pkt->data, line, len);

	*out = (git_pkt *) pkt;

	return 0;
}