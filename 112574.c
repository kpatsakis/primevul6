AP4_MetaData::ParseMoov(AP4_MoovAtom* moov)
{
    // look for a 'meta' atom with 'hdlr' type 'mdir'
    AP4_HdlrAtom* hdlr = AP4_DYNAMIC_CAST(AP4_HdlrAtom, moov->FindChild("udta/meta/hdlr"));
    if (hdlr == NULL || hdlr->GetHandlerType() != AP4_HANDLER_TYPE_MDIR) return AP4_ERROR_NO_SUCH_ITEM;

    // get the list of entries
    AP4_ContainerAtom* ilst = AP4_DYNAMIC_CAST(AP4_ContainerAtom, moov->FindChild("udta/meta/ilst"));
    if (ilst == NULL) return AP4_ERROR_NO_SUCH_ITEM;
    
    AP4_List<AP4_Atom>::Item* ilst_item = ilst->GetChildren().FirstItem();
    while (ilst_item) {
        AP4_ContainerAtom* entry_atom = AP4_DYNAMIC_CAST(AP4_ContainerAtom, ilst_item->GetData()); 
        if (entry_atom) {
            AddIlstEntries(entry_atom, "meta");
        }
        ilst_item = ilst_item->GetNext();
    }
    
    return AP4_SUCCESS;
}