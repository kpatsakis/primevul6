static tag_table_type exif_get_tag_table(int section)
{
	switch(section) {
		case SECTION_FILE:      return &tag_table_IFD[0];
		case SECTION_COMPUTED:  return &tag_table_IFD[0];
		case SECTION_ANY_TAG:   return &tag_table_IFD[0];
		case SECTION_IFD0:      return &tag_table_IFD[0];
		case SECTION_THUMBNAIL: return &tag_table_IFD[0];
		case SECTION_COMMENT:   return &tag_table_IFD[0];
		case SECTION_APP0:      return &tag_table_IFD[0];
		case SECTION_EXIF:      return &tag_table_IFD[0];
		case SECTION_FPIX:      return &tag_table_IFD[0];
		case SECTION_GPS:       return &tag_table_GPS[0];
		case SECTION_INTEROP:   return &tag_table_IOP[0];
		case SECTION_APP12:     return &tag_table_IFD[0];
		case SECTION_WINXP:     return &tag_table_IFD[0];
	}
	return &tag_table_IFD[0];
}