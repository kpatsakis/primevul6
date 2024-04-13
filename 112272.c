static void print_channel_info(NeAACDecFrameInfo *frameInfo)
{
    /* print some channel info */
    int i;
    long channelMask = aacChannelConfig2wavexChannelMask(frameInfo);

    faad_fprintf(stderr, "  ---------------------\n");
    if (frameInfo->num_lfe_channels > 0)
    {
        faad_fprintf(stderr, " | Config: %2d.%d Ch     |", frameInfo->channels-frameInfo->num_lfe_channels, frameInfo->num_lfe_channels);
    } else {
        faad_fprintf(stderr, " | Config: %2d Ch       |", frameInfo->channels);
    }
    if (channelMask)
        faad_fprintf(stderr, " WARNING: channels are reordered according to\n");
    else
        faad_fprintf(stderr, "\n");
    faad_fprintf(stderr, "  ---------------------");
    if (channelMask)
        faad_fprintf(stderr, "  MS defaults defined in WAVE_FORMAT_EXTENSIBLE\n");
    else
        faad_fprintf(stderr, "\n");
    faad_fprintf(stderr, " | Ch |    Position    |\n");
    faad_fprintf(stderr, "  ---------------------\n");
    for (i = 0; i < frameInfo->channels; i++)
    {
        faad_fprintf(stderr, " | %.2d | %-14s |\n", i, position2string((int)frameInfo->channel_position[i]));
    }
    faad_fprintf(stderr, "  ---------------------\n");
    faad_fprintf(stderr, "\n");
}