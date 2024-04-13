int rad_encode(RADIUS_PACKET *packet, const RADIUS_PACKET *original,
	       const char *secret)
{
	radius_packet_t	*hdr;
	uint8_t	        *ptr;
	uint16_t	total_length;
	int		len;
	VALUE_PAIR	*reply;
	
	/*
	 *	For simplicity in the following logic, we allow
	 *	the attributes to "overflow" the 4k maximum
	 *	RADIUS packet size, by one attribute.
	 *
	 *	It's uint32_t, for alignment purposes.
	 */
	uint32_t	data[(MAX_PACKET_LEN + 256) / 4];

	/*
	 *	Double-check some things based on packet code.
	 */
	switch (packet->code) {
	case PW_AUTHENTICATION_ACK:
	case PW_AUTHENTICATION_REJECT:
	case PW_ACCESS_CHALLENGE:
		if (!original) {
			librad_log("ERROR: Cannot sign response packet without a request packet.");
			return -1;
		}
		break;
		
		/*
		 *	These packet vectors start off as all zero.
		 */
	case PW_ACCOUNTING_REQUEST:
	case PW_DISCONNECT_REQUEST:
	case PW_COA_REQUEST:
		memset(packet->vector, 0, sizeof(packet->vector));
		break;
		
	default:
		break;
	}
		
	/*
	 *	Use memory on the stack, until we know how
	 *	large the packet will be.
	 */
	hdr = (radius_packet_t *) data;
	
	/*
	 *	Build standard header
	 */
	hdr->code = packet->code;
	hdr->id = packet->id;
	
	memcpy(hdr->vector, packet->vector, sizeof(hdr->vector));

	total_length = AUTH_HDR_LEN;
	packet->verified = 0;
	
	/*
	 *	Load up the configuration values for the user
	 */
	ptr = hdr->data;

	/*
	 *	FIXME: Loop twice over the reply list.  The first time,
	 *	calculate the total length of data.  The second time,
	 *	allocate the memory, and fill in the VP's.
	 *
	 *	Hmm... this may be slower than just doing a small
	 *	memcpy.
	 */
	
	/*
	 *	Loop over the reply attributes for the packet.
	 */
	for (reply = packet->vps; reply; reply = reply->next) {
		/*
		 *	Ignore non-wire attributes
		 */
		if ((VENDOR(reply->attribute) == 0) &&
		    ((reply->attribute & 0xFFFF) > 0xff)) {
			continue;
		}
		
		/*
		 *	Set the Message-Authenticator to the correct
		 *	length and initial value.
		 */
		if (reply->attribute == PW_MESSAGE_AUTHENTICATOR) {
			reply->length = AUTH_VECTOR_LEN;
			memset(reply->strvalue, 0, AUTH_VECTOR_LEN);
			packet->verified = total_length; /* HACK! */
		}
		
		/*
		 *	Print out ONLY the attributes which
		 *	we're sending over the wire, and print
		 *	them out BEFORE they're encrypted.
		 */
		debug_pair(reply);

		len = rad_vp2attr(packet, original, secret, reply, ptr);
		if (len < 0) return -1;

		/*
		 *	Check that the packet is no more than 4k in
		 *	size, AFTER writing the attribute past the 4k
		 *	boundary, but BEFORE deciding to increase the
		 *	size of the packet. Note that the 'data'
		 *	buffer, above, is one attribute longer than
		 *	necessary, in order to permit this overflow.
		 */
		if ((total_length + len) > MAX_PACKET_LEN) {
			break;
		}

		ptr += len;
		total_length += len;
	} /* done looping over all attributes */
	
	/*
	 *	Fill in the rest of the fields, and copy the data over
	 *	from the local stack to the newly allocated memory.
	 *
	 *	Yes, all this 'memcpy' is slow, but it means
	 *	that we only allocate the minimum amount of
	 *	memory for a request.
	 */
	packet->data_len = total_length;
	packet->data = (uint8_t *) malloc(packet->data_len);
	if (!packet->data) {
		librad_log("Out of memory");
		return -1;
	}

	memcpy(packet->data, data, packet->data_len);
	hdr = (radius_packet_t *) packet->data;
	
	total_length = htons(total_length);
	memcpy(hdr->length, &total_length, sizeof(total_length));

	return 0;
}