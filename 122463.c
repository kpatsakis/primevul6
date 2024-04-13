int __export rad_packet_change_str(struct rad_packet_t *pack, const char *vendor_name, const char *name, const char *val, int len)
{
	struct rad_attr_t *ra;

	ra = rad_packet_find_attr(pack, vendor_name, name);
	if (!ra)
		return -1;

	if (ra->len != len) {
		if (pack->len - ra->len + len >= REQ_LENGTH_MAX)
			return -1;

		ra->val.string = _realloc(ra->val.string, len + 1);
		if (!ra->val.string) {
			log_emerg("radius: out of memory\n");
			return -1;
		}

		ra->raw = ra->val.string;

		pack->len += len - ra->len;
		ra->len = len;
	}

	memcpy(ra->val.string, val, len);
	ra->val.string[len] = 0;

	return 0;
}