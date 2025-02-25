DNSInfo DNSRequest::ResultIsReady(DNSHeader &header, unsigned length)
{
	unsigned i = 0, o;
	int q = 0;
	int curanswer;
	ResourceRecord rr;
 	unsigned short ptr;

	/* This is just to keep _FORTIFY_SOURCE happy */
	rr.type = DNS_QUERY_NONE;
	rr.rdlength = 0;
	rr.ttl = 1;	/* GCC is a whiney bastard -- see the XXX below. */
	rr.rr_class = 0; /* Same for VC++ */

	if (!(header.flags1 & FLAGS_MASK_QR))
		return std::make_pair((unsigned char*)NULL,"Not a query result");

	if (header.flags1 & FLAGS_MASK_OPCODE)
		return std::make_pair((unsigned char*)NULL,"Unexpected value in DNS reply packet");

	if (header.flags2 & FLAGS_MASK_RCODE)
		return std::make_pair((unsigned char*)NULL,"Domain name not found");

	if (header.ancount < 1)
		return std::make_pair((unsigned char*)NULL,"No resource records returned");

	/* Subtract the length of the header from the length of the packet */
	length -= 12;

	while ((unsigned int)q < header.qdcount && i < length)
	{
		if (header.payload[i] > 63)
		{
			i += 6;
			q++;
		}
		else
		{
			if (header.payload[i] == 0)
			{
				q++;
				i += 5;
			}
			else i += header.payload[i] + 1;
		}
	}
	curanswer = 0;
	while ((unsigned)curanswer < header.ancount)
	{
		q = 0;
		while (q == 0 && i < length)
		{
			if (header.payload[i] > 63)
			{
				i += 2;
				q = 1;
			}
			else
			{
				if (header.payload[i] == 0)
				{
					i++;
					q = 1;
				}
				else i += header.payload[i] + 1; /* skip length and label */
			}
		}
		if (static_cast<int>(length - i) < 10)
			return std::make_pair((unsigned char*)NULL,"Incorrectly sized DNS reply");

		/* XXX: We actually initialise 'rr' here including its ttl field */
		DNS::FillResourceRecord(&rr,&header.payload[i]);

		i += 10;
		ServerInstance->Logs->Log("RESOLVER",DEBUG,"Resolver: rr.type is %d and this.type is %d rr.class %d this.class %d", rr.type, this->type, rr.rr_class, this->rr_class);
		if (rr.type != this->type)
		{
			curanswer++;
			i += rr.rdlength;
			continue;
		}
		if (rr.rr_class != this->rr_class)
		{
			curanswer++;
			i += rr.rdlength;
			continue;
		}
		break;
	}
	if ((unsigned int)curanswer == header.ancount)
		return std::make_pair((unsigned char*)NULL,"No A, AAAA or PTR type answers (" + ConvToStr(header.ancount) + " answers)");

	if (i + rr.rdlength > (unsigned int)length)
		return std::make_pair((unsigned char*)NULL,"Resource record larger than stated");

	if (rr.rdlength > 1023)
		return std::make_pair((unsigned char*)NULL,"Resource record too large");

	this->ttl = rr.ttl;

	switch (rr.type)
	{
		/*
		 * CNAME and PTR are compressed.  We need to decompress them.
		 */
		case DNS_QUERY_CNAME:
		case DNS_QUERY_PTR:
		{
			unsigned short lowest_pos = length;
			o = 0;
			q = 0;
			while (q == 0 && i < length && o + 256 < 1023)
			{
				/* DN label found (byte over 63) */
				if (header.payload[i] > 63)
				{
					memcpy(&ptr,&header.payload[i],2);

					i = ntohs(ptr);

					/* check that highest two bits are set. if not, we've been had */
					if ((i & DN_COMP_BITMASK) != DN_COMP_BITMASK)
						return std::make_pair((unsigned char *) NULL, "DN label decompression header is bogus");

					/* mask away the two highest bits. */
					i &= ~DN_COMP_BITMASK;

					/* and decrease length by 12 bytes. */
					i -= 12;

					if (i >= lowest_pos)
						return std::make_pair((unsigned char *) NULL, "Invalid decompression pointer");
					lowest_pos = i;
				}
				else
				{
					if (header.payload[i] == 0)
					{
						q = 1;
					}
					else
					{
						res[o] = 0;
						if (o != 0)
							res[o++] = '.';

						if (o + header.payload[i] > sizeof(DNSHeader))
							return std::make_pair((unsigned char *) NULL, "DN label decompression is impossible -- malformed/hostile packet?");

						memcpy(&res[o], &header.payload[i + 1], header.payload[i]);
						o += header.payload[i];
						i += header.payload[i] + 1;
					}
				}
			}
			res[o] = 0;
		}
		break;
		case DNS_QUERY_AAAA:
			if (rr.rdlength != sizeof(struct in6_addr))
				return std::make_pair((unsigned char *) NULL, "rr.rdlength is larger than 16 bytes for an ipv6 entry -- malformed/hostile packet?");

			memcpy(res,&header.payload[i],rr.rdlength);
			res[rr.rdlength] = 0;
		break;
		case DNS_QUERY_A:
			if (rr.rdlength != sizeof(struct in_addr))
				return std::make_pair((unsigned char *) NULL, "rr.rdlength is larger than 4 bytes for an ipv4 entry -- malformed/hostile packet?");

			memcpy(res,&header.payload[i],rr.rdlength);
			res[rr.rdlength] = 0;
		break;
		default:
			return std::make_pair((unsigned char *) NULL, "don't know how to handle undefined type (" + ConvToStr(rr.type) + ") -- rejecting");
		break;
	}
	return std::make_pair(res,"No error");
}