static void sprintf_hexa( char *s, uint8_t *p_data, int i_data )
{
    static const char hex[16] = "0123456789abcdef";

    for( int i = 0; i < i_data; i++ )
    {
        s[2*i+0] = hex[(p_data[i]>>4)&0xf];
        s[2*i+1] = hex[(p_data[i]   )&0xf];
    }
    s[2*i_data] = '\0';
}