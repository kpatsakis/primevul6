AP4_AudioSampleEntry::GetSampleRate()
{
    if (m_QtVersion == 2) {
        return (AP4_UI32)(m_QtV2SampleRate64);
    } else {
        return m_SampleRate>>16;
    }
}