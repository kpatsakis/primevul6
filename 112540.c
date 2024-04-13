AP4_DcfdAtom::InspectFields(AP4_AtomInspector& inspector)
{
    inspector.AddField("duration", m_Duration);
    return AP4_SUCCESS;
}