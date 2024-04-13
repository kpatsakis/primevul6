AP4_HvccAtom::AP4_HvccAtom(AP4_UI08                         general_profile_space,
                           AP4_UI08                         general_tier_flag,
                           AP4_UI08                         general_profile,
                           AP4_UI32                         general_profile_compatibility_flags,
                           AP4_UI64                         general_constraint_indicator_flags,
                           AP4_UI08                         general_level,
                           AP4_UI32                         min_spatial_segmentation,
                           AP4_UI08                         parallelism_type,
                           AP4_UI08                         chroma_format,
                           AP4_UI08                         luma_bit_depth,
                           AP4_UI08                         chroma_bit_depth,
                           AP4_UI16                         average_frame_rate,
                           AP4_UI08                         constant_frame_rate,
                           AP4_UI08                         num_temporal_layers,
                           AP4_UI08                         temporal_id_nested,
                           AP4_UI08                         nalu_length_size,
                           const AP4_Array<AP4_DataBuffer>& video_parameters,
                           const AP4_Array<AP4_DataBuffer>& sequence_parameters,
                           const AP4_Array<AP4_DataBuffer>& picture_parameters) :
    AP4_Atom(AP4_ATOM_TYPE_HVCC, AP4_ATOM_HEADER_SIZE),
    m_ConfigurationVersion(1),
    m_GeneralProfileSpace(general_profile_space),
    m_GeneralTierFlag(general_tier_flag),
    m_GeneralProfile(general_profile),
    m_GeneralProfileCompatibilityFlags(general_profile_compatibility_flags),
    m_GeneralConstraintIndicatorFlags(general_constraint_indicator_flags),
    m_GeneralLevel(general_level),
    m_Reserved1(0),
    m_MinSpatialSegmentation(min_spatial_segmentation),
    m_Reserved2(0),
    m_ParallelismType(parallelism_type),
    m_Reserved3(0),
    m_ChromaFormat(chroma_format),
    m_Reserved4(0),
    m_LumaBitDepth(luma_bit_depth),
    m_Reserved5(0),
    m_ChromaBitDepth(chroma_bit_depth),
    m_AverageFrameRate(average_frame_rate),
    m_ConstantFrameRate(constant_frame_rate),
    m_NumTemporalLayers(num_temporal_layers),
    m_TemporalIdNested(temporal_id_nested),
    m_NaluLengthSize(nalu_length_size)
{
    // deep copy of the parameters
    AP4_HvccAtom::Sequence vps_sequence;
    vps_sequence.m_NaluType = AP4_HEVC_NALU_TYPE_VPS_NUT;
    vps_sequence.m_ArrayCompleteness = 0;
    for (unsigned int i=0; i<video_parameters.ItemCount(); i++) {
        vps_sequence.m_Nalus.Append(video_parameters[i]);
    }
    if (vps_sequence.m_Nalus.ItemCount()) {
        m_Sequences.Append(vps_sequence);
    }
    
    AP4_HvccAtom::Sequence sps_sequence;
    sps_sequence.m_NaluType = AP4_HEVC_NALU_TYPE_SPS_NUT;
    sps_sequence.m_ArrayCompleteness = 0;
    for (unsigned int i=0; i<sequence_parameters.ItemCount(); i++) {
        sps_sequence.m_Nalus.Append(sequence_parameters[i]);
    }
    if (sps_sequence.m_Nalus.ItemCount()) {
        m_Sequences.Append(sps_sequence);
    }

    AP4_HvccAtom::Sequence pps_sequence;
    pps_sequence.m_NaluType = AP4_HEVC_NALU_TYPE_PPS_NUT;
    pps_sequence.m_ArrayCompleteness = 0;
    for (unsigned int i=0; i<picture_parameters.ItemCount(); i++) {
        pps_sequence.m_Nalus.Append(picture_parameters[i]);
    }
    if (pps_sequence.m_Nalus.ItemCount()) {
        m_Sequences.Append(pps_sequence);
    }
    
    UpdateRawBytes();
    m_Size32 += m_RawBytes.GetDataSize();
}