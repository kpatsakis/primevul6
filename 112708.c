AP4_HdlrAtom::InspectFields(AP4_AtomInspector& inspector)
{
    char type[5];
    AP4_FormatFourChars(type, m_HandlerType);
    inspector.AddField("handler_type", type);
    inspector.AddField("handler_name", m_HandlerName.GetChars());

    return AP4_SUCCESS;
}