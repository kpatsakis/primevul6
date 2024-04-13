void git_pkt_free(git_pkt *pkt)
{
	if (pkt->type == GIT_PKT_REF) {
		git_pkt_ref *p = (git_pkt_ref *) pkt;
		git__free(p->head.name);
		git__free(p->head.symref_target);
	}

	if (pkt->type == GIT_PKT_OK) {
		git_pkt_ok *p = (git_pkt_ok *) pkt;
		git__free(p->ref);
	}

	if (pkt->type == GIT_PKT_NG) {
		git_pkt_ng *p = (git_pkt_ng *) pkt;
		git__free(p->ref);
		git__free(p->msg);
	}

	git__free(pkt);
}