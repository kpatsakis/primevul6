AP4_AtomMetaDataValue::ToInteger() const
{
    long value;
    if (AP4_SUCCEEDED(m_DataAtom->LoadInteger(value))) {
        return value;
    } else {
        return 0;
    }
}