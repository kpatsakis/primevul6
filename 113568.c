AP4_StszAtom::InspectFields(AP4_AtomInspector& inspector)
{
    inspector.AddField("sample_size", m_SampleSize);
    inspector.AddField("sample_count", m_Entries.ItemCount());

    if (inspector.GetVerbosity() >= 2) {
        char header[32];
        for (AP4_Ordinal i=0; i<m_Entries.ItemCount(); i++) {
            AP4_FormatString(header, sizeof(header), "entry %8d", i);
            inspector.AddField(header, m_Entries[i]);
        }
    }

    return AP4_SUCCESS;
}