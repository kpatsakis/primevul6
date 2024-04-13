int __export rad_packet_add_val(struct rad_packet_t *pack, const char *vendor_name, const char *name, const char *val)
{
	struct rad_attr_t *ra;
	struct rad_dict_attr_t *attr;
	struct rad_dict_value_t *v;
	struct rad_dict_vendor_t *vendor;

	if (pack->len + (vendor_name ? 8 : 2) + 4 >= REQ_LENGTH_MAX)
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

	v = rad_dict_find_val_name(attr, val);
	if (!v)
		return -1;

	ra = mempool_alloc(attr_pool);
	if (!ra)
		return -1;

	memset(ra, 0, sizeof(*ra));
	ra->vendor = vendor;
	ra->attr = attr;
	ra->len = 4;
	ra->val = v->val;
	ra->raw = &ra->val;
	list_add_tail(&ra->entry, &pack->attrs);
	pack->len += (vendor_name ? 8 : 2) + 4;

	return 0;
}