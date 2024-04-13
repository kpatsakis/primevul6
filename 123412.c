AP4_HvccAtom::AP4_HvccAtom() :
    AP4_Atom(AP4_ATOM_TYPE_HVCC, AP4_ATOM_HEADER_SIZE),
    m_ConfigurationVersion(1),
    m_GeneralProfileSpace(0),
    m_GeneralTierFlag(0),
    m_GeneralProfile(0),
    m_GeneralProfileCompatibilityFlags(0),
    m_GeneralConstraintIndicatorFlags(0),
    m_GeneralLevel(0),
    m_Reserved1(0),
    m_MinSpatialSegmentation(0),
    m_Reserved2(0),
    m_ParallelismType(0),
    m_Reserved3(0),
    m_ChromaFormat(0),
    m_Reserved4(0),
    m_LumaBitDepth(8),
    m_Reserved5(0),
    m_ChromaBitDepth(8),
    m_AverageFrameRate(0),
    m_ConstantFrameRate(0),
    m_NumTemporalLayers(0),
    m_TemporalIdNested(0),
    m_NaluLengthSize(4)
{
    UpdateRawBytes();
    m_Size32 += m_RawBytes.GetDataSize();
}