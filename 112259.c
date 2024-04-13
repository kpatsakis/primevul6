static int decodeAACfile(char *aacfile, char *sndfile, char *adts_fn, int to_stdout,
                  int def_srate, int object_type, int outputFormat, int fileType,
                  int downMatrix, int infoOnly, int adts_out, int old_format,
                  float *song_length)
{
    int tagsize;
    unsigned long samplerate;
    unsigned char channels;
    void *sample_buffer;

    audio_file *aufile;

    FILE *adtsFile;
    unsigned char *adtsData;
    int adtsDataSize;

    NeAACDecHandle hDecoder;
    NeAACDecFrameInfo frameInfo;
    NeAACDecConfigurationPtr config;

    char percents[200];
    int percent, old_percent = -1;
    int bread, fileread;
    int header_type = 0;
    int bitrate = 0;
    float length = 0;

    int first_time = 1;
    int retval;
    int streaminput = 0;

    aac_buffer b;

    memset(&b, 0, sizeof(aac_buffer));

    if (adts_out)
    {
        adtsFile = fopen(adts_fn, "wb");
        if (adtsFile == NULL)
        {
            faad_fprintf(stderr, "Error opening file: %s\n", adts_fn);
            return 1;
        }
    }

    if (0 == strcmp(aacfile, "-"))
    {
	b.infile = stdin;
#ifdef _WIN32
        setmode(fileno(stdin), O_BINARY);
#endif

    } else
    {
    	b.infile = fopen(aacfile, "rb");
    	if (b.infile == NULL)
    	{
    	    /* unable to open file */
    	    faad_fprintf(stderr, "Error opening file: %s\n", aacfile);
    	    return 1;
    	}
    }

    retval = fseek(b.infile, 0, SEEK_END);
#ifdef _WIN32
	if (0 == strcmp(aacfile, "-")) {
	  retval = -1;
	}
#endif
    if (retval )
    {
	 faad_fprintf(stderr, "Input not seekable %s\n", aacfile);
	 fileread = -1;
         streaminput = 1;
    } else {
    	fileread = ftell(b.infile);
    	fseek(b.infile, 0, SEEK_SET);
    };

    if (!(b.buffer = (unsigned char*)malloc(FAAD_MIN_STREAMSIZE*MAX_CHANNELS)))
    {
        faad_fprintf(stderr, "Memory allocation error\n");
        return 0;
    }
    memset(b.buffer, 0, FAAD_MIN_STREAMSIZE*MAX_CHANNELS);

    bread = fread(b.buffer, 1, FAAD_MIN_STREAMSIZE*MAX_CHANNELS, b.infile);
    b.bytes_into_buffer = bread;
    b.bytes_consumed = 0;
    b.file_offset = 0;

    if (bread != FAAD_MIN_STREAMSIZE*MAX_CHANNELS)
        b.at_eof = 1;

    tagsize = 0;
    if (!memcmp(b.buffer, "ID3", 3))
    {
        /* high bit is not used */
        tagsize = (b.buffer[6] << 21) | (b.buffer[7] << 14) |
            (b.buffer[8] <<  7) | (b.buffer[9] <<  0);

        tagsize += 10;
        advance_buffer(&b, tagsize);
        fill_buffer(&b);
    }

    hDecoder = NeAACDecOpen();

    /* Set the default object type and samplerate */
    /* This is useful for RAW AAC files */
    config = NeAACDecGetCurrentConfiguration(hDecoder);
    if (def_srate)
        config->defSampleRate = def_srate;
    config->defObjectType = object_type;
    config->outputFormat = outputFormat;
    config->downMatrix = downMatrix;
    config->useOldADTSFormat = old_format;
    //config->dontUpSampleImplicitSBR = 1;
    NeAACDecSetConfiguration(hDecoder, config);

    /* get AAC infos for printing */
    header_type = 0;
    if (streaminput == 1 )
	lookforheader(&b);

    if ((b.buffer[0] == 0xFF) && ((b.buffer[1] & 0xF6) == 0xF0))
    {
        if (streaminput ==1)
	{
	    int frames, frame_length;
	    int samplerate;
	    float frames_per_sec, bytes_per_frame;
	    samplerate = adts_sample_rates[(b.buffer[2]&0x3c)>>2];
	    frame_length = ((((unsigned int)b.buffer[3] & 0x3)) << 11)
	        | (((unsigned int)b.buffer[4]) << 3) | (b.buffer[5] >> 5);

	    frames_per_sec = (float)samplerate/1024.0f;
	    bytes_per_frame = (float)frame_length/(float)(1000);
	    bitrate = (int)(8. * bytes_per_frame * frames_per_sec + 0.5);
	    length = 1;
	    faad_fprintf(stderr, "Streamed input format  samplerate %d channels %d.\n",samplerate,channels);

	} else {
		adts_parse(&b, &bitrate, &length);
        	fseek(b.infile, tagsize, SEEK_SET);

        	bread = fread(b.buffer, 1, FAAD_MIN_STREAMSIZE*MAX_CHANNELS, b.infile);
        	if (bread != FAAD_MIN_STREAMSIZE*MAX_CHANNELS)
        	    b.at_eof = 1;
        	else
        	    b.at_eof = 0;
        	b.bytes_into_buffer = bread;
        	b.bytes_consumed = 0;
        	b.file_offset = tagsize;
	}

        header_type = 1;
    } else if (memcmp(b.buffer, "ADIF", 4) == 0) {
        int skip_size = (b.buffer[4] & 0x80) ? 9 : 0;
        bitrate = ((unsigned int)(b.buffer[4 + skip_size] & 0x0F)<<19) |
            ((unsigned int)b.buffer[5 + skip_size]<<11) |
            ((unsigned int)b.buffer[6 + skip_size]<<3) |
            ((unsigned int)b.buffer[7 + skip_size] & 0xE0);

        length = (float)fileread;
        if (length != 0)
        {
            length = ((float)length*8.f)/((float)bitrate) + 0.5f;
        }

        bitrate = (int)((float)bitrate/1000.0f + 0.5f);

        header_type = 2;
    }

    *song_length = length;

    fill_buffer(&b);
    if ((bread = NeAACDecInit(hDecoder, b.buffer,
        b.bytes_into_buffer, &samplerate, &channels)) < 0)
    {
        /* If some error initializing occured, skip the file */
        faad_fprintf(stderr, "Error initializing decoder library.\n");
        if (b.buffer)
            free(b.buffer);
        NeAACDecClose(hDecoder);
        if (b.infile != stdin)
            fclose(b.infile);
        return 1;
    }
    advance_buffer(&b, bread);
    fill_buffer(&b);

    /* print AAC file info */
    faad_fprintf(stderr, "%s file info:\n", aacfile);
    switch (header_type)
    {
    case 0:
        faad_fprintf(stderr, "RAW\n\n");
        break;
    case 1:
        faad_fprintf(stderr, "ADTS, %.3f sec, %d kbps, %d Hz\n\n",
            length, bitrate, samplerate);
        break;
    case 2:
        faad_fprintf(stderr, "ADIF, %.3f sec, %d kbps, %d Hz\n\n",
            length, bitrate, samplerate);
        break;
    }

    if (infoOnly)
    {
        NeAACDecClose(hDecoder);
        if (b.infile != stdin)
            fclose(b.infile);
        if (b.buffer)
            free(b.buffer);
        return 0;
    }

    do
    {
        sample_buffer = NeAACDecDecode(hDecoder, &frameInfo,
            b.buffer, b.bytes_into_buffer);

        if (adts_out == 1)
        {
            int skip = (old_format) ? 8 : 7;
            adtsData = MakeAdtsHeader(&adtsDataSize, &frameInfo, old_format);

            /* write the adts header */
            fwrite(adtsData, 1, adtsDataSize, adtsFile);

            /* write the frame data */
            if (frameInfo.header_type == ADTS)
                fwrite(b.buffer + skip, 1, frameInfo.bytesconsumed - skip, adtsFile);
            else
                fwrite(b.buffer, 1, frameInfo.bytesconsumed, adtsFile);
        }

        /* update buffer indices */
        advance_buffer(&b, frameInfo.bytesconsumed);

        if (frameInfo.error > 0)
        {
            faad_fprintf(stderr, "Error: %s\n",
                NeAACDecGetErrorMessage(frameInfo.error));
        }

        /* open the sound file now that the number of channels are known */
        if (first_time && !frameInfo.error)
        {
            /* print some channel info */
            print_channel_info(&frameInfo);

            if (!adts_out)
            {
                /* open output file */
                if (!to_stdout)
                {
                    aufile = open_audio_file(sndfile, frameInfo.samplerate, frameInfo.channels,
                        outputFormat, fileType, aacChannelConfig2wavexChannelMask(&frameInfo));
                } else {
                    aufile = open_audio_file("-", frameInfo.samplerate, frameInfo.channels,
                        outputFormat, fileType, aacChannelConfig2wavexChannelMask(&frameInfo));
                }
                if (aufile == NULL)
                {
                    if (b.buffer)
                        free(b.buffer);
                    NeAACDecClose(hDecoder);
                    if (b.infile != stdin)
                        fclose(b.infile);
                    return 0;
                }
            } else {
                faad_fprintf(stderr, "Writing output MPEG-4 AAC ADTS file.\n\n");
            }
            first_time = 0;
        }

        percent = min((int)(b.file_offset*100)/fileread, 100);
        if (percent > old_percent)
        {
            old_percent = percent;
            sprintf(percents, "%d%% decoding %s.", percent, aacfile);
            faad_fprintf(stderr, "%s\r", percents);
#ifdef _WIN32
            SetConsoleTitle(percents);
#endif
        }

        if ((frameInfo.error == 0) && (frameInfo.samples > 0) && (!adts_out))
        {
            if (write_audio_file(aufile, sample_buffer, frameInfo.samples, 0) == 0)
                break;
		}

        /* fill buffer */
        fill_buffer(&b);

        if (b.bytes_into_buffer == 0)
            sample_buffer = NULL; /* to make sure it stops now */

    } while (sample_buffer != NULL);

    NeAACDecClose(hDecoder);

    if (adts_out == 1)
    {
        fclose(adtsFile);
    }

    if (b.infile != stdin)
        fclose(b.infile);

    if (!first_time && !adts_out)
        close_audio_file(aufile);

    if (b.buffer)
        free(b.buffer);

    return frameInfo.error;
}