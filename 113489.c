AP4_VisualSampleEntry::InspectFields(AP4_AtomInspector& inspector)
{
    // dump the fields of the base class
    AP4_SampleEntry::InspectFields(inspector);

    // fields
    inspector.AddField("width", m_Width);
    inspector.AddField("height", m_Height);
    inspector.AddField("compressor", m_CompressorName.GetChars());

    return AP4_SUCCESS;
}