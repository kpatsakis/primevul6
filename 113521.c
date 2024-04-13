AP4_AudioSampleEntry::InspectFields(AP4_AtomInspector& inspector)
{
    // dump the fields from the base class
    AP4_SampleEntry::InspectFields(inspector);

    // fields
    inspector.AddField("channel_count", GetChannelCount());
    inspector.AddField("sample_size", GetSampleSize());
    inspector.AddField("sample_rate", GetSampleRate());
    if (m_QtVersion) {
        inspector.AddField("qt_version", m_QtVersion);
    }
    
    return AP4_SUCCESS;
}