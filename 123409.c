AP4_HvccAtom::InspectFields(AP4_AtomInspector& inspector)
{
    inspector.AddField("Configuration Version", m_ConfigurationVersion);
    inspector.AddField("Profile Space", m_GeneralProfileSpace);
    const char* profile_name = GetProfileName(m_GeneralProfileSpace, m_GeneralProfile);
    if (profile_name) {
        inspector.AddField("Profile", profile_name);
    } else {
        inspector.AddField("Profile", m_GeneralProfile);
    }
    inspector.AddField("Tier", m_GeneralTierFlag);
    inspector.AddField("Profile Compatibility", m_GeneralProfileCompatibilityFlags, AP4_AtomInspector::HINT_HEX);
    inspector.AddField("Constraint", m_GeneralConstraintIndicatorFlags, AP4_AtomInspector::HINT_HEX);
    inspector.AddField("Level", m_GeneralLevel);
    inspector.AddField("Min Spatial Segmentation", m_MinSpatialSegmentation);
    inspector.AddField("Parallelism Type", m_ParallelismType);
    inspector.AddField("Chroma Format", m_ChromaFormat);
    inspector.AddField("Chroma Depth", m_ChromaBitDepth);
    inspector.AddField("Luma Depth", m_LumaBitDepth);
    inspector.AddField("Average Frame Rate", m_AverageFrameRate);
    inspector.AddField("Constant Frame Rate", m_ConstantFrameRate);
    inspector.AddField("Number Of Temporal Layers", m_NumTemporalLayers);
    inspector.AddField("Temporal Id Nested", m_TemporalIdNested);
    inspector.AddField("NALU Length Size", m_NaluLengthSize);
    return AP4_SUCCESS;
}