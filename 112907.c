static int ref_pkt(git_pkt **out, const char *line, size_t len)
{
	int error;
	git_pkt_ref *pkt;
	size_t alloclen;

	pkt = git__malloc(sizeof(git_pkt_ref));
	GITERR_CHECK_ALLOC(pkt);

	memset(pkt, 0x0, sizeof(git_pkt_ref));
	pkt->type = GIT_PKT_REF;
	if ((error = git_oid_fromstr(&pkt->head.oid, line)) < 0)
		goto error_out;

	/* Check for a bit of consistency */
	if (line[GIT_OID_HEXSZ] != ' ') {
		giterr_set(GITERR_NET, "Error parsing pkt-line");
		error = -1;
		goto error_out;
	}

	/* Jump from the name */
	line += GIT_OID_HEXSZ + 1;
	len -= (GIT_OID_HEXSZ + 1);

	if (line[len - 1] == '\n')
		--len;

	GITERR_CHECK_ALLOC_ADD(&alloclen, len, 1);
	pkt->head.name = git__malloc(alloclen);
	GITERR_CHECK_ALLOC(pkt->head.name);

	memcpy(pkt->head.name, line, len);
	pkt->head.name[len] = '\0';

	if (strlen(pkt->head.name) < len) {
		pkt->capabilities = strchr(pkt->head.name, '\0') + 1;
	}

	*out = (git_pkt *)pkt;
	return 0;

error_out:
	git__free(pkt);
	return error;
}