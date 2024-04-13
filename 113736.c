inline void DNS::EmptyHeader(unsigned char *output, const DNSHeader *header, const int length)
{
	output[0] = header->id[0];
	output[1] = header->id[1];
	output[2] = header->flags1;
	output[3] = header->flags2;
	output[4] = header->qdcount >> 8;
	output[5] = header->qdcount & 0xFF;
	output[6] = header->ancount >> 8;
	output[7] = header->ancount & 0xFF;
	output[8] = header->nscount >> 8;
	output[9] = header->nscount & 0xFF;
	output[10] = header->arcount >> 8;
	output[11] = header->arcount & 0xFF;
	memcpy(&output[12],header->payload,length);
}