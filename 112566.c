AP4_MetaData::Entry::AddToFile(AP4_File& file, AP4_Ordinal index)
{
    // check that we have a correct entry
    if (m_Value == NULL) return AP4_ERROR_INVALID_STATE;
 
    // check the namespace of the key to know where to add the atom
    if (m_Key.GetNamespace() == "meta") {
        return AddToFileIlst(file, index);
    } else if (m_Key.GetNamespace() == "dcf") {
        return AddToFileDcf(file, index);
    } else {
        // custom namespace
        return AddToFileIlst(file, index);
    }
}