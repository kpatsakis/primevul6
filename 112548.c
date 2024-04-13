AP4_MetaData::AddDcfStringEntry(AP4_DcfStringAtom* atom, const char* namespc)
{
    AP4_String key_name;
    ResolveKeyName(atom->GetType(), key_name);
    
    AP4_MetaData::Value* value = new AP4_StringMetaDataValue(atom->GetValue().GetChars());
    m_Entries.Add(new Entry(key_name.GetChars(), namespc, value));
    
    return AP4_SUCCESS;
}