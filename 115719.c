AP4_AvccAtom::InspectFields(AP4_AtomInspector& inspector)
{
    inspector.AddField("Configuration Version", m_ConfigurationVersion);
    const char* profile_name = GetProfileName(m_Profile);
    if (profile_name) {
        inspector.AddField("Profile", profile_name);
    } else {
        inspector.AddField("Profile", m_Profile);
    }
    inspector.AddField("Profile Compatibility", m_ProfileCompatibility, AP4_AtomInspector::HINT_HEX);
    inspector.AddField("Level", m_Level);
    inspector.AddField("NALU Length Size", m_NaluLengthSize);
    for (unsigned int i=0; i<m_SequenceParameters.ItemCount(); i++) {
        inspector.AddField("Sequence Parameter", m_SequenceParameters[i].GetData(), m_SequenceParameters[i].GetDataSize());
    }
    for (unsigned int i=0; i<m_PictureParameters.ItemCount(); i++) {
        inspector.AddField("Picture Parameter", m_PictureParameters[i].GetData(), m_PictureParameters[i].GetDataSize());
    }
    return AP4_SUCCESS;
}