int DNS::MakePayload(const char * const name, const QueryType rr, const unsigned short rr_class, unsigned char * const payload)
{
	short payloadpos = 0;
	const char* tempchr, *tempchr2 = name;
	unsigned short length;

	/* split name up into labels, create query */
	while ((tempchr = strchr(tempchr2,'.')) != NULL)
	{
		length = tempchr - tempchr2;
		if (payloadpos + length + 1 > 507)
			return -1;
		payload[payloadpos++] = length;
		memcpy(&payload[payloadpos],tempchr2,length);
		payloadpos += length;
		tempchr2 = &tempchr[1];
	}
	length = strlen(tempchr2);
	if (length)
	{
		if (payloadpos + length + 2 > 507)
			return -1;
		payload[payloadpos++] = length;
		memcpy(&payload[payloadpos],tempchr2,length);
		payloadpos += length;
		payload[payloadpos++] = 0;
	}
	if (payloadpos > 508)
		return -1;
	length = htons(rr);
	memcpy(&payload[payloadpos],&length,2);
	length = htons(rr_class);
	memcpy(&payload[payloadpos + 2],&length,2);
	return payloadpos + 4;
}