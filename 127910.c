static char *exif_get_tagformat(int format)
{
	switch(format) {
		case TAG_FMT_BYTE:      return "BYTE";
		case TAG_FMT_STRING:    return "STRING";
		case TAG_FMT_USHORT:    return "USHORT";
		case TAG_FMT_ULONG:     return "ULONG";
		case TAG_FMT_URATIONAL: return "URATIONAL";
		case TAG_FMT_SBYTE:     return "SBYTE";
		case TAG_FMT_UNDEFINED: return "UNDEFINED";
		case TAG_FMT_SSHORT:    return "SSHORT";
		case TAG_FMT_SLONG:     return "SLONG";
		case TAG_FMT_SRATIONAL: return "SRATIONAL";
		case TAG_FMT_SINGLE:    return "SINGLE";
		case TAG_FMT_DOUBLE:    return "DOUBLE";
		case TAG_FMT_IFD:       return "IFD";
	}
	return "*Illegal";
}