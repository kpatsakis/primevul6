AP4_MetaData::Entry::RemoveFromFile(AP4_File& file, AP4_Ordinal index)
{
    // check the namespace of the key to know where to add the atom
    if (m_Key.GetNamespace() == "meta") {
        return RemoveFromFileIlst(file, index);
    } else if (m_Key.GetNamespace() == "dcf") {
        return RemoveFromFileDcf(file, index);
    } else {
        // custom namespace
        return RemoveFromFileIlst(file, index);
    }
}