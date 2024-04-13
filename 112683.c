AP4_DefaultAtomFactory::Initialize()
{
    // register built-in type handlers
    AP4_Result result = AddTypeHandler(new AP4_MetaDataAtomTypeHandler(this));
    if (AP4_SUCCEEDED(result)) m_Initialized = true;
    return result;
}