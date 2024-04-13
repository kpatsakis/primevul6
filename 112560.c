AP4_DataAtom::LoadInteger(long& value) 
{
    AP4_Result result = AP4_FAILURE;
    value = 0;
    if (m_Source == NULL) return AP4_SUCCESS;
    AP4_LargeSize size = 0;
    m_Source->GetSize(size);
    if (size > 4) {
        return AP4_ERROR_OUT_OF_RANGE;
    }
    unsigned char bytes[4];
    m_Source->Seek(0);
    m_Source->Read(bytes, (AP4_Size)size);
    result = AP4_SUCCESS;
    switch (size) {
        case 1: value = bytes[0]; break;
        case 2: value = AP4_BytesToInt16BE(bytes); break;
        case 4: value = AP4_BytesToInt32BE(bytes); break;
        default: value = 0; result = AP4_ERROR_INVALID_FORMAT; break;
    }
    return result;
}