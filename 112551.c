AP4_3GppLocalizedStringAtom::InspectFields(AP4_AtomInspector& inspector)
{
    inspector.AddField("language", GetLanguage());
    inspector.AddField("value", m_Value.GetChars());
    return AP4_SUCCESS;
}