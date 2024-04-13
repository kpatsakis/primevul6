AP4_AtomMetaDataValue::ToString() const
{
    char string[256] = "";

    AP4_MetaData::Value::Type value_type = m_DataAtom->GetValueType();
    switch (AP4_MetaData::Value::MapTypeToCategory(value_type)) {
        case AP4_MetaData::Value::TYPE_CATEGORY_INTEGER: 
            {
                long value;
                if (AP4_SUCCEEDED(m_DataAtom->LoadInteger(value))) {
                    if (m_Meaning == MEANING_BOOLEAN) {
                        if (value) {
                            return "True";
                        } else {
                            return "False";
                        }
                    } else if (m_Meaning == MEANING_FILE_KIND) {
                        if (value >= 0 && ((unsigned int)value) <= sizeof(Ap4StikNames)/sizeof(Ap4StikNames[0])) {
                            AP4_FormatString(string, sizeof(string), "(%ld) %s", value, Ap4StikNames[value]);
                        } else {
                            return "Unknown";
                        }
                    } else {
                        AP4_FormatString(string, sizeof(string), "%ld", value);
                    }
                }
                return AP4_String((const char*)string);
                break;
            }

        case AP4_MetaData::Value::TYPE_CATEGORY_STRING:
            {
                AP4_String* category_string;
                if (AP4_SUCCEEDED(m_DataAtom->LoadString(category_string))) {
                    AP4_String result(*category_string);
                    delete category_string;
                    return result;
                }
                break;
            }

        case AP4_MetaData::Value::TYPE_CATEGORY_BINARY:
            {
                AP4_DataBuffer data;
                if (AP4_SUCCEEDED(m_DataAtom->LoadBytes(data))) {
                    if (m_Meaning == MEANING_ID3_GENRE && data.GetDataSize() == 2) {
                        unsigned int genre = (data.GetData()[0])*256+data.GetData()[1];
                        if (genre >= 1 && genre <= sizeof(Ap4Id3Genres)/sizeof(Ap4Id3Genres[0])) {
                            AP4_FormatString(string, sizeof(string), "(%d) %s", genre, Ap4Id3Genres[genre-1]);
                            return AP4_String((const char*)string);
                        } else {
                            return "Unknown";
                        }
                    } else if (m_Meaning == MEANING_BINARY_ENCODED_CHARS) {
                        AP4_String result;
                        result.Assign((const char*)data.GetData(), data.GetDataSize());
                        return result;
                    } else {
                        unsigned int dump_length = data.GetDataSize();
                        bool truncate = false;
                        if (dump_length > 16) {
                            dump_length = 16;
                            truncate = true;
                        }
                        char* out = string;
                        for (unsigned int i=0; i<dump_length; i++) {
                            AP4_FormatString(out, sizeof(string)-(out-string), "%02x ", data.GetData()[i]);
                            out += 3;
                        }
                        if (truncate) {
                            *out++='.'; *out++='.'; *out++='.'; *out++=' ';
                        }
                        AP4_FormatString(out, sizeof(string)-(out-string), "[%d bytes]", (int)data.GetDataSize());
                    }
                }
                return AP4_String(string);
            }
        default:
            return AP4_String();
    }

    return AP4_String();
}