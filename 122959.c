static VALUE_PAIR *rad_attr2vp(const RADIUS_PACKET *packet, const RADIUS_PACKET *original,
			const char *secret, int attribute, int length,
			const uint8_t *data)
{
	int offset = 0;
	VALUE_PAIR *vp;

	if ((vp = paircreate(attribute, PW_TYPE_OCTETS)) == NULL) {
		return NULL;
	}
	
	/*
	 *	If length is greater than 253, something is SERIOUSLY
	 *	wrong.
	 */
	if (length > 253) length = 253;	/* paranoia (pair-anoia?) */

	vp->length = length;
	vp->operator = T_OP_EQ;
	vp->next = NULL;

	/*
	 *	Handle tags.
	 */
	if (vp->flags.has_tag) {
		if (TAG_VALID(data[0]) ||
		    (vp->flags.encrypt == FLAG_ENCRYPT_TUNNEL_PASSWORD)) {
			/*
			 *	Tunnel passwords REQUIRE a tag, even
			 *	if don't have a valid tag.
			 */
			vp->flags.tag = data[0];

			if ((vp->type == PW_TYPE_STRING) ||
			    (vp->type == PW_TYPE_OCTETS)) offset = 1;
		}
	}

	/*
	 *	Copy the data to be decrypted
	 */
	memcpy(&vp->strvalue[0], data + offset, length - offset);
	vp->length -= offset;

	/*
	 *	Decrypt the attribute.
	 */
	switch (vp->flags.encrypt) {
		/*
		 *  User-Password
		 */
	case FLAG_ENCRYPT_USER_PASSWORD:
		if (original) {
			rad_pwdecode((char *)vp->strvalue,
				     vp->length, secret,
				     original->vector);
		} else {
			rad_pwdecode((char *)vp->strvalue,
				     vp->length, secret,
				     packet->vector);
		}
		if (vp->attribute == PW_USER_PASSWORD) {
			vp->length = strlen(vp->strvalue);
		}
		break;
		
		/*
		 *	Tunnel-Password's may go ONLY
		 *	in response packets.
		 */
	case FLAG_ENCRYPT_TUNNEL_PASSWORD:
		if (!original) goto raw;
		
		if (rad_tunnel_pwdecode(vp->strvalue, &vp->length,
					secret, original->vector) < 0) {
			goto raw;
		}
		break;
		
		/*
		 *  Ascend-Send-Secret
		 *  Ascend-Receive-Secret
		 */
	case FLAG_ENCRYPT_ASCEND_SECRET:
		if (!original) {
			goto raw;
		} else {
			uint8_t my_digest[AUTH_VECTOR_LEN];
			make_secret(my_digest,
				    original->vector,
				    secret, data);
			memcpy(vp->strvalue, my_digest,
			       AUTH_VECTOR_LEN );
			vp->strvalue[AUTH_VECTOR_LEN] = '\0';
			vp->length = strlen(vp->strvalue);
		}
		break;

	default:
		break;
	} /* switch over encryption flags */


	switch (vp->type) {
	case PW_TYPE_STRING:
	case PW_TYPE_OCTETS:
	case PW_TYPE_ABINARY:
		/* nothing more to do */
		break;

	case PW_TYPE_INTEGER:
		if (vp->length != 4) goto raw;

		memcpy(&vp->lvalue, vp->strvalue, 4);
		vp->lvalue = ntohl(vp->lvalue);

		if (vp->flags.has_tag) vp->lvalue &= 0x00ffffff;

		/*
		 *	Try to get named VALUEs
		 */
		{
			DICT_VALUE *dval;
			dval = dict_valbyattr(vp->attribute,
					      vp->lvalue);
			if (dval) {
				strNcpy(vp->strvalue,
					dval->name,
					sizeof(vp->strvalue));
			}
		}
		break;

	case PW_TYPE_DATE:
		if (vp->length != 4) goto raw;

		memcpy(&vp->lvalue, vp->strvalue, 4);
		vp->lvalue = ntohl(vp->lvalue);
		break;

		/*
		 *	IPv4 address. Keep it in network byte order in
		 *	vp->lvalue and put ASCII IP address in standard
		 *	dot notation into vp->strvalue.
		 */
	case PW_TYPE_IPADDR:
		if (vp->length != 4) goto raw;

		memcpy(&vp->lvalue, vp->strvalue, 4);
		ip_ntoa(vp->strvalue, vp->lvalue);
		break;

		/*
		 *	IPv6 interface ID is 8 octets long.
		 */
	case PW_TYPE_IFID:
		if (vp->length != 8) goto raw;
		/* vp->vp_ifid == vp->strvalue */
		break;
		
		/*
		 *	IPv6 addresses are 16 octets long
		 */
	case PW_TYPE_IPV6ADDR:
		if (vp->length != 16) goto raw;
		/* vp->vp_ipv6addr == vp->strvalue */
		break;
		
		/*
		 *	IPv6 prefixes are 2 to 18 octets long.
		 *
		 *	RFC 3162: The first octet is unused.
		 *	The second is the length of the prefix
		 *	the rest are the prefix data.
		 *
		 *	The prefix length can have value 0 to 128.
		 */
	case PW_TYPE_IPV6PREFIX:
		if (vp->length < 2 || vp->length > 18) goto raw;
		if (vp->strvalue[1] > 128) goto raw;

		/*
		 *	FIXME: double-check that
		 *	(vp->strvalue[1] >> 3) matches vp->length + 2
		 */
		if (vp->length < 18) {
			memset(vp->strvalue + vp->length, 0,
			       18 - vp->length);
		}
		break;

	default:
	raw:
		vp->type = PW_TYPE_OCTETS;
		vp->length = length;
		memcpy(vp->strvalue, data, length);
		

		/*
		 *	Ensure there's no encryption or tag stuff,
		 *	we just pass the attribute as-is.
		 */
		memset(&vp->flags, 0, sizeof(vp->flags));
	}

	return vp;
}