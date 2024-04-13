static int MP4_ReadBox_tkhd(  stream_t *p_stream, MP4_Box_t *p_box )
{
#ifdef MP4_VERBOSE
    char s_creation_time[128];
    char s_modification_time[128];
    char s_duration[128];
#endif
    MP4_READBOX_ENTER( MP4_Box_data_tkhd_t );

    MP4_GETVERSIONFLAGS( p_box->data.p_tkhd );

    if( p_box->data.p_tkhd->i_version )
    {
        MP4_GET8BYTES( p_box->data.p_tkhd->i_creation_time );
        MP4_GET8BYTES( p_box->data.p_tkhd->i_modification_time );
        MP4_GET4BYTES( p_box->data.p_tkhd->i_track_ID );
        MP4_GET4BYTES( p_box->data.p_tkhd->i_reserved );
        MP4_GET8BYTES( p_box->data.p_tkhd->i_duration );
    }
    else
    {
        MP4_GET4BYTES( p_box->data.p_tkhd->i_creation_time );
        MP4_GET4BYTES( p_box->data.p_tkhd->i_modification_time );
        MP4_GET4BYTES( p_box->data.p_tkhd->i_track_ID );
        MP4_GET4BYTES( p_box->data.p_tkhd->i_reserved );
        MP4_GET4BYTES( p_box->data.p_tkhd->i_duration );
    }

    for( unsigned i = 0; i < 2; i++ )
    {
        MP4_GET4BYTES( p_box->data.p_tkhd->i_reserved2[i] );
    }
    MP4_GET2BYTES( p_box->data.p_tkhd->i_layer );
    MP4_GET2BYTES( p_box->data.p_tkhd->i_predefined );
    MP4_GET2BYTES( p_box->data.p_tkhd->i_volume );
    MP4_GET2BYTES( p_box->data.p_tkhd->i_reserved3 );

    for( unsigned i = 0; i < 9; i++ )
    {
        MP4_GET4BYTES( p_box->data.p_tkhd->i_matrix[i] );
    }
    MP4_GET4BYTES( p_box->data.p_tkhd->i_width );
    MP4_GET4BYTES( p_box->data.p_tkhd->i_height );

    double rotation;    //angle in degrees to be rotated clockwise
    double scale[2];    // scale factor; sx = scale[0] , sy = scale[1]
    double translate[2];// amount to translate; tx = translate[0] , ty = translate[1]

    int32_t *matrix = p_box->data.p_tkhd->i_matrix;

    translate[0] = conv_fx(matrix[6]);
    translate[1] = conv_fx(matrix[7]);

    scale[0] = sqrt(conv_fx(matrix[0]) * conv_fx(matrix[0]) +
                    conv_fx(matrix[3]) * conv_fx(matrix[3]));
    scale[1] = sqrt(conv_fx(matrix[1]) * conv_fx(matrix[1]) +
                    conv_fx(matrix[4]) * conv_fx(matrix[4]));

    rotation = atan2(conv_fx(matrix[1]) / scale[1], conv_fx(matrix[0]) / scale[0]) * 180 / M_PI;

    if (rotation < 0)
        rotation += 360.;

    p_box->data.p_tkhd->f_rotation = rotation;

#ifdef MP4_VERBOSE
    MP4_ConvertDate2Str( s_creation_time, p_box->data.p_mvhd->i_creation_time, false );
    MP4_ConvertDate2Str( s_modification_time, p_box->data.p_mvhd->i_modification_time, false );
    MP4_ConvertDate2Str( s_duration, p_box->data.p_mvhd->i_duration, true );

    msg_Dbg( p_stream, "read box: \"tkhd\" creation %s modification %s duration %s track ID %d layer %d volume %f rotation %f scaleX %f scaleY %f translateX %f translateY %f width %f height %f. "
            "Matrix: %i %i %i %i %i %i %i %i %i",
                  s_creation_time,
                  s_modification_time,
                  s_duration,
                  p_box->data.p_tkhd->i_track_ID,
                  p_box->data.p_tkhd->i_layer,
                  (float)p_box->data.p_tkhd->i_volume / 256 ,
                  rotation,
                  scale[0],
                  scale[1],
                  translate[0],
                  translate[1],
                  (float)p_box->data.p_tkhd->i_width / BLOCK16x16,
                  (float)p_box->data.p_tkhd->i_height / BLOCK16x16,
                  p_box->data.p_tkhd->i_matrix[0],
                  p_box->data.p_tkhd->i_matrix[1],
                  p_box->data.p_tkhd->i_matrix[2],
                  p_box->data.p_tkhd->i_matrix[3],
                  p_box->data.p_tkhd->i_matrix[4],
                  p_box->data.p_tkhd->i_matrix[5],
                  p_box->data.p_tkhd->i_matrix[6],
                  p_box->data.p_tkhd->i_matrix[7],
                  p_box->data.p_tkhd->i_matrix[8] );
#endif
    MP4_READBOX_EXIT( 1 );
}