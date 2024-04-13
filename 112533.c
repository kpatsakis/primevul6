AP4_IntegerMetaDataValue::ToString() const
{
    char value[16];
    AP4_FormatString(value, sizeof(value), "%ld", m_Value);
    return AP4_String(value);
}