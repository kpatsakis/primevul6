AP4_AtomFactory::GetContext(AP4_Ordinal depth) 
{
    AP4_Ordinal available = m_ContextStack.ItemCount();
    if (depth >= available) return 0;
    return m_ContextStack[available-depth-1];
}