AP4_MetaData::AddDcfdEntry(AP4_DcfdAtom* atom, const char* namespc)
{
    AP4_String key_name;
    ResolveKeyName(atom->GetType(), key_name);
    
    AP4_MetaData::Value* value = new AP4_IntegerMetaDataValue(AP4_MetaData::Value::TYPE_INT_32_BE, 
                                                              atom->GetDuration());
    m_Entries.Add(new Entry(key_name.GetChars(), namespc, value));
    
    return AP4_SUCCESS;
}