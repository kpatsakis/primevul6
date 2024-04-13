void rad_packet_free(struct rad_packet_t *pack)
{
	struct rad_attr_t *attr;

	if (pack->buf)
		mempool_free(pack->buf);
		//munmap(pack->buf, REQ_LENGTH_MAX);

	while(!list_empty(&pack->attrs)) {
		attr = list_entry(pack->attrs.next, typeof(*attr), entry);
		list_del(&attr->entry);
		if (attr->alloc)
			_free(attr->val.string);
		mempool_free(attr);
	}

	mempool_free(pack);
}