AP4_MetaDataAtomTypeHandler::CreateAtom(AP4_Atom::Type  type,
                                        AP4_UI32        size,
                                        AP4_ByteStream& stream,
                                        AP4_Atom::Type  context,
                                        AP4_Atom*&      atom)
{
    atom = NULL;

    if (context == AP4_ATOM_TYPE_ILST) {
        if (IsTypeInList(type, IlstTypeList)) {
            m_AtomFactory->PushContext(type);
            atom = AP4_ContainerAtom::Create(type, size, false, false, stream, *m_AtomFactory);
            m_AtomFactory->PopContext();
        }
    } else if (type == AP4_ATOM_TYPE_DATA) {
        if (IsTypeInList(context, IlstTypeList)) {
            atom = new AP4_DataAtom(size, stream);
        }
    } else if (context == AP4_ATOM_TYPE_dddd) {
        if (type == AP4_ATOM_TYPE_MEAN || type == AP4_ATOM_TYPE_NAME) {
            atom = new AP4_MetaDataStringAtom(type, size, stream);
        }
    } else if (context == AP4_ATOM_TYPE_UDTA) {
        if (IsTypeInList(type, _3gppLocalizedStringTypeList)) {
            atom = AP4_3GppLocalizedStringAtom::Create(type, size, stream);
        } else if (IsTypeInList(type, DcfStringTypeList)) {
            atom = AP4_DcfStringAtom::Create(type, size, stream);
        } else if (type == AP4_ATOM_TYPE_DCFD) {
            atom = AP4_DcfdAtom::Create(size, stream);
        }
    }

    return atom?AP4_SUCCESS:AP4_FAILURE;
}