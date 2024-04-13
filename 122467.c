int __export rad_packet_add_str(struct rad_packet_t *pack, const char *vendor_name, const char *name, const char *val)
{
	struct rad_attr_t *ra;
	struct rad_dict_attr_t *attr;
	struct rad_dict_vendor_t *vendor;
	int len = strlen(val);

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
	ra->alloc = 1;
	ra->val.string = _malloc(len + 1);
	if (!ra->val.string) {
		log_emerg("radius: out of memory\n");
		_free(ra);
		return -1;
	}
	memcpy(ra->val.string, val, len);
	ra->val.string[len] = 0;
	ra->raw = ra->val.string;
	list_add_tail(&ra->entry, &pack->attrs);
	pack->len += (vendor_name ? 8 : 2) + len;

	return 0;
}