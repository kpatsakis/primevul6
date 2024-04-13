AP4_RtpHintSampleEntry::InspectFields(AP4_AtomInspector& inspector)
{
    // sample entry
    AP4_SampleEntry::InspectFields(inspector);
    
    // fields
    inspector.AddField("hint_track_version", m_HintTrackVersion);
    inspector.AddField("highest_compatible_version", m_HighestCompatibleVersion);
    inspector.AddField("max_packet_size", m_MaxPacketSize);
    
    return AP4_SUCCESS;
}