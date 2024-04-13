int __export rad_packet_add_octets(struct rad_packet_t *pack, const char *vendor_name, const char *name, const uint8_t *val, int len)
{
	struct rad_attr_t *ra;
	struct rad_dict_attr_t *attr;
	struct rad_dict_vendor_t *vendor;

	if (pack->len + (vendor_name ? 8 : 2) + len >= REQ_LENGTH_MAX)
		return -1;

	if (vendor_name) {
		vendor = rad_dict_find_vendor_name(vendor_name);
		if (!vendor)
			return -1;
		attr = rad_dict_find_vendor_attr(vendor, name);
	} else {
		vendor = NULL;
		attr = rad_dict_find_attr(name);
	}

	if (!attr)
		return -1;

	ra = mempool_alloc(attr_pool);
	if (!ra) {
		log_emerg("radius: out of memory\n");
		return -1;
	}

	memset(ra, 0, sizeof(*ra));
	ra->vendor = vendor;
	ra->attr = attr;
	ra->len = len;

	if (len) {
		ra->alloc = 1;
		ra->val.octets = _malloc(len);
		if (!ra->val.octets) {
			log_emerg("radius: out of memory\n");
			_free(ra);
			return -1;
		}
		memcpy(ra->val.octets, val, len);
	}

	ra->raw = ra->val.octets;

	list_add_tail(&ra->entry, &pack->attrs);
	pack->len += (vendor_name ? 8 : 2) + len;

	return 0;
}