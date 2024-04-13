AP4_AvccAtom::Create(AP4_Size size, AP4_ByteStream& stream)
{
    // read the raw bytes in a buffer
    unsigned int payload_size = size-AP4_ATOM_HEADER_SIZE;
    AP4_DataBuffer payload_data(payload_size);
    AP4_Result result = stream.Read(payload_data.UseData(), payload_size);
    if (AP4_FAILED(result)) return NULL;
    
    // check the version
    const AP4_UI08* payload = payload_data.GetData();
    if (payload[0] != 1) {
        return NULL;
    }

    // check the size
    if (payload_size < 6) return NULL;
    unsigned int num_seq_params = payload[5]&31;
    unsigned int cursor = 6;
    for (unsigned int i=0; i<num_seq_params; i++) {
        if (cursor+2 > payload_size) return NULL;
        cursor += 2+AP4_BytesToInt16BE(&payload[cursor]);
        if (cursor > payload_size) return NULL;
    }
    unsigned int num_pic_params = payload[cursor++];
    if (cursor > payload_size) return NULL;
    for (unsigned int i=0; i<num_pic_params; i++) {
        if (cursor+2 > payload_size) return NULL;
        cursor += 2+AP4_BytesToInt16BE(&payload[cursor]);
        if (cursor > payload_size) return NULL;
    }
    return new AP4_AvccAtom(size, payload);
}