static bool misc_type_to_wave (int misctype, Tag *miscid)
{
	if (misctype == AF_MISC_AUTH)
		*miscid = "IART";
	else if (misctype == AF_MISC_NAME)
		*miscid = "INAM";
	else if (misctype == AF_MISC_COPY)
		*miscid = "ICOP";
	else if (misctype == AF_MISC_ICMT)
		*miscid = "ICMT";
	else if (misctype == AF_MISC_ICRD)
		*miscid = "ICRD";
	else if (misctype == AF_MISC_ISFT)
		*miscid = "ISFT";
	else
		return false;

	return true;
}