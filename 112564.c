AP4_DcfStringAtom::InspectFields(AP4_AtomInspector& inspector)
{
    inspector.AddField("value", m_Value.GetChars());
    return AP4_SUCCESS;
}