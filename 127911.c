static char *exif_get_sectionname(int section)
{
	switch(section) {
		case SECTION_FILE:      return "FILE";
		case SECTION_COMPUTED:  return "COMPUTED";
		case SECTION_ANY_TAG:   return "ANY_TAG";
		case SECTION_IFD0:      return "IFD0";
		case SECTION_THUMBNAIL: return "THUMBNAIL";
		case SECTION_COMMENT:   return "COMMENT";
		case SECTION_APP0:      return "APP0";
		case SECTION_EXIF:      return "EXIF";
		case SECTION_FPIX:      return "FPIX";
		case SECTION_GPS:       return "GPS";
		case SECTION_INTEROP:   return "INTEROP";
		case SECTION_APP12:     return "APP12";
		case SECTION_WINXP:     return "WINXP";
		case SECTION_MAKERNOTE: return "MAKERNOTE";
	}
	return "";
}