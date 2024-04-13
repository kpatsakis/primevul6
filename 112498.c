AFfilesetup WAVEFile::completeSetup(AFfilesetup setup)
{
	if (setup->trackSet && setup->trackCount != 1)
	{
		_af_error(AF_BAD_NUMTRACKS, "WAVE file must have 1 track");
		return AF_NULL_FILESETUP;
	}

	TrackSetup *track = setup->getTrack();
	if (!track)
		return AF_NULL_FILESETUP;

	if (track->f.isCompressed())
	{
		if (!track->sampleFormatSet)
			_af_set_sample_format(&track->f, AF_SAMPFMT_TWOSCOMP, 16);
		else
			_af_set_sample_format(&track->f, track->f.sampleFormat, track->f.sampleWidth);
	}
	else if (track->sampleFormatSet)
	{
		switch (track->f.sampleFormat)
		{
			case AF_SAMPFMT_FLOAT:
				if (track->sampleWidthSet &&
					track->f.sampleWidth != 32)
				{
					_af_error(AF_BAD_WIDTH,
						"Warning: invalid sample width for floating-point WAVE file: %d (must be 32 bits)\n",
						track->f.sampleWidth);
					_af_set_sample_format(&track->f, AF_SAMPFMT_FLOAT, 32);
				}
				break;

			case AF_SAMPFMT_DOUBLE:
				if (track->sampleWidthSet &&
					track->f.sampleWidth != 64)
				{
					_af_error(AF_BAD_WIDTH,
						"Warning: invalid sample width for double-precision floating-point WAVE file: %d (must be 64 bits)\n",
						track->f.sampleWidth);
					_af_set_sample_format(&track->f, AF_SAMPFMT_DOUBLE, 64);
				}
				break;

			case AF_SAMPFMT_UNSIGNED:
				if (track->sampleWidthSet)
				{
					if (track->f.sampleWidth < 1 || track->f.sampleWidth > 32)
					{
						_af_error(AF_BAD_WIDTH, "invalid sample width for WAVE file: %d (must be 1-32 bits)\n", track->f.sampleWidth);
						return AF_NULL_FILESETUP;
					}
					if (track->f.sampleWidth > 8)
					{
						_af_error(AF_BAD_SAMPFMT, "WAVE integer data of more than 8 bits must be two's complement signed");
						_af_set_sample_format(&track->f, AF_SAMPFMT_TWOSCOMP, track->f.sampleWidth);
					}
				}
				else
				/*
					If the sample width is not set but the user requests
					unsigned data, set the width to 8 bits.
				*/
					_af_set_sample_format(&track->f, track->f.sampleFormat, 8);
				break;

			case AF_SAMPFMT_TWOSCOMP:
				if (track->sampleWidthSet)
				{
					if (track->f.sampleWidth < 1 || track->f.sampleWidth > 32)
					{
						_af_error(AF_BAD_WIDTH, "invalid sample width %d for WAVE file (must be 1-32)", track->f.sampleWidth);
						return AF_NULL_FILESETUP;
					}
					else if (track->f.sampleWidth <= 8)
					{
						_af_error(AF_BAD_SAMPFMT, "Warning: WAVE format integer data of 1-8 bits must be unsigned; setting sample format to unsigned");
						_af_set_sample_format(&track->f, AF_SAMPFMT_UNSIGNED, track->f.sampleWidth);
					}
				}
				else
				/*
					If no sample width was specified, we default to 16 bits
					for signed integer data.
				*/
					_af_set_sample_format(&track->f, track->f.sampleFormat, 16);
				break;
		}
	}
	/*
		Otherwise set the sample format depending on the sample
		width or set completely to default.
	*/
	else
	{
		if (!track->sampleWidthSet)
		{
			track->f.sampleWidth = 16;
			track->f.sampleFormat = AF_SAMPFMT_TWOSCOMP;
		}
		else
		{
			if (track->f.sampleWidth < 1 || track->f.sampleWidth > 32)
			{
				_af_error(AF_BAD_WIDTH, "invalid sample width %d for WAVE file (must be 1-32)", track->f.sampleWidth);
				return AF_NULL_FILESETUP;
			}
			else if (track->f.sampleWidth > 8)
				/* Here track->f.sampleWidth is in {1..32}. */
				track->f.sampleFormat = AF_SAMPFMT_TWOSCOMP;
			else
				/* Here track->f.sampleWidth is in {1..8}. */
				track->f.sampleFormat = AF_SAMPFMT_UNSIGNED;
		}
	}

	if (track->f.compressionType != AF_COMPRESSION_NONE &&
		track->f.compressionType != AF_COMPRESSION_G711_ULAW &&
		track->f.compressionType != AF_COMPRESSION_G711_ALAW &&
		track->f.compressionType != AF_COMPRESSION_IMA &&
		track->f.compressionType != AF_COMPRESSION_MS_ADPCM)
	{
		_af_error(AF_BAD_NOT_IMPLEMENTED, "compression format not supported in WAVE format");
		return AF_NULL_FILESETUP;
	}

	if (track->f.isUncompressed() &&
		track->byteOrderSet &&
		track->f.byteOrder != AF_BYTEORDER_LITTLEENDIAN &&
		track->f.isByteOrderSignificant())
	{
		_af_error(AF_BAD_BYTEORDER, "WAVE format only supports little-endian data");
		return AF_NULL_FILESETUP;
	}

	if (track->f.isUncompressed())
		track->f.byteOrder = AF_BYTEORDER_LITTLEENDIAN;

	if (track->aesDataSet)
	{
		_af_error(AF_BAD_FILESETUP, "WAVE files cannot have AES data");
		return AF_NULL_FILESETUP;
	}

	if (setup->instrumentSet)
	{
		if (setup->instrumentCount > 1)
		{
			_af_error(AF_BAD_NUMINSTS, "WAVE files can have 0 or 1 instrument");
			return AF_NULL_FILESETUP;
		}
		else if (setup->instrumentCount == 1)
		{
			if (setup->instruments[0].loopSet &&
				setup->instruments[0].loopCount > 0 &&
				(!track->markersSet || track->markerCount == 0))
			{
				_af_error(AF_BAD_NUMMARKS, "WAVE files with loops must contain at least 1 marker");
				return AF_NULL_FILESETUP;
			}
		}
	}

	/* Make sure the miscellaneous data is of an acceptable type. */
	if (setup->miscellaneousSet)
	{
		for (int i=0; i<setup->miscellaneousCount; i++)
		{
			switch (setup->miscellaneous[i].type)
			{
				case AF_MISC_COPY:
				case AF_MISC_AUTH:
				case AF_MISC_NAME:
				case AF_MISC_ICRD:
				case AF_MISC_ISFT:
				case AF_MISC_ICMT:
					break;
				default:
					_af_error(AF_BAD_MISCTYPE, "illegal miscellaneous type [%d] for WAVE file", setup->miscellaneous[i].type);
					return AF_NULL_FILESETUP;
			}
		}
	}

	/*
		Allocate an AFfilesetup and make all the unset fields correct.
	*/
	AFfilesetup	newsetup = _af_filesetup_copy(setup, &waveDefaultFileSetup, false);

	/* Make sure we do not copy loops if they are not specified in setup. */
	if (setup->instrumentSet && setup->instrumentCount > 0 &&
		setup->instruments[0].loopSet)
	{
		free(newsetup->instruments[0].loops);
		newsetup->instruments[0].loopCount = 0;
	}

	return newsetup;
}