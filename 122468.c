int __export rad_packet_add_ipaddr(struct rad_packet_t *pack, const char *vendor_name, const char *name, in_addr_t ipaddr)
{
	return rad_packet_add_int(pack, vendor_name, name, ipaddr);
}