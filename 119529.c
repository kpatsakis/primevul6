MYSOFA_EXPORT struct MYSOFA_HRTF* mysofa_load(const char *filename, int *err) {
	struct READER reader;
	struct MYSOFA_HRTF *hrtf = NULL;

	if (filename == NULL)
		filename = CMAKE_INSTALL_PREFIX "/share/libmysofa/default.sofa";

	if (strcmp(filename, "-"))
		reader.fhd = fopen(filename, "rb");
	else
		reader.fhd = stdin;

	if (!reader.fhd) {
		log("cannot open file %s\n", filename);
		*err = errno;
		return NULL;
	}
	reader.gcol = NULL;
	reader.all = NULL;
	reader.recursive_counter = 0;

	*err = superblockRead(&reader, &reader.superblock);

	if (!*err) {
		hrtf = getHrtf(&reader, err);
	}

	superblockFree(&reader, &reader.superblock);
	gcolFree(reader.gcol);
	if (strcmp(filename, "-"))
		fclose(reader.fhd);

	return hrtf;
}