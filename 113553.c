AP4_AudioSampleEntry::GetChannelCount()
{
    if (m_QtVersion == 2) {
        return (AP4_UI16)m_QtV2ChannelCount;
    } else {
        return m_ChannelCount;
    }
}