static struct MYSOFA_HRTF *getHrtf(struct READER *reader, int *err) {
	int dimensionflags = 0;
	struct DIR *dir = reader->superblock.dataobject.directory;

	struct MYSOFA_HRTF *hrtf = malloc(sizeof(struct MYSOFA_HRTF));
	if (!hrtf) {
		*err = errno;
		return NULL;
	}
	memset(hrtf, 0, sizeof(struct MYSOFA_HRTF));

	/* copy SOFA file attributes */
	hrtf->attributes = reader->superblock.dataobject.attributes;
	reader->superblock.dataobject.attributes = NULL;

	/* check SOFA file attributes */
	if (!!(*err = checkAttribute(hrtf->attributes, "Conventions", "SOFA")))
		goto error;

	/* read dimensions */
	while (dir) {
		if (dir->dataobject.name && dir->dataobject.name[0]
				&& dir->dataobject.name[1] == 0) {
			switch (dir->dataobject.name[0]) {
			case 'I':
				*err = getDimension(&hrtf->I, &dir->dataobject);
				dimensionflags |= 1;
				break;
			case 'C':
				*err = getDimension(&hrtf->C, &dir->dataobject);
				dimensionflags |= 2;
				break;
			case 'R':
				*err = getDimension(&hrtf->R, &dir->dataobject);
				dimensionflags |= 4;
				break;
			case 'E':
				*err = getDimension(&hrtf->E, &dir->dataobject);
				dimensionflags |= 8;
				break;
			case 'N':
				*err = getDimension(&hrtf->N, &dir->dataobject);
				dimensionflags |= 0x10;
				break;
			case 'M':
				*err = getDimension(&hrtf->M, &dir->dataobject);
				dimensionflags |= 0x20;
				break;
			case 'S':
				break; /* be graceful, some issues with API version 0.4.4 */
			default:
				log("UNKNOWN SOFA VARIABLE %s", dir->dataobject.name);
				goto error;
			}
			if (*err)
				goto error;
		}
		dir = dir->next;
	}

	if (dimensionflags != 0x3f || hrtf->I != 1 || hrtf->C != 3) {
		log("dimensions are missing or wrong\n");
		goto error;
	}

	dir = reader->superblock.dataobject.directory;
	while (dir) {

		if(!dir->dataobject.name) {
			log("SOFA VARIABLE IS NULL.\n");
		}
		else if (!strcmp(dir->dataobject.name, "ListenerPosition")) {
			*err = getArray(&hrtf->ListenerPosition, &dir->dataobject);
		} else if (!strcmp(dir->dataobject.name, "ReceiverPosition")) {
			*err = getArray(&hrtf->ReceiverPosition, &dir->dataobject);
		} else if (!strcmp(dir->dataobject.name, "SourcePosition")) {
			*err = getArray(&hrtf->SourcePosition, &dir->dataobject);
		} else if (!strcmp(dir->dataobject.name, "EmitterPosition")) {
			*err = getArray(&hrtf->EmitterPosition, &dir->dataobject);
		} else if (!strcmp(dir->dataobject.name, "ListenerUp")) {
			*err = getArray(&hrtf->ListenerUp, &dir->dataobject);
		} else if (!strcmp(dir->dataobject.name, "ListenerView")) {
			*err = getArray(&hrtf->ListenerView, &dir->dataobject);
		} else if (!strcmp(dir->dataobject.name, "Data.IR")) {
			*err = getArray(&hrtf->DataIR, &dir->dataobject);
		} else if (!strcmp(dir->dataobject.name, "Data.SamplingRate")) {
			*err = getArray(&hrtf->DataSamplingRate, &dir->dataobject);
		} else if (!strcmp(dir->dataobject.name, "Data.Delay")) {
			*err = getArray(&hrtf->DataDelay, &dir->dataobject);
		} else {
			if (!(dir->dataobject.name[0] && !dir->dataobject.name[1]))
				log("UNKNOWN SOFA VARIABLE %s.\n", dir->dataobject.name);
		}
		dir = dir->next;
	}

	return hrtf;

	error: free(hrtf);
	if (!*err)
		*err = MYSOFA_INVALID_FORMAT;
	return NULL;
}