inline void DNS::FillResourceRecord(ResourceRecord* rr, const unsigned char *input)
{
	rr->type = (QueryType)((input[0] << 8) + input[1]);
	rr->rr_class = (input[2] << 8) + input[3];
	rr->ttl = (input[4] << 24) + (input[5] << 16) + (input[6] << 8) + input[7];
	rr->rdlength = (input[8] << 8) + input[9];
}