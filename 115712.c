AP4_DefaultFragmentHandler::ProcessSample(AP4_DataBuffer& data_in, AP4_DataBuffer& data_out)
{
    if (m_TrackHandler == NULL) {
        data_out.SetData(data_in.GetData(), data_in.GetDataSize());
        return AP4_SUCCESS;
    }
    return m_TrackHandler->ProcessSample(data_in, data_out);
}