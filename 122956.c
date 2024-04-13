int rad_decode(RADIUS_PACKET *packet, RADIUS_PACKET *original,
	       const char *secret)
{
	uint32_t		lvalue;
	uint32_t		vendorcode;
	VALUE_PAIR		**tail;
	VALUE_PAIR		*pair;
	uint8_t			*ptr;
	int			packet_length;
	int			attribute;
	int			attrlen;
	int			vendorlen;
	radius_packet_t		*hdr;
	int			vsa_tlen, vsa_llen;
	DICT_VENDOR		*dv = NULL;

	/*
	 *	Extract attribute-value pairs
	 */
	hdr = (radius_packet_t *)packet->data;
	ptr = hdr->data;
	packet_length = packet->data_len - AUTH_HDR_LEN;

	/*
	 *	There may be VP's already in the packet.  Don't
	 *	destroy them.
	 */
	for (tail = &packet->vps; *tail != NULL; tail = &((*tail)->next)) {
		/* nothing */
	}

	vendorcode = 0;
	vendorlen  = 0;
	vsa_tlen = vsa_llen = 1;

	/*
	 *	We have to read at least two bytes.
	 *
	 *	rad_recv() above ensures that this is OK.
	 */
	while (packet_length > 0) {
		attribute = -1;
		attrlen = -1;

		/*
		 *	Normal attribute, handle it like normal.
		 */
		if (vendorcode == 0) {
			/*
			 *	No room to read attr/length,
			 *	or bad attribute, or attribute is
			 *	too short, or attribute is too long,
			 *	stop processing the packet.
			 */
			if ((packet_length < 2) ||
			    (ptr[0] == 0) ||  (ptr[1] < 2) ||
			    (ptr[1] > packet_length)) break;

			attribute = *ptr++;
			attrlen   = *ptr++;

			attrlen -= 2;
			packet_length  -= 2;

			if (attribute != PW_VENDOR_SPECIFIC) goto create_pair;
			
			/*
			 *	No vendor code, or ONLY vendor code.
			 */
			if (attrlen <= 4) goto create_pair;

			vendorlen = 0;
		}
		
		/*
		 *	Handle Vendor-Specific
		 */
		if (vendorlen == 0) {
			uint8_t *subptr;
			int sublen;
			int myvendor;
			
			/*
			 *	attrlen was checked above.
			 */
			memcpy(&lvalue, ptr, 4);
			myvendor = ntohl(lvalue);

			/*
			 *	Zero isn't allowed.
			 */
			if (myvendor == 0) goto create_pair;
			
			/*
			 *	This is an implementation issue.
			 *	We currently pack vendor into the upper
			 *	16 bits of a 32-bit attribute number,
			 *	so we can't handle vendor numbers larger
			 *	than 16 bits.
			 */
			if (myvendor > 65535) goto create_pair;
			
			vsa_tlen = vsa_llen = 1;
			dv = dict_vendorbyvalue(myvendor);
			if (dv) {
				vsa_tlen = dv->type;
				vsa_llen = dv->length;
			}
			
			/*
			 *	Sweep through the list of VSA's,
			 *	seeing if they exactly fill the
			 *	outer Vendor-Specific attribute.
			 *
			 *	If not, create a raw Vendor-Specific.
			 */
			subptr = ptr + 4;
			sublen = attrlen - 4;

			/*
			 *	See if we can parse it.
			 */
			do {
				int myattr = 0;

				/*
				 *	Don't have a type, it's bad.
				 */
				if (sublen < vsa_tlen) goto create_pair;
				
				/*
				 *	Ensure that the attribute number
				 *	is OK.
				 */
				switch (vsa_tlen) {
				case 1:
					myattr = subptr[0];
					break;
					
				case 2:
					myattr = (subptr[0] << 8) | subptr[1];
					break;
					
				case 4:
					if ((subptr[0] != 0) ||
					    (subptr[1] != 0)) goto create_pair;
					
					myattr = (subptr[2] << 8) | subptr[3];
					break;
					
					/*
					 *	Our dictionary is broken.
					 */
				default:
					goto create_pair;
				}
				
				/*
				 *	Not enough room for one more
				 *	attribute.  Die!
				 */
				if (sublen < vsa_tlen + vsa_llen) goto create_pair;
				switch (vsa_llen) {
				case 0:
					attribute = (myvendor << 16) | myattr;
					ptr += 4 + vsa_tlen;
					attrlen -= (4 + vsa_tlen);
					packet_length -= 4 + vsa_tlen;
					goto create_pair;

				case 1:
					if (subptr[vsa_tlen] < (vsa_tlen + vsa_llen))
						goto create_pair;

					if (subptr[vsa_tlen] > sublen)
						goto create_pair;
					sublen -= subptr[vsa_tlen];
					subptr += subptr[vsa_tlen];
					break;

				case 2:
					if (subptr[vsa_tlen] != 0) goto create_pair;
					if (subptr[vsa_tlen + 1] < (vsa_tlen + vsa_llen))
						goto create_pair;
					if (subptr[vsa_tlen + 1] > sublen)
						goto create_pair;
					sublen -= subptr[vsa_tlen + 1];
					subptr += subptr[vsa_tlen + 1];
					break;

					/*
					 *	Our dictionaries are
					 *	broken.
					 */
				default:
					goto create_pair;
				}
			} while (sublen > 0);

			vendorcode = myvendor;
			vendorlen = attrlen - 4;
			packet_length -= 4;

			ptr += 4;
		}

		/*
		 *	attrlen is the length of this attribute.
		 *	total_len is the length of the encompassing
		 *	attribute.
		 */
		switch (vsa_tlen) {
		case 1:
			attribute = ptr[0];
			break;
			
		case 2:
			attribute = (ptr[0] << 8) | ptr[1];
			break;

		default:	/* can't hit this. */
			return -1;
		}
		attribute |= (vendorcode << 16);
		ptr += vsa_tlen;

		switch (vsa_llen) {
		case 1:
			attrlen = ptr[0] - (vsa_tlen + vsa_llen);
			break;
			
		case 2:
			attrlen = ptr[1] - (vsa_tlen + vsa_llen);
			break;

		default:	/* can't hit this. */
			return -1;
		}
		ptr += vsa_llen;
		vendorlen -= vsa_tlen + vsa_llen + attrlen;
		if (vendorlen == 0) vendorcode = 0;
		packet_length -= (vsa_tlen + vsa_llen);

		/*
		 *	Create the attribute, setting the default type
		 *	to 'octects'.  If the type in the dictionary
		 *	is different, then the dictionary type will
		 *	over-ride this one.
		 */
	create_pair:
		/*
		 *	Ignore VSAs that have no data.
		 */
		if (attrlen == 0) goto next;

		pair = rad_attr2vp(packet, original, secret,
				 attribute, attrlen, ptr);
		if (!pair) {
			pairfree(&packet->vps);
			librad_log("out of memory");
			return -1;
		}

		debug_pair(pair);
		*tail = pair;
		tail = &pair->next;

	next:
		ptr += attrlen;
		packet_length -= attrlen;
	}

	/*
	 *	Merge information from the outside world into our
	 *	random pool.
	 */
	lrad_rand_seed(packet->data, AUTH_HDR_LEN);
	  
	return 0;
}