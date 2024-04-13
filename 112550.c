AP4_DataAtom::LoadBytes(AP4_DataBuffer& bytes)
{
    if (m_Source == NULL) {
        bytes.SetDataSize(0);
        return AP4_SUCCESS;
    }
    AP4_LargeSize size = 0;
    m_Source->GetSize(size);
    if (size > AP4_DATA_ATOM_MAX_SIZE) return AP4_ERROR_OUT_OF_RANGE;
    bytes.SetDataSize((AP4_Size)size);
    m_Source->Seek(0);
    AP4_Result result = m_Source->Read(bytes.UseData(), (AP4_Size)size);
    if (AP4_FAILED(result)) {
        bytes.SetDataSize(0);
    }
    return result;
}