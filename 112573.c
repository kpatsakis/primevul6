AP4_AtomMetaDataValue::ToBytes(AP4_DataBuffer& bytes) const
{
    return m_DataAtom->LoadBytes(bytes);
}