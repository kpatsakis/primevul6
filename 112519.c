AP4_MetaDataAtomTypeHandler::IsTypeInList(AP4_Atom::Type type, const AP4_MetaDataAtomTypeHandler::TypeList& list)
{
    for (unsigned int i=0; i<list.m_Size; i++) {
        if (type == list.m_Types[i]) return true;
    }
    return false;
}