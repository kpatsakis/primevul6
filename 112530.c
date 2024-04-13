AP4_MetaData::Entry::RemoveFromFileDcf(AP4_File& file, AP4_Ordinal index)
{
    // look for 'odrm/odhe/udta'
    AP4_ContainerAtom* udta = AP4_DYNAMIC_CAST(AP4_ContainerAtom, file.FindChild("odrm/odhe/udta"));
    if (udta == NULL) return AP4_ERROR_NO_SUCH_ITEM;
                
    // remove the data atom in the entry
    AP4_UI32 type = AP4_BytesToUInt32BE((const unsigned char*)m_Key.GetName().GetChars());
    AP4_Result result = udta->DeleteChild(type, index);
    if (AP4_FAILED(result)) return result;
    
    return AP4_SUCCESS;
}