static int decodeMP4file(char *mp4file, char *sndfile, char *adts_fn, int to_stdout,
                  int outputFormat, int fileType, int downMatrix, int noGapless,
                  int infoOnly, int adts_out, float *song_length)
{
    int track;
    unsigned long samplerate;
    unsigned char channels;
    void *sample_buffer;

    mp4ff_t *infile;
    long sampleId, numSamples;

    audio_file *aufile;

    FILE *mp4File;
    FILE *adtsFile;
    unsigned char *adtsData;
    int adtsDataSize;

    NeAACDecHandle hDecoder;
    NeAACDecConfigurationPtr config;
    NeAACDecFrameInfo frameInfo;
    mp4AudioSpecificConfig mp4ASC;

    unsigned char *buffer;
    int buffer_size;

    char percents[200];
    int percent, old_percent = -1;

    int first_time = 1;

    /* for gapless decoding */
    unsigned int useAacLength = 1;
    unsigned int initial = 1;
    unsigned int framesize;
    unsigned long timescale;


    /* initialise the callback structure */
    mp4ff_callback_t *mp4cb = malloc(sizeof(mp4ff_callback_t));

    if (strcmp(mp4file, "-") == 0 ) {
        faad_fprintf(stderr, "Cannot open stdin for MP4 input \n");
        return 1;
    }

    mp4File = fopen(mp4file, "rb");
    mp4cb->read = read_callback;
    mp4cb->seek = seek_callback;
    mp4cb->user_data = mp4File;


    hDecoder = NeAACDecOpen();

    /* Set configuration */
    config = NeAACDecGetCurrentConfiguration(hDecoder);
    config->outputFormat = outputFormat;
    config->downMatrix = downMatrix;
    //config->dontUpSampleImplicitSBR = 1;
    NeAACDecSetConfiguration(hDecoder, config);

    if (adts_out)
    {
        adtsFile = fopen(adts_fn, "wb");
        if (adtsFile == NULL)
        {
            faad_fprintf(stderr, "Error opening file: %s\n", adts_fn);
            return 1;
        }
    }

    infile = mp4ff_open_read(mp4cb);
    if (!infile)
    {
        /* unable to open file */
        faad_fprintf(stderr, "Error opening file: %s\n", mp4file);
        return 1;
    }

    if ((track = GetAACTrack(infile)) < 0)
    {
        faad_fprintf(stderr, "Unable to find correct AAC sound track in the MP4 file.\n");
        NeAACDecClose(hDecoder);
        mp4ff_close(infile);
        free(mp4cb);
        fclose(mp4File);
        return 1;
    }

    buffer = NULL;
    buffer_size = 0;
    mp4ff_get_decoder_config(infile, track, &buffer, &buffer_size);

    if(NeAACDecInit2(hDecoder, buffer, buffer_size,
                    &samplerate, &channels) < 0)
    {
        /* If some error initializing occured, skip the file */
        faad_fprintf(stderr, "Error initializing decoder library.\n");
        NeAACDecClose(hDecoder);
        mp4ff_close(infile);
        free(mp4cb);
        fclose(mp4File);
        return 1;
    }

    timescale = mp4ff_time_scale(infile, track);
    framesize = 1024;
    useAacLength = 0;

    if (buffer)
    {
        if (NeAACDecAudioSpecificConfig(buffer, buffer_size, &mp4ASC) >= 0)
        {
            if (mp4ASC.frameLengthFlag == 1) framesize = 960;
            if (mp4ASC.sbr_present_flag == 1) framesize *= 2;
        }
        free(buffer);
    }

    /* print some mp4 file info */
    faad_fprintf(stderr, "%s file info:\n\n", mp4file);
    {
        char *tag = NULL, *item = NULL;
        int k, j;
        char *ot[6] = { "NULL", "MAIN AAC", "LC AAC", "SSR AAC", "LTP AAC", "HE AAC" };
        long samples = mp4ff_num_samples(infile, track);
        float f = 1024.0;
        float seconds;
        if (mp4ASC.sbr_present_flag == 1)
        {
            f = f * 2.0;
        }
        seconds = (float)samples*(float)(f-1.0)/(float)mp4ASC.samplingFrequency;

        *song_length = seconds;

        faad_fprintf(stderr, "%s\t%.3f secs, %d ch, %d Hz\n\n", ot[(mp4ASC.objectTypeIndex > 5)?0:mp4ASC.objectTypeIndex],
            seconds, mp4ASC.channelsConfiguration, mp4ASC.samplingFrequency);

#define PRINT_MP4_METADATA
#ifdef PRINT_MP4_METADATA
        j = mp4ff_meta_get_num_items(infile);
        for (k = 0; k < j; k++)
        {
            if (mp4ff_meta_get_by_index(infile, k, &item, &tag))
            {
                if (item != NULL && tag != NULL)
                {
                    faad_fprintf(stderr, "%s: %s\n", item, tag);
                    free(item); item = NULL;
                    free(tag); tag = NULL;
                }
            }
        }
        if (j > 0) faad_fprintf(stderr, "\n");
#endif
    }

    if (infoOnly)
    {
        NeAACDecClose(hDecoder);
        mp4ff_close(infile);
        free(mp4cb);
        fclose(mp4File);
        return 0;
    }

    numSamples = mp4ff_num_samples(infile, track);

    for (sampleId = 0; sampleId < numSamples; sampleId++)
    {
        int rc;
        long dur;
        unsigned int sample_count;
        unsigned int delay = 0;

        /* get acces unit from MP4 file */
        buffer = NULL;
        buffer_size = 0;

        dur = mp4ff_get_sample_duration(infile, track, sampleId);
        rc = mp4ff_read_sample(infile, track, sampleId, &buffer,  &buffer_size);
        if (rc == 0)
        {
            faad_fprintf(stderr, "Reading from MP4 file failed.\n");
            NeAACDecClose(hDecoder);
            mp4ff_close(infile);
            free(mp4cb);
            fclose(mp4File);
            return 1;
        }

        sample_buffer = NeAACDecDecode(hDecoder, &frameInfo, buffer, buffer_size);

        if (adts_out == 1)
        {
            adtsData = MakeAdtsHeader(&adtsDataSize, &frameInfo, 0);

            /* write the adts header */
            fwrite(adtsData, 1, adtsDataSize, adtsFile);

            fwrite(buffer, 1, frameInfo.bytesconsumed, adtsFile);
        }

        if (buffer) free(buffer);

        if (!noGapless)
        {
            if (sampleId == 0) dur = 0;

            if (useAacLength || (timescale != samplerate)) {
                sample_count = frameInfo.samples;
            } else {
                sample_count = (unsigned int)(dur * frameInfo.channels);
                if (sample_count > frameInfo.samples)
                    sample_count = frameInfo.samples;

                if (!useAacLength && !initial && (sampleId < numSamples/2) && (sample_count != frameInfo.samples))
                {
                    faad_fprintf(stderr, "MP4 seems to have incorrect frame duration, using values from AAC data.\n");
                    useAacLength = 1;
                    sample_count = frameInfo.samples;
                }
            }

            if (initial && (sample_count < framesize*frameInfo.channels) && (frameInfo.samples > sample_count))
                delay = frameInfo.samples - sample_count;
        } else {
            sample_count = frameInfo.samples;
        }

        /* open the sound file now that the number of channels are known */
        if (first_time && !frameInfo.error)
        {
            /* print some channel info */
            print_channel_info(&frameInfo);

            if (!adts_out)
            {
                /* open output file */
                if(!to_stdout)
                {
                    aufile = open_audio_file(sndfile, frameInfo.samplerate, frameInfo.channels,
                        outputFormat, fileType, aacChannelConfig2wavexChannelMask(&frameInfo));
                } else {
#ifdef _WIN32
                    setmode(fileno(stdout), O_BINARY);
#endif
                    aufile = open_audio_file("-", frameInfo.samplerate, frameInfo.channels,
                        outputFormat, fileType, aacChannelConfig2wavexChannelMask(&frameInfo));
                }
                if (aufile == NULL)
                {
                    NeAACDecClose(hDecoder);
                    mp4ff_close(infile);
                    free(mp4cb);
                    fclose(mp4File);
                    return 0;
                }
            }
            first_time = 0;
        }

        if (sample_count > 0) initial = 0;

        percent = min((int)(sampleId*100)/numSamples, 100);
        if (percent > old_percent)
        {
            old_percent = percent;
            sprintf(percents, "%d%% decoding %s.", percent, mp4file);
            faad_fprintf(stderr, "%s\r", percents);
#ifdef _WIN32
            SetConsoleTitle(percents);
#endif
        }

        if ((frameInfo.error == 0) && (sample_count > 0) && (!adts_out))
        {
            if (write_audio_file(aufile, sample_buffer, sample_count, delay) == 0)
                break;
        }

        if (frameInfo.error > 0)
        {
            faad_fprintf(stderr, "Warning: %s\n",
                NeAACDecGetErrorMessage(frameInfo.error));
        }
    }

    NeAACDecClose(hDecoder);

    if (adts_out == 1)
    {
        fclose(adtsFile);
    }

    mp4ff_close(infile);

    if (!first_time && !adts_out)
        close_audio_file(aufile);

    free(mp4cb);
    fclose(mp4File);

    return frameInfo.error;
}