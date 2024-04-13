AP4_DataAtom::GetValueType()
{
    switch (m_DataType) {
        case DATA_TYPE_BINARY:
            return AP4_MetaData::Value::TYPE_BINARY;

        case DATA_TYPE_SIGNED_INT_BE:
            switch (m_Size32-16) {
                case 1: return AP4_MetaData::Value::TYPE_INT_08_BE;
                case 2: return AP4_MetaData::Value::TYPE_INT_16_BE;
                case 4: return AP4_MetaData::Value::TYPE_INT_32_BE;
                default: return AP4_MetaData::Value::TYPE_BINARY;
            }
            break;
            
        case DATA_TYPE_STRING_UTF_8:
            return AP4_MetaData::Value::TYPE_STRING_UTF_8;
            
        case DATA_TYPE_STRING_UTF_16:
            return AP4_MetaData::Value::TYPE_STRING_UTF_16;
            
        case DATA_TYPE_STRING_PASCAL:
            return AP4_MetaData::Value::TYPE_STRING_PASCAL;

        case DATA_TYPE_GIF:
            return AP4_MetaData::Value::TYPE_GIF;

        case DATA_TYPE_JPEG:
            return AP4_MetaData::Value::TYPE_JPEG;

        default:
            return AP4_MetaData::Value::TYPE_BINARY;
    }
    
    // unreachable - return AP4_MetaData::Value::TYPE_BINARY;
}