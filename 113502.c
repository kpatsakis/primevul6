AP4_SampleEntry::Inspect(AP4_AtomInspector& inspector)
{
    // inspect the header
    InspectHeader(inspector);

    // inspect the fields
    InspectFields(inspector);

    // inspect children
    m_Children.Apply(AP4_AtomListInspector(inspector));

    // finish
    inspector.EndAtom();

    return AP4_SUCCESS;
}