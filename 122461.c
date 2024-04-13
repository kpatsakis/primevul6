int __export rad_packet_change_octets(struct rad_packet_t *pack, const char *vendor_name, const char *name, const uint8_t *val, int len)
{
	struct rad_attr_t *ra;

	ra = rad_packet_find_attr(pack, vendor_name, name);
	if (!ra)
		return -1;

	if (ra->len != len) {
		if (pack->len - ra->len + len >= REQ_LENGTH_MAX)
			return -1;

		ra->val.octets = _realloc(ra->val.octets, len);
		if (len && !ra->val.octets) {
			log_emerg("radius: out of memory\n");
			return -1;
		}

		ra->raw = ra->val.octets;

		pack->len += len - ra->len;
		ra->len = len;
	}

	if (len)
		memcpy(ra->val.octets, val, len);

	return 0;
}