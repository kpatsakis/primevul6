AP4_RtpHintSampleEntry::AP4_RtpHintSampleEntry(AP4_UI16 hint_track_version,
                                               AP4_UI16 highest_compatible_version,
                                               AP4_UI32 max_packet_size,
                                               AP4_UI32 timescale):
    AP4_SampleEntry(AP4_ATOM_TYPE_RTP_),
    m_HintTrackVersion(hint_track_version),
    m_HighestCompatibleVersion(highest_compatible_version),
    m_MaxPacketSize(max_packet_size)
{
    // build an atom for timescale
    AddChild(new AP4_TimsAtom(timescale));
}