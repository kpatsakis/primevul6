int rad_packet_build(struct rad_packet_t *pack, uint8_t *RA)
{
	struct rad_attr_t *attr;
	uint8_t *ptr;

	if (!pack->buf) {
		//ptr = mmap(NULL, REQ_LENGTH_MAX, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
		ptr = mempool_alloc(buf_pool);

		//if (ptr == MAP_FAILED) {
		if (!ptr) {
			log_emerg("radius:packet: out of memory\n");
			return -1;
		}

		pack->buf = ptr;
	} else
		ptr = pack->buf;

	*ptr = pack->code; ptr++;
	*ptr = pack->id; ptr++;
	*(uint16_t*)ptr = htons(pack->len); ptr+= 2;
	memcpy(ptr, RA, 16);	ptr+=16;

	list_for_each_entry(attr, &pack->attrs, entry) {
		if (attr->vendor) {
			*ptr = 26; ptr++;
			*ptr = attr->len + 2 + 6; ptr++;
			*(uint32_t *)ptr = htonl(attr->vendor->id); ptr+=4;
		}
		*ptr = attr->attr->id; ptr++;
		*ptr = attr->len + 2; ptr++;
		switch(attr->attr->type) {
			case ATTR_TYPE_INTEGER:
				*(uint32_t*)ptr = htonl(attr->val.integer);
				break;
			case ATTR_TYPE_OCTETS:
			case ATTR_TYPE_STRING:
				memcpy(ptr, attr->val.string, attr->len);
				break;
			case ATTR_TYPE_IPADDR:
			case ATTR_TYPE_IFID:
			case ATTR_TYPE_IPV6ADDR:
				memcpy(ptr, &attr->val, attr->len);
				break;
			case ATTR_TYPE_DATE:
				*(uint32_t*)ptr = htonl(attr->val.date);
				break;
			case ATTR_TYPE_IPV6PREFIX:
				ptr[0] = 0;
				ptr[1] = attr->val.ipv6prefix.len;
				memcpy(ptr + 2, &attr->val.ipv6prefix.prefix, sizeof(attr->val.ipv6prefix.prefix));
				break;
			default:
				log_emerg("radius:packet:BUG: unknown attribute type\n");
				abort();
		}
		ptr += attr->len;
	}

	//print_buf(pack->buf, pack->len);
	return 0;
}