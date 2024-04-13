static void jpeg2000_init_static_data(AVCodec *codec)
{
    ff_jpeg2000_init_tier1_luts();
    ff_mqc_init_context_tables();
}