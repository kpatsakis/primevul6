void rad_packet_print(struct rad_packet_t *pack, struct rad_server_t *s, void (*print)(const char *fmt, ...))
{
	struct rad_attr_t *attr;
	struct rad_dict_value_t *val;
	char ip_str[50];
	union {
		uint64_t ifid;
		uint16_t u16[4];
	} ifid_u;
	in_addr_t addr;

	if (s)
		print("[RADIUS(%i) ", s->id);
	else
		print("[RADIUS ");

	switch(pack->code) {
		case CODE_ACCESS_REQUEST:
			print("Access-Request");
			break;
		case CODE_ACCESS_CHALLENGE:
			print("Access-Challenge");
			break;
		case CODE_ACCESS_ACCEPT:
			print("Access-Accept");
			break;
		case CODE_ACCESS_REJECT:
			print("Access-Reject");
			break;
		case CODE_ACCOUNTING_REQUEST:
			print("Accounting-Request");
			break;
		case CODE_ACCOUNTING_RESPONSE:
			print("Accounting-Response");
			break;
		case CODE_DISCONNECT_REQUEST:
			print("Disconnect-Request");
			break;
		case CODE_DISCONNECT_ACK:
			print("Disconnect-ACK");
			break;
		case CODE_DISCONNECT_NAK:
			print("Disconnect-NAK");
			break;
		case CODE_COA_REQUEST:
			print("CoA-Request");
			break;
		case CODE_COA_ACK:
			print("CoA-ACK");
			break;
		case CODE_COA_NAK:
			print("CoA-NAK");
			break;
		default:
			print("Unknown (%i)", pack->code);
	}

	print(" id=%x", pack->id);

	list_for_each_entry(attr, &pack->attrs, entry) {
		print(" <%s", attr->attr->name);

		if (attr->attr->array)
			print_octets(attr->raw, attr->len, print);
		else {
			switch (attr->attr->type) {
				case ATTR_TYPE_INTEGER:
					val = rad_dict_find_val(attr->attr, attr->val);
					if (val)
						print(" %s", val->name);
					else
						print(" %u", attr->val.integer);
					break;
				case ATTR_TYPE_STRING:
					print(" \"%s\"", attr->val.string);
					break;
				case ATTR_TYPE_IPADDR:
					addr = ntohl(attr->val.ipaddr);
					print(" %i.%i.%i.%i", (addr >> 24) & 0xff, (addr >> 16) & 0xff, (addr >> 8) & 0xff, addr & 0xff);
					break;
				case ATTR_TYPE_IFID:
					ifid_u.ifid = attr->val.ifid;
					print(" %x:%x:%x:%x", ntohs(ifid_u.u16[0]), ntohs(ifid_u.u16[1]), ntohs(ifid_u.u16[2]), ntohs(ifid_u.u16[3]));
					break;
				case ATTR_TYPE_IPV6ADDR:
					inet_ntop(AF_INET6, &attr->val.ipv6addr, ip_str, sizeof(ip_str));
					print(" %s", ip_str);
					break;
				case ATTR_TYPE_IPV6PREFIX:
					inet_ntop(AF_INET6, &attr->val.ipv6prefix.prefix, ip_str, sizeof(ip_str));
					print(" %s/%i", ip_str, attr->val.ipv6prefix.len);
					break;
				default:
					print_octets(attr->raw, attr->len, print);
			}
		}

		print(">");
	}

	print("]\n");
}