AP4_MetaData::Value::MapTypeToCategory(Type type)
{
    switch (type) {
        case AP4_MetaData::Value::TYPE_INT_08_BE:
        case AP4_MetaData::Value::TYPE_INT_16_BE:
        case AP4_MetaData::Value::TYPE_INT_32_BE:
            return AP4_MetaData::Value::TYPE_CATEGORY_INTEGER;

        case AP4_MetaData::Value::TYPE_STRING_UTF_8:
        case AP4_MetaData::Value::TYPE_STRING_UTF_16:
        case AP4_MetaData::Value::TYPE_STRING_PASCAL:
            return AP4_MetaData::Value::TYPE_CATEGORY_STRING;

        case AP4_MetaData::Value::TYPE_FLOAT_32_BE:
        case AP4_MetaData::Value::TYPE_FLOAT_64_BE:
            return AP4_MetaData::Value::TYPE_CATEGORY_FLOAT;
            
        default:
            return AP4_MetaData::Value::TYPE_CATEGORY_BINARY;
    }
}