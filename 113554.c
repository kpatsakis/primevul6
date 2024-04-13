AP4_SampleEntry::OnChildChanged(AP4_Atom*)
{
    // recompute our size
    AP4_UI64 size = GetHeaderSize()+GetFieldsSize();
    m_Children.Apply(AP4_AtomSizeAdder(size));
    m_Size32 = (AP4_UI32)size;

    // update our parent
    if (m_Parent) m_Parent->OnChildChanged(this);
}