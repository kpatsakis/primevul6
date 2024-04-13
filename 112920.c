static int ok_pkt(git_pkt **out, const char *line, size_t len)
{
	git_pkt_ok *pkt;
	const char *ptr;
	size_t alloc_len;

	pkt = git__malloc(sizeof(*pkt));
	GITERR_CHECK_ALLOC(pkt);

	pkt->type = GIT_PKT_OK;

	line += 3; /* skip "ok " */
	if (!(ptr = strchr(line, '\n'))) {
		giterr_set(GITERR_NET, "Invalid packet line");
		git__free(pkt);
		return -1;
	}
	len = ptr - line;

	GITERR_CHECK_ALLOC_ADD(&alloc_len, len, 1);
	pkt->ref = git__malloc(alloc_len);
	GITERR_CHECK_ALLOC(pkt->ref);

	memcpy(pkt->ref, line, len);
	pkt->ref[len] = '\0';

	*out = (git_pkt *)pkt;
	return 0;
}