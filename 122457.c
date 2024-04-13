int __export rad_packet_change_int(struct rad_packet_t *pack, const char *vendor_name, const char *name, int val)
{
	struct rad_attr_t *ra;

	ra = rad_packet_find_attr(pack, vendor_name, name);
	if (!ra)
		return -1;

	ra->val.integer = val;

	return 0;
}