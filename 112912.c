static int comment_pkt(git_pkt **out, const char *line, size_t len)
{
	git_pkt_comment *pkt;
	size_t alloclen;

	GITERR_CHECK_ALLOC_ADD(&alloclen, sizeof(git_pkt_comment), len);
	GITERR_CHECK_ALLOC_ADD(&alloclen, alloclen, 1);
	pkt = git__malloc(alloclen);
	GITERR_CHECK_ALLOC(pkt);

	pkt->type = GIT_PKT_COMMENT;
	memcpy(pkt->comment, line, len);
	pkt->comment[len] = '\0';

	*out = (git_pkt *) pkt;

	return 0;
}