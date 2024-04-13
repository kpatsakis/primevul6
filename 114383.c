GF_ISOFile *gf_isom_open(const char *fileName, GF_ISOOpenMode OpenMode, const char *tmp_dir)
{
	GF_ISOFile *movie;
	MP4_API_IO_Err = GF_OK;

	switch (OpenMode & 0xFF) {
	case GF_ISOM_OPEN_READ_DUMP:
	case GF_ISOM_OPEN_READ:
		movie = gf_isom_open_file(fileName, OpenMode, NULL);
		break;

#ifndef GPAC_DISABLE_ISOM_WRITE

	case GF_ISOM_OPEN_WRITE:
		movie = gf_isom_create_movie(fileName, OpenMode, tmp_dir);
		break;
	case GF_ISOM_OPEN_EDIT:
	case GF_ISOM_OPEN_READ_EDIT:
	case GF_ISOM_OPEN_KEEP_FRAGMENTS:
		movie = gf_isom_open_file(fileName, OpenMode, tmp_dir);
		break;
	case GF_ISOM_WRITE_EDIT:
		movie = gf_isom_create_movie(fileName, OpenMode, tmp_dir);
		break;

#endif /*GPAC_DISABLE_ISOM_WRITE*/

	default:
		return NULL;
	}
	return (GF_ISOFile *) movie;
}