static void MP4_FreeBox_stsdext_chan( MP4_Box_t *p_box )
{
    MP4_Box_data_chan_t *p_chan = p_box->data.p_chan;
    free( p_chan->layout.p_descriptions );
}