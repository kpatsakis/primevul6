static void put_amf_double(AVIOContext *pb, double d)
{
    avio_w8(pb, AMF_DATA_TYPE_NUMBER);
    avio_wb64(pb, av_double2int(d));
}