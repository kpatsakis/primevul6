static void MP4_FreeBox_keys( MP4_Box_t *p_box )
{
    for( uint32_t i=0; i<p_box->data.p_keys->i_entry_count; i++ )
        free( p_box->data.p_keys->p_entries[i].psz_value );
    free( p_box->data.p_keys->p_entries );
}