static void put_amf_bool(AVIOContext *pb, int b)
{
    avio_w8(pb, AMF_DATA_TYPE_BOOL);
    avio_w8(pb, !!b);
}