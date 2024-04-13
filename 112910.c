int git_pkt_buffer_wants(
	const git_remote_head * const *refs,
	size_t count,
	transport_smart_caps *caps,
	git_buf *buf)
{
	size_t i = 0;
	const git_remote_head *head;

	if (caps->common) {
		for (; i < count; ++i) {
			head = refs[i];
			if (!head->local)
				break;
		}

		if (buffer_want_with_caps(refs[i], caps, buf) < 0)
			return -1;

		i++;
	}

	for (; i < count; ++i) {
		char oid[GIT_OID_HEXSZ];

		head = refs[i];
		if (head->local)
			continue;

		git_oid_fmt(oid, &head->oid);
		git_buf_put(buf, pkt_want_prefix, strlen(pkt_want_prefix));
		git_buf_put(buf, oid, GIT_OID_HEXSZ);
		git_buf_putc(buf, '\n');
		if (git_buf_oom(buf))
			return -1;
	}

	return git_pkt_buffer_flush(buf);
}