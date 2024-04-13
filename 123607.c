static inline bool SchroSetEnum( const encoder_t *p_enc, int i_list_size, const char *list[],
                  const char *psz_name,  const char *psz_name_text,  const char *psz_value)
{
    encoder_sys_t *p_sys = p_enc->p_sys;
    if( list && psz_name_text && psz_name && psz_value ) {
        for( int i = 0; i < i_list_size; ++i ) {
            if( strcmp( list[i], psz_value ) )
                continue;
            schro_encoder_setting_set_double( p_sys->p_schro, psz_name, i );
            return true;
        }
        msg_Err( p_enc, "Invalid %s: %s", psz_name_text, psz_value );
    }
    return false;
}