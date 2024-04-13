AP4_VisualSampleEntry::WriteFields(AP4_ByteStream& stream)
{
    AP4_Result result;
        
    // write the fields of the base class
    result = AP4_SampleEntry::WriteFields(stream);
    if (AP4_FAILED(result)) return result;

    // predefined1
    result = stream.WriteUI16(m_Predefined1);
    if (AP4_FAILED(result)) return result;
    
    // reserved2
    result = stream.WriteUI16(m_Reserved2);
    if (AP4_FAILED(result)) return result;
    
    // predefined2
    result = stream.Write(m_Predefined2, sizeof(m_Predefined2));
    if (AP4_FAILED(result)) return result;
    
    // width
    result = stream.WriteUI16(m_Width);
    if (AP4_FAILED(result)) return result;
    
    // height
    result = stream.WriteUI16(m_Height);
    if (AP4_FAILED(result)) return result;
    
    // horizontal resolution
    result = stream.WriteUI32(m_HorizResolution);
    if (AP4_FAILED(result)) return result;
    
    // vertical resolution
    result = stream.WriteUI32(m_VertResolution);
    if (AP4_FAILED(result)) return result;
    
    // reserved3
    result = stream.WriteUI32(m_Reserved3);
    if (AP4_FAILED(result)) return result;
    
    // frame count
    result = stream.WriteUI16(m_FrameCount);
    if (AP4_FAILED(result)) return result;
    
    // compressor name
    unsigned char compressor_name[32];
    unsigned int name_length = m_CompressorName.GetLength();
    if (name_length > 31) name_length = 31;
    compressor_name[0] = (unsigned char)name_length;
    for (unsigned int i=0; i<name_length; i++) {
        compressor_name[i+1] = m_CompressorName[i];
    }
    for (unsigned int i=name_length+1; i<32; i++) {
        compressor_name[i] = 0;
    }
    result = stream.Write(compressor_name, 32);
    if (AP4_FAILED(result)) return result;
    
    // depth
    result = stream.WriteUI16(m_Depth);
    if (AP4_FAILED(result)) return result;
    
    // predefined3
    result = stream.WriteUI16(m_Predefined3);
    if (AP4_FAILED(result)) return result;
    
    return result;
}