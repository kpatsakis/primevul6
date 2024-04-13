static int MP4_ReadBox_tfra( stream_t *p_stream, MP4_Box_t *p_box )
{
#define READ_VARIABLE_LENGTH(lengthvar, p_array) switch (lengthvar)\
{\
    case 0:\
        MP4_GET1BYTE( p_array[i] );\
        break;\
    case 1:\
        MP4_GET2BYTES( *((uint16_t *)&p_array[i*2]) );\
        break;\
    case 2:\
        MP4_GET3BYTES( *((uint32_t *)&p_array[i*4]) );\
        break;\
    case 3:\
        MP4_GET4BYTES( *((uint32_t *)&p_array[i*4]) );\
        break;\
    default:\
        goto error;\
}
#define FIX_VARIABLE_LENGTH(lengthvar) if ( lengthvar == 3 ) lengthvar = 4

    uint32_t i_number_of_entries;
    MP4_READBOX_ENTER( MP4_Box_data_tfra_t );
    MP4_Box_data_tfra_t *p_tfra = p_box->data.p_tfra;
    MP4_GETVERSIONFLAGS( p_box->data.p_tfra );
    if ( p_tfra->i_version > 1 )
        MP4_READBOX_EXIT( 0 );
    MP4_GET4BYTES( p_tfra->i_track_ID );
    uint32_t i_lengths = 0;
    MP4_GET4BYTES( i_lengths );
    MP4_GET4BYTES( p_tfra->i_number_of_entries );
    i_number_of_entries = p_tfra->i_number_of_entries;
    p_tfra->i_length_size_of_traf_num = i_lengths >> 4;
    p_tfra->i_length_size_of_trun_num = ( i_lengths & 0x0c ) >> 2;
    p_tfra->i_length_size_of_sample_num = i_lengths & 0x03;

    size_t size = 4 + 4*p_tfra->i_version; /* size in {4, 8} */
    p_tfra->p_time = calloc( i_number_of_entries, size );
    p_tfra->p_moof_offset = calloc( i_number_of_entries, size );

    size = 1 + p_tfra->i_length_size_of_traf_num; /* size in [|1, 4|] */
    if ( size == 3 ) size++;
    p_tfra->p_traf_number = calloc( i_number_of_entries, size );
    size = 1 + p_tfra->i_length_size_of_trun_num;
    if ( size == 3 ) size++;
    p_tfra->p_trun_number = calloc( i_number_of_entries, size );
    size = 1 + p_tfra->i_length_size_of_sample_num;
    if ( size == 3 ) size++;
    p_tfra->p_sample_number = calloc( i_number_of_entries, size );

    if( !p_tfra->p_time || !p_tfra->p_moof_offset || !p_tfra->p_traf_number
                        || !p_tfra->p_trun_number || !p_tfra->p_sample_number )
        goto error;

    int i_fields_length = 3 + p_tfra->i_length_size_of_traf_num
            + p_tfra->i_length_size_of_trun_num
            + p_tfra->i_length_size_of_sample_num;

    uint32_t i;
    for( i = 0; i < i_number_of_entries; i++ )
    {

        if( p_tfra->i_version == 1 )
        {
            if ( i_read < i_fields_length + 16 )
                break;
            MP4_GET8BYTES( *((uint64_t *)&p_tfra->p_time[i*2]) );
            MP4_GET8BYTES( *((uint64_t *)&p_tfra->p_moof_offset[i*2]) );
        }
        else
        {
            if ( i_read < i_fields_length + 8 )
                break;
            MP4_GET4BYTES( p_tfra->p_time[i] );
            MP4_GET4BYTES( p_tfra->p_moof_offset[i] );
        }

        READ_VARIABLE_LENGTH(p_tfra->i_length_size_of_traf_num, p_tfra->p_traf_number);
        READ_VARIABLE_LENGTH(p_tfra->i_length_size_of_trun_num, p_tfra->p_trun_number);
        READ_VARIABLE_LENGTH(p_tfra->i_length_size_of_sample_num, p_tfra->p_sample_number);
    }
    if ( i < i_number_of_entries )
        i_number_of_entries = i;

    FIX_VARIABLE_LENGTH(p_tfra->i_length_size_of_traf_num);
    FIX_VARIABLE_LENGTH(p_tfra->i_length_size_of_trun_num);
    FIX_VARIABLE_LENGTH(p_tfra->i_length_size_of_sample_num);

#ifdef MP4_ULTRA_VERBOSE
    for( i = 0; i < i_number_of_entries; i++ )
    {
        if( p_tfra->i_version == 0 )
        {
            msg_Dbg( p_stream, "tfra[%"PRIu32"] time[%"PRIu32"]: %"PRIu32", "
                               "moof_offset[%"PRIu32"]: %"PRIu32"",
                     p_tfra->i_track_ID,
                     i, p_tfra->p_time[i],
                     i, p_tfra->p_moof_offset[i] );
        }
        else
        {
            msg_Dbg( p_stream, "tfra[%"PRIu32"] time[%"PRIu32"]: %"PRIu64", "
                               "moof_offset[%"PRIu32"]: %"PRIu64"",
                     p_tfra->i_track_ID,
                     i, ((uint64_t *)(p_tfra->p_time))[i],
                     i, ((uint64_t *)(p_tfra->p_moof_offset))[i] );
        }
    }
#endif
#ifdef MP4_VERBOSE
    msg_Dbg( p_stream, "tfra[%"PRIu32"] %"PRIu32" entries",
             p_tfra->i_track_ID, i_number_of_entries );
#endif

    MP4_READBOX_EXIT( 1 );
error:
    MP4_READBOX_EXIT( 0 );

#undef READ_VARIABLE_LENGTH
#undef FIX_VARIABLE_LENGTH
}