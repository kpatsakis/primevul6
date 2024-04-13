int fr_dhcp_decode(RADIUS_PACKET *packet)
{
	int i;
	uint8_t *p, *next;
	uint32_t giaddr;
	VALUE_PAIR *head, *vp, **tail;
	VALUE_PAIR *maxms, *mtu;
	char buffer[2048];

	head = NULL;
	tail = &head;
	p = packet->data;
	
	if ((fr_debug_flag > 2) && fr_log_fp) {
		for (i = 0; i < packet->data_len; i++) {
			if ((i & 0x0f) == 0x00) fr_strerror_printf("%d: ", i);
			fprintf(fr_log_fp, "%02x ", packet->data[i]);
			if ((i & 0x0f) == 0x0f) fprintf(fr_log_fp, "\n");
		}
		fprintf(fr_log_fp, "\n");
	}

	if (packet->data[1] != 1) {
		fr_strerror_printf("Packet is not Ethernet: %u",
		      packet->data[1]);
		return -1;
	}

	/*
	 *	Decode the header.
	 */
	for (i = 0; i < 14; i++) {
		vp = pairmake(dhcp_header_names[i], NULL, T_OP_EQ);
		if (!vp) {
			fr_strerror_printf("Parse error %s", fr_strerror());
			pairfree(&head);
			return -1;
		}

		if ((i == 11) && 
		    (packet->data[1] == 1) &&
		    (packet->data[2] == 6)) {
			vp->type = PW_TYPE_ETHERNET;
		}

		switch (vp->type) {
		case PW_TYPE_BYTE:
			vp->vp_integer = p[0];
			vp->length = 1;
			break;
			
		case PW_TYPE_SHORT:
			vp->vp_integer = (p[0] << 8) | p[1];
			vp->length = 2;
			break;
			
		case PW_TYPE_INTEGER:
			memcpy(&vp->vp_integer, p, 4);
			vp->vp_integer = ntohl(vp->vp_integer);
			vp->length = 4;
			break;
			
		case PW_TYPE_IPADDR:
			memcpy(&vp->vp_ipaddr, p, 4);
			vp->length = 4;
			break;
			
		case PW_TYPE_STRING:
			memcpy(vp->vp_strvalue, p, dhcp_header_sizes[i]);
			vp->vp_strvalue[dhcp_header_sizes[i]] = '\0';
			vp->length = strlen(vp->vp_strvalue);
			if (vp->length == 0) {
				pairfree(&vp);
			}
			break;
			
		case PW_TYPE_OCTETS:
			memcpy(vp->vp_octets, p, packet->data[2]);
			vp->length = packet->data[2];
			break;
			
		case PW_TYPE_ETHERNET:
			memcpy(vp->vp_ether, p, sizeof(vp->vp_ether));
			vp->length = sizeof(vp->vp_ether);
			break;
			
		default:
			fr_strerror_printf("BAD TYPE %d", vp->type);
			pairfree(&vp);
			break;
		}
		p += dhcp_header_sizes[i];

		if (!vp) continue;
		
		if (fr_debug_flag > 1) {
			vp_prints(buffer, sizeof(buffer), vp);
			fr_strerror_printf("\t%s", buffer);
		}
		*tail = vp;
		tail = &vp->next;
	}
	
	/*
	 *	Loop over the options.
	 */
	next = packet->data + 240;

	/*
	 *	FIXME: This should also check sname && file fields.
	 *	See the dhcp_get_option() function above.
	 */
	while (next < (packet->data + packet->data_len)) {
		int num_entries, alen;
		DICT_ATTR *da;
		
		p = next;

		if (*p == 0) break;
		if (*p == 255) break; /* end of options signifier */
		if ((p + 2) > (packet->data + packet->data_len)) break;

		next = p + 2 + p[1];

		if (p[1] >= 253) {
			fr_strerror_printf("Attribute too long %u %u",
			      p[0], p[1]);
			continue;
		}
				
		da = dict_attrbyvalue(DHCP2ATTR(p[0]));
		if (!da) {
			fr_strerror_printf("Attribute not in our dictionary: %u",
			      p[0]);
			continue;
		}

		vp = NULL;
		num_entries = 1;
		alen = p[1];
		p += 2;

		/*
		 *	Could be an array of bytes, integers, etc.
		 */
		if (da->flags.array) {
			switch (da->type) {
			case PW_TYPE_BYTE:
				num_entries = alen;
				alen = 1;
				break;

			case PW_TYPE_SHORT: /* ignore any trailing data */
				num_entries = alen >> 1;
				alen = 2;
				break;

			case PW_TYPE_IPADDR:
			case PW_TYPE_INTEGER:
			case PW_TYPE_DATE: /* ignore any trailing data */
				num_entries = alen >> 2;
				alen = 4;
				break;

			default:

				break; /* really an internal sanity failure */
			}
		}

		/*
		 *	Loop over all of the entries, building VPs
		 */
		for (i = 0; i < num_entries; i++) {
			vp = pairmake(da->name, NULL, T_OP_EQ);
			if (!vp) {
				fr_strerror_printf("Cannot build attribute %s",
					fr_strerror());
				pairfree(&head);
				return -1;
			}

			/*
			 *	Hack for ease of use.
			 */
			if ((da->attr == DHCP2ATTR(0x3d)) &&
			    !da->flags.array &&
			    (alen == 7) && (*p == 1) && (num_entries == 1)) {
				vp->type = PW_TYPE_ETHERNET;
				memcpy(vp->vp_octets, p + 1, 6);
				vp->length = alen;

			} else if (fr_dhcp_attr2vp(vp, p, alen) < 0) {
					pairfree(&vp);
					pairfree(&head);
					return -1;
			}

			if (fr_debug_flag > 1) {
				vp_prints(buffer, sizeof(buffer), vp);
				fr_strerror_printf("\t%s", buffer);
			}

			*tail = vp;
			while (*tail) tail = &(*tail)->next;
			p += alen;
		} /* loop over array entries */
	} /* loop over the entire packet */

	/*
	 *	If DHCP request, set ciaddr to zero.
	 */

	/*
	 *	Set broadcast flag for broken vendors, but only if
	 *	giaddr isn't set.
	 */
	memcpy(&giaddr, packet->data + 24, sizeof(giaddr));
	if (giaddr == htonl(INADDR_ANY)) {
		/*
		 *	DHCP Opcode is request
		 */
		vp = pairfind(head, DHCP2ATTR(256));
		if (vp && vp->lvalue == 3) {
			/*
			 *	Vendor is "MSFT 98"
			 */
			vp = pairfind(head, DHCP2ATTR(63));
			if (vp && (strcmp(vp->vp_strvalue, "MSFT 98") == 0)) {
				vp = pairfind(head, DHCP2ATTR(262));

				/*
				 *	Reply should be broadcast.
				 */
				if (vp) vp->lvalue |= 0x8000;
				packet->data[10] |= 0x80;			
			}
		}
	}

	/*
	 *	FIXME: Nuke attributes that aren't used in the normal
	 *	header for discover/requests.
	 */
	packet->vps = head;

	/*
	 *	Client can request a LARGER size, but not a smaller
	 *	one.  They also cannot request a size larger than MTU.
	 */
	maxms = pairfind(packet->vps, DHCP2ATTR(57));
	mtu = pairfind(packet->vps, DHCP2ATTR(26));

	if (mtu && (mtu->vp_integer < DEFAULT_PACKET_SIZE)) {
		fr_strerror_printf("DHCP Fatal: Client says MTU is smaller than minimum permitted by the specification.");
		return -1;
	}

	if (maxms && (maxms->vp_integer < DEFAULT_PACKET_SIZE)) {
		fr_strerror_printf("DHCP WARNING: Client says maximum message size is smaller than minimum permitted by the specification: fixing it");
		maxms->vp_integer = DEFAULT_PACKET_SIZE;
	}

	if (maxms && mtu && (maxms->vp_integer > mtu->vp_integer)) {
		fr_strerror_printf("DHCP WARNING: Client says MTU is smaller than maximum message size: fixing it");
		maxms->vp_integer = mtu->vp_integer;
	}

	if (fr_debug_flag) fflush(stdout);

	return 0;
}