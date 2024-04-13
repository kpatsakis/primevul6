struct rad_attr_t __export *rad_packet_find_attr(struct rad_packet_t *pack, const char *vendor_name, const char *name)
{
	struct rad_attr_t *ra;
	struct rad_dict_vendor_t *vendor;

	if (vendor_name) {
		vendor = rad_dict_find_vendor_name(vendor_name);
		if (!vendor)
			return NULL;
	} else
		vendor = NULL;

	list_for_each_entry(ra, &pack->attrs, entry) {
		if (vendor && vendor != ra->vendor)
			continue;

		if (strcmp(ra->attr->name, name))
			continue;

		return ra;
	}

	return NULL;
}