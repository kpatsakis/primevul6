static void MP4_ConvertDate2Str( char *psz, uint64_t i_date, bool b_relative )
{
    int i_day;
    int i_hour;
    int i_min;
    int i_sec;

    /* date begin at 1 jan 1904 */
    if ( !b_relative )
        i_date += ((INT64_C(1904) * 365) + 17) * 24 * 60 * 60;

    i_day = i_date / ( 60*60*24);
    i_hour = ( i_date /( 60*60 ) ) % 60;
    i_min  = ( i_date / 60 ) % 60;
    i_sec =  i_date % 60;
    sprintf( psz, "%dd-%2.2dh:%2.2dm:%2.2ds", i_day, i_hour, i_min, i_sec );
}