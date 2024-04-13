int rad_vp2attr(const RADIUS_PACKET *packet, const RADIUS_PACKET *original,
		const char *secret, const VALUE_PAIR *vp, uint8_t *ptr)
{
	int		vendorcode;
	int		offset, len, total_length;
	uint32_t	lvalue;
	uint8_t		*length_ptr, *vsa_length_ptr;
	const uint8_t	*data = NULL;
	uint8_t		array[4];

	vendorcode = total_length = 0;
	length_ptr = vsa_length_ptr = NULL;
	
	/*
	 *	For interoperability, always put vendor attributes
	 *	into their own VSA.
	 */
	if ((vendorcode = VENDOR(vp->attribute)) != 0) {
		/*
		 *	Build a VSA header.
		 */
		*ptr++ = PW_VENDOR_SPECIFIC;
		vsa_length_ptr = ptr;
		*ptr++ = 6;
		lvalue = htonl(vendorcode);
		memcpy(ptr, &lvalue, 4);
		ptr += 4;
		total_length += 6;
		
		if (vendorcode == VENDORPEC_USR) {
			lvalue = htonl(vp->attribute & 0xFFFF);
			memcpy(ptr, &lvalue, 4);
			
			length_ptr = vsa_length_ptr;
			
			total_length += 4;
			*length_ptr  += 4;
			ptr          += 4;
			
			/*
			 *	We don't have two different lengths.
			 */
			vsa_length_ptr = NULL;
			
		} else if (vendorcode == VENDORPEC_LUCENT) {
			/*
			 *	16-bit attribute, 8-bit length
			 */
			*ptr++ = ((vp->attribute >> 8) & 0xFF);
			*ptr++ = (vp->attribute & 0xFF);
			length_ptr = ptr;
			*vsa_length_ptr += 3;
			*ptr++ = 3;
			total_length += 3;

		} else if (vendorcode == VENDORPEC_STARENT) {
			/*
			 *	16-bit attribute, 16-bit length
			 *	with the upper 8 bits of the length
			 *	always zero!
			 */
			*ptr++ = ((vp->attribute >> 8) & 0xFF);
			*ptr++ = (vp->attribute & 0xFF);
			*ptr++ = 0;
			length_ptr = ptr;
			*vsa_length_ptr += 4;
			*ptr++ = 4;
			total_length += 4;
		} else {
			/*
			 *	All other VSA's are encoded the same
			 *	as RFC attributes.
			 */
			*vsa_length_ptr += 2;
			goto rfc;
		}
	} else {
	rfc:
		/*
		 *	All other attributes are encoded as
		 *	per the RFC.
		 */
		*ptr++ = (vp->attribute & 0xFF);
		length_ptr = ptr;
		*ptr++ = 2;
		total_length += 2;
	}

	offset = 0;
	if (vp->flags.has_tag) {
		if (TAG_VALID(vp->flags.tag)) {
			ptr[0] = vp->flags.tag & 0xff;
			offset = 1;
	    
		} else if (vp->flags.encrypt == FLAG_ENCRYPT_TUNNEL_PASSWORD) {
			/*
			 *	Tunnel passwords REQUIRE a tag, even
			 *	if don't have a valid tag.
			 */
			ptr[0] = 0;
			offset = 1;
		} /* else don't write a tag */
	} /* else the attribute doesn't have a tag */
	
	/*
	 *	Set up the default sources for the data.
	 */
	data = vp->strvalue;
	len = vp->length;

	/*
	 *	Encrypted passwords can't be very long.
	 *	This check also ensures that the hashed version
	 *	of the password + attribute header fits into one
	 *	attribute.
	 *
	 *	FIXME: Print a warning message if it's too long?
	 */
	if (vp->flags.encrypt && (len > MAX_PASS_LEN)) {
		len = MAX_PASS_LEN;
	}

	switch(vp->type) {
	case PW_TYPE_STRING:
	case PW_TYPE_OCTETS:
	case PW_TYPE_IFID:
	case PW_TYPE_IPV6ADDR:
	case PW_TYPE_IPV6PREFIX:
	case PW_TYPE_ABINARY:
		/* nothing more to do */
		break;
			
	case PW_TYPE_INTEGER:
		len = 4;	/* just in case */
		lvalue = htonl(vp->lvalue);
		memcpy(array, &lvalue, sizeof(lvalue));

		/*
		 *	Perhaps discard the first octet.
		 */
		data = &array[offset];
		len -= offset;
		break;
			
	case PW_TYPE_IPADDR:
		data = (const uint8_t *) &vp->lvalue;
		len = 4;	/* just in case */
		break;

		/*
		 *  There are no tagged date attributes.
		 */
	case PW_TYPE_DATE:
		lvalue = htonl(vp->lvalue);
		data = (const uint8_t *) &lvalue;
		len = 4;	/* just in case */
		break;

	default:		/* unknown type: ignore it */
		librad_log("ERROR: Unknown attribute type %d", vp->type);
		return -1;
	}

	/*
	 *	Bound the data to 255 bytes.
	 */
	if (len + offset + total_length > 255) {
		len = 255 - offset - total_length;
	}	

	/*
	 *	Encrypt the various password styles
	 *
	 *	Attributes with encrypted values MUST be less than
	 *	128 bytes long.
	 */
	switch (vp->flags.encrypt) {
	case FLAG_ENCRYPT_USER_PASSWORD:
		make_passwd(ptr + offset, &len,
			    data, len,
			    secret, packet->vector);
		break;
		
	case FLAG_ENCRYPT_TUNNEL_PASSWORD:
		if (!original) {
			librad_log("ERROR: No request packet, cannot encrypt %s attribute in the vp.", vp->name);
			return -1;
		}

		/*
		 *	Check if 255 - offset - total_length is less
		 *	than 18.  If so, we can't fit the data into
		 *	the available space, and we discard the
		 *	attribute.
		 *
		 *	This is ONLY a problem if we have multiple VSA's
		 *	in one Vendor-Specific, though.
		 */
		if ((255 - offset - total_length) < 18) return 0;

		/*
		 *	Can't make the password, suppress it.
		 */
		make_tunnel_passwd(ptr + offset, &len,
				   data, len, 255 - offset - total_length,
				   secret, original->vector);
		break;

		/*
		 *	The code above ensures that this attribute
		 *	always fits.
		 */
	case FLAG_ENCRYPT_ASCEND_SECRET:
		make_secret(ptr + offset, packet->vector,
			    secret, data);
		len = AUTH_VECTOR_LEN;
		break;

		
	default:
		/*
		 *	Just copy the data over
		 */
		memcpy(ptr + offset, data, len);
		break;
	} /* switch over encryption flags */

	/*
	 *	Account for the tag (if any).
	 */
	len += offset;

	/*
	 *	RFC 2865 section 5 says that zero-length attributes
	 *	MUST NOT be sent.
	 */
	if (len == 0) return 0;

	/*
	 *	Update the various lengths.
	 */
	*length_ptr += len;
	if (vsa_length_ptr) *vsa_length_ptr += len;
	ptr += len;
	total_length += len;

	return total_length;	/* of attribute */
}