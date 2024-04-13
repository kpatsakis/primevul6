static int ack_pkt(git_pkt **out, const char *line, size_t len)
{
	git_pkt_ack *pkt;
	GIT_UNUSED(line);
	GIT_UNUSED(len);

	pkt = git__calloc(1, sizeof(git_pkt_ack));
	GITERR_CHECK_ALLOC(pkt);

	pkt->type = GIT_PKT_ACK;
	line += 3;
	len -= 3;

	if (len >= GIT_OID_HEXSZ) {
		git_oid_fromstr(&pkt->oid, line + 1);
		line += GIT_OID_HEXSZ + 1;
		len -= GIT_OID_HEXSZ + 1;
	}

	if (len >= 7) {
		if (!git__prefixcmp(line + 1, "continue"))
			pkt->status = GIT_ACK_CONTINUE;
		if (!git__prefixcmp(line + 1, "common"))
			pkt->status = GIT_ACK_COMMON;
		if (!git__prefixcmp(line + 1, "ready"))
			pkt->status = GIT_ACK_READY;
	}

	*out = (git_pkt *) pkt;

	return 0;
}