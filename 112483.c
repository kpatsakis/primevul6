int main (int argc, char **argv)
{
	if (argc == 2)
	{
		if (!strcmp(argv[1], "--version") || !strcmp(argv[1], "-v"))
		{
			printversion();
			exit(EXIT_SUCCESS);
		}

		if (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h"))
		{
			printusage();
			exit(EXIT_SUCCESS);
		}
	}

	if (argc < 3)
		usageerror();

	const char *inFileName = argv[1];
	const char *outFileName = argv[2];

	int outFileFormat = AF_FILE_UNKNOWN;
	int outSampleFormat = -1, outSampleWidth = -1, outChannelCount = -1;
	int outCompression = AF_COMPRESSION_NONE;
	double outMaxAmp = 1.0;

	int i = 3;

	while (i < argc)
	{
		if (!strcmp(argv[i], "format"))
		{
			if (i + 1 >= argc)
				usageerror();

			if (!strcmp(argv[i+1], "aiff"))
				outFileFormat = AF_FILE_AIFF;
			else if (!strcmp(argv[i+1], "aifc"))
				outFileFormat = AF_FILE_AIFFC;
			else if (!strcmp(argv[i+1], "wave"))
				outFileFormat = AF_FILE_WAVE;
			else if (!strcmp(argv[i+1], "next"))
				outFileFormat = AF_FILE_NEXTSND;
			else if (!strcmp(argv[i+1], "bics"))
				outFileFormat = AF_FILE_BICSF;
			else if (!strcmp(argv[i+1], "smp"))
				outFileFormat = AF_FILE_SAMPLEVISION;
			else if (!strcmp(argv[i+1], "voc"))
				outFileFormat = AF_FILE_VOC;
			else if (!strcmp(argv[i+1], "nist"))
				outFileFormat = AF_FILE_NIST_SPHERE;
			else if (!strcmp(argv[i+1], "caf"))
				outFileFormat = AF_FILE_CAF;
			else if (!strcmp(argv[i+1], "flac"))
				outFileFormat = AF_FILE_FLAC;
			else
			{
				fprintf(stderr, "sfconvert: Unknown format %s.\n", argv[i+1]);
				exit(EXIT_FAILURE);
			}

			// Increment for argument.
			i++;
		}
		else if (!strcmp(argv[i], "channels"))
		{
			if (i + 1 >= argc)
				usageerror();

			outChannelCount = atoi(argv[i+1]);
			if (outChannelCount < 1)
				usageerror();

			// Increment for argument.
			i++;
		}
		else if (!strcmp(argv[i], "float"))
		{
			if (i + 1 >= argc)
				usageerror();

			outSampleFormat = AF_SAMPFMT_FLOAT;
			outSampleWidth = 32;
			outMaxAmp = atof(argv[i+1]);

			// outMaxAmp is currently unused.
			(void) outMaxAmp;

			// Increment for argument.
			i++;
		}
		else if (!strcmp(argv[i], "integer"))
		{
			if (i + 2 >= argc)
				usageerror();

			outSampleWidth = atoi(argv[i+1]);
			if (outSampleWidth < 1 || outSampleWidth > 32)
				usageerror();

			if (!strcmp(argv[i+2], "2scomp"))
				outSampleFormat = AF_SAMPFMT_TWOSCOMP;
			else if (!strcmp(argv[i+2], "unsigned"))
				outSampleFormat = AF_SAMPFMT_UNSIGNED;
			else
				usageerror();

			// Increment for arguments.
			i += 2;
		}
		else if (!strcmp(argv[i], "compression"))
		{
			if (i + 1 >= argc)
				usageerror();

			if (!strcmp(argv[i+1], "none"))
				outCompression = AF_COMPRESSION_NONE;
			else if (!strcmp(argv[i+1], "ulaw"))
				outCompression = AF_COMPRESSION_G711_ULAW;
			else if (!strcmp(argv[i+1], "alaw"))
				outCompression = AF_COMPRESSION_G711_ALAW;
			else if (!strcmp(argv[i+1], "ima"))
				outCompression = AF_COMPRESSION_IMA;
			else if (!strcmp(argv[i+1], "msadpcm"))
				outCompression = AF_COMPRESSION_MS_ADPCM;
			else if (!strcmp(argv[i+1], "flac"))
				outCompression = AF_COMPRESSION_FLAC;
			else if (!strcmp(argv[i+1], "alac"))
				outCompression = AF_COMPRESSION_ALAC;
			else
			{
				fprintf(stderr, "sfconvert: Unknown compression format %s.\n", argv[i+1]);
				exit(EXIT_FAILURE);
			}

			i++;
		}
		else
		{
			printf("Unrecognized command %s\n", argv[i]);
		}

		i++;
	}

	AFfilehandle inFile = afOpenFile(inFileName, "r", AF_NULL_FILESETUP);
	if (!inFile)
	{
		printf("Could not open file '%s' for reading.\n", inFileName);
		return EXIT_FAILURE;
	}

	// Get audio format parameters from input file.
	int fileFormat = afGetFileFormat(inFile, NULL);
	int channelCount = afGetChannels(inFile, AF_DEFAULT_TRACK);
	double sampleRate = afGetRate(inFile, AF_DEFAULT_TRACK);
	int sampleFormat, sampleWidth;
	afGetSampleFormat(inFile, AF_DEFAULT_TRACK, &sampleFormat, &sampleWidth);

	// Initialize output audio format parameters.
	AFfilesetup outFileSetup = afNewFileSetup();

	if (outFileFormat == -1)
		outFileFormat = fileFormat;

	if (outSampleFormat == -1 || outSampleWidth == -1)
	{
		outSampleFormat = sampleFormat;
		outSampleWidth = sampleWidth;
	}

	if (outChannelCount == -1)
		outChannelCount = channelCount;

	afInitFileFormat(outFileSetup, outFileFormat);
	afInitCompression(outFileSetup, AF_DEFAULT_TRACK, outCompression);
	afInitSampleFormat(outFileSetup, AF_DEFAULT_TRACK, outSampleFormat,
		outSampleWidth);
	afInitChannels(outFileSetup, AF_DEFAULT_TRACK, outChannelCount);
	afInitRate(outFileSetup, AF_DEFAULT_TRACK, sampleRate);

	AFfilehandle outFile = afOpenFile(outFileName, "w", outFileSetup);
	if (!outFile)
	{
		printf("Could not open file '%s' for writing.\n", outFileName);
		return EXIT_FAILURE;
	}

	afFreeFileSetup(outFileSetup);

	/*
		Set the output file's virtual audio format parameters
		to match the audio format parameters of the input file.
	*/
	afSetVirtualChannels(outFile, AF_DEFAULT_TRACK, channelCount);
	afSetVirtualSampleFormat(outFile, AF_DEFAULT_TRACK, sampleFormat,
		sampleWidth);

	bool success = copyaudiodata(inFile, outFile, AF_DEFAULT_TRACK);

	afCloseFile(inFile);
	afCloseFile(outFile);

	if (!success)
	{
		unlink(outFileName);
		return EXIT_FAILURE;
	}

	printfileinfo(inFileName);
	putchar('\n');
	printfileinfo(outFileName);

	return EXIT_SUCCESS;
}