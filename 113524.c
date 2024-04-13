AP4_AudioSampleEntry::ToSampleDescription()
{
    // create a sample description
    return new AP4_GenericAudioSampleDescription(
        m_Type,
        GetSampleRate(),
        GetSampleSize(),
        GetChannelCount(),
        this);
}