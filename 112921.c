static int sideband_error_pkt(git_pkt **out, const char *line, size_t len)
{
	git_pkt_err *pkt;
	size_t alloc_len;

	line++;
	len--;

	GITERR_CHECK_ALLOC_ADD(&alloc_len, sizeof(git_pkt_err), len);
	GITERR_CHECK_ALLOC_ADD(&alloc_len, alloc_len, 1);
	pkt = git__malloc(alloc_len);
	GITERR_CHECK_ALLOC(pkt);

	pkt->type = GIT_PKT_ERR;
	pkt->len = (int)len;
	memcpy(pkt->error, line, len);
	pkt->error[len] = '\0';

	*out = (git_pkt *)pkt;

	return 0;
}