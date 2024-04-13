GF_Err sdp_Size(GF_Box *s)
{
	GF_SDPBox *ptr = (GF_SDPBox *)s;
	//don't count the NULL char!!!
	ptr->size += strlen(ptr->sdpText);
	return GF_OK;
}