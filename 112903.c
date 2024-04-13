static int flush_pkt(git_pkt **out)
{
	git_pkt *pkt;

	pkt = git__malloc(sizeof(git_pkt));
	GITERR_CHECK_ALLOC(pkt);

	pkt->type = GIT_PKT_FLUSH;
	*out = pkt;

	return 0;
}