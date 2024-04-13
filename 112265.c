static long aacChannelConfig2wavexChannelMask(NeAACDecFrameInfo *hInfo)
{
    if (hInfo->channels == 6 && hInfo->num_lfe_channels)
    {
        return SPEAKER_FRONT_LEFT + SPEAKER_FRONT_RIGHT +
            SPEAKER_FRONT_CENTER + SPEAKER_LOW_FREQUENCY +
            SPEAKER_BACK_LEFT + SPEAKER_BACK_RIGHT;
    } else {
        return 0;
    }
}