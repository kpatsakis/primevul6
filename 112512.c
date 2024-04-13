status WAVEFile::writeFormat()
{
	uint16_t	formatTag, channelCount;
	uint32_t	sampleRate, averageBytesPerSecond;
	uint16_t	blockAlign;
	uint32_t	chunkSize;
	uint16_t	bitsPerSample;

	Track *track = getTrack();

	m_fh->write("fmt ", 4);

	switch (track->f.compressionType)
	{
		case AF_COMPRESSION_NONE:
			chunkSize = 16;
			if (track->f.sampleFormat == AF_SAMPFMT_FLOAT ||
				track->f.sampleFormat == AF_SAMPFMT_DOUBLE)
			{
				formatTag = WAVE_FORMAT_IEEE_FLOAT;
			}
			else if (track->f.sampleFormat == AF_SAMPFMT_TWOSCOMP ||
				track->f.sampleFormat == AF_SAMPFMT_UNSIGNED)
			{
				formatTag = WAVE_FORMAT_PCM;
			}
			else
			{
				_af_error(AF_BAD_COMPTYPE, "bad sample format");
				return AF_FAIL;
			}

			blockAlign = _af_format_frame_size(&track->f, false);
			bitsPerSample = 8 * _af_format_sample_size(&track->f, false);
			break;

		/*
			G.711 compression uses eight bits per sample.
		*/
		case AF_COMPRESSION_G711_ULAW:
			chunkSize = 18;
			formatTag = IBM_FORMAT_MULAW;
			blockAlign = track->f.channelCount;
			bitsPerSample = 8;
			break;

		case AF_COMPRESSION_G711_ALAW:
			chunkSize = 18;
			formatTag = IBM_FORMAT_ALAW;
			blockAlign = track->f.channelCount;
			bitsPerSample = 8;
			break;

		case AF_COMPRESSION_IMA:
			chunkSize = 20;
			formatTag = WAVE_FORMAT_DVI_ADPCM;
			blockAlign = track->f.bytesPerPacket;
			bitsPerSample = 4;
			break;

		case AF_COMPRESSION_MS_ADPCM:
			chunkSize = 50;
			formatTag = WAVE_FORMAT_ADPCM;
			blockAlign = track->f.bytesPerPacket;
			bitsPerSample = 4;
			break;

		default:
			_af_error(AF_BAD_COMPTYPE, "bad compression type");
			return AF_FAIL;
	}

	writeU32(&chunkSize);
	writeU16(&formatTag);

	channelCount = track->f.channelCount;
	writeU16(&channelCount);

	sampleRate = track->f.sampleRate;
	writeU32(&sampleRate);

	averageBytesPerSecond =
		track->f.sampleRate * _af_format_frame_size(&track->f, false);
	if (track->f.compressionType == AF_COMPRESSION_IMA ||
		track->f.compressionType == AF_COMPRESSION_MS_ADPCM)
		averageBytesPerSecond = track->f.sampleRate * track->f.bytesPerPacket /
			track->f.framesPerPacket;
	writeU32(&averageBytesPerSecond);

	writeU16(&blockAlign);

	writeU16(&bitsPerSample);

	if (track->f.compressionType == AF_COMPRESSION_G711_ULAW ||
		track->f.compressionType == AF_COMPRESSION_G711_ALAW)
	{
		uint16_t zero = 0;
		writeU16(&zero);
	}
	else if (track->f.compressionType == AF_COMPRESSION_IMA)
	{
		uint16_t extraByteCount = 2;
		writeU16(&extraByteCount);
		uint16_t samplesPerBlock = track->f.framesPerPacket;
		writeU16(&samplesPerBlock);
	}
	else if (track->f.compressionType == AF_COMPRESSION_MS_ADPCM)
	{
		uint16_t extraByteCount = 2 + 2 + m_msadpcmNumCoefficients * 4;
		writeU16(&extraByteCount);
		uint16_t samplesPerBlock = track->f.framesPerPacket;
		writeU16(&samplesPerBlock);

		uint16_t numCoefficients = m_msadpcmNumCoefficients;
		writeU16(&numCoefficients);

		for (int i=0; i<m_msadpcmNumCoefficients; i++)
		{
			writeS16(&m_msadpcmCoefficients[i][0]);
			writeS16(&m_msadpcmCoefficients[i][1]);
		}
	}

	return AF_SUCCEED;
}