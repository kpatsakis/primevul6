AP4_SampleEntry::ToSampleDescription()
{
    return new AP4_SampleDescription(AP4_SampleDescription::TYPE_UNKNOWN, m_Type, this);
}