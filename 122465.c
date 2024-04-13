int __export rad_packet_change_val(struct rad_packet_t *pack, const char *vendor_name, const char *name, const char *val)
{
	struct rad_attr_t *ra;
	struct rad_dict_value_t *v;

	ra = rad_packet_find_attr(pack, vendor_name, name);
	if (!ra)
		return -1;

	v = rad_dict_find_val_name(ra->attr, val);
	if (!v)
		return -1;

	ra->val = v->val;

	return 0;
}