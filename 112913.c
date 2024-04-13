static int unpack_pkt(git_pkt **out, const char *line, size_t len)
{
	git_pkt_unpack *pkt;

	GIT_UNUSED(len);

	pkt = git__malloc(sizeof(*pkt));
	GITERR_CHECK_ALLOC(pkt);

	pkt->type = GIT_PKT_UNPACK;
	if (!git__prefixcmp(line, "unpack ok"))
		pkt->unpack_ok = 1;
	else
		pkt->unpack_ok = 0;

	*out = (git_pkt *)pkt;
	return 0;
}