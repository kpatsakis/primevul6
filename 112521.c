AP4_DataAtom::InspectFields(AP4_AtomInspector& inspector)
{
    inspector.AddField("type", m_DataType);
    inspector.AddField("lang", m_DataLang);
    if (m_DataType == DATA_TYPE_STRING_UTF_8) {
        AP4_String* str;
        if (AP4_SUCCEEDED(LoadString(str))) {
            inspector.AddField("value", str->GetChars());
            delete str;
        }
    } else if (m_DataType == DATA_TYPE_SIGNED_INT_BE) {
        long value;
        if (AP4_SUCCEEDED(LoadInteger(value))) {
            inspector.AddField("value", value);
        }
    }

    return AP4_SUCCESS;
}