AP4_HvccAtom::AP4_HvccAtom(const AP4_HvccAtom& other) :
    AP4_Atom(AP4_ATOM_TYPE_HVCC, other.m_Size32),
    m_ConfigurationVersion(other.m_ConfigurationVersion),
    m_GeneralProfileSpace(other.m_GeneralProfileSpace),
    m_GeneralTierFlag(other.m_GeneralTierFlag),
    m_GeneralProfile(other.m_GeneralProfile),
    m_GeneralProfileCompatibilityFlags(other.m_GeneralProfileCompatibilityFlags),
    m_GeneralConstraintIndicatorFlags(other.m_GeneralConstraintIndicatorFlags),
    m_GeneralLevel(other.m_GeneralLevel),
    m_Reserved1(other.m_Reserved1),
    m_MinSpatialSegmentation(other.m_MinSpatialSegmentation),
    m_Reserved2(other.m_Reserved2),
    m_ParallelismType(other.m_ParallelismType),
    m_Reserved3(other.m_Reserved3),
    m_ChromaFormat(other.m_ChromaFormat),
    m_Reserved4(other.m_Reserved4),
    m_LumaBitDepth(other.m_LumaBitDepth),
    m_Reserved5(other.m_Reserved5),
    m_ChromaBitDepth(other.m_ChromaBitDepth),
    m_AverageFrameRate(other.m_AverageFrameRate),
    m_ConstantFrameRate(other.m_ConstantFrameRate),
    m_NumTemporalLayers(other.m_NumTemporalLayers),
    m_TemporalIdNested(other.m_TemporalIdNested),
    m_NaluLengthSize(other.m_NaluLengthSize),
    m_RawBytes(other.m_RawBytes)
{
    // deep copy of the parameters
    unsigned int i = 0;
    for (i=0; i<other.m_Sequences.ItemCount(); i++) {
        m_Sequences.Append(other.m_Sequences[i]);
    }
}