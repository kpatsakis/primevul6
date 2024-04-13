AP4_BinaryMetaDataValue::ToBytes(AP4_DataBuffer& bytes) const
{
    bytes.SetDataSize(m_Value.GetDataSize());
    AP4_CopyMemory(bytes.UseData(), m_Value.GetData(), m_Value.GetDataSize());
    return AP4_SUCCESS;
}