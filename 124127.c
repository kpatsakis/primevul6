bool  PxMEncoder::write( const Mat& img, const std::vector<int>& params )
{
    bool isBinary = true;

    int  width = img.cols, height = img.rows;
    int  _channels = img.channels(), depth = (int)img.elemSize1()*8;
    int  channels = _channels > 1 ? 3 : 1;
    int  fileStep = width*(int)img.elemSize();
    int  x, y;

    for( size_t i = 0; i < params.size(); i += 2 )
        if( params[i] == CV_IMWRITE_PXM_BINARY )
            isBinary = params[i+1] != 0;

    WLByteStream strm;

    if( m_buf )
    {
        if( !strm.open(*m_buf) )
            return false;
        int t = CV_MAKETYPE(img.depth(), channels);
        m_buf->reserve( alignSize(256 + (isBinary ? fileStep*height :
            ((t == CV_8UC1 ? 4 : t == CV_8UC3 ? 4*3+2 :
            t == CV_16UC1 ? 6 : 6*3+2)*width+1)*height), 256));
    }
    else if( !strm.open(m_filename) )
        return false;

    int  lineLength;
    int  bufferSize = 128; // buffer that should fit a header

    if( isBinary )
        lineLength = width * (int)img.elemSize();
    else
        lineLength = (6 * channels + (channels > 1 ? 2 : 0)) * width + 32;

    if( bufferSize < lineLength )
        bufferSize = lineLength;

    AutoBuffer<char> _buffer(bufferSize);
    char* buffer = _buffer;

    // write header;
    sprintf( buffer, "P%c\n# Generated by OpenCV %s\n%d %d\n%d\n",
             '2' + (channels > 1 ? 1 : 0) + (isBinary ? 3 : 0),
             CV_VERSION,
             width, height, (1 << depth) - 1 );

    strm.putBytes( buffer, (int)strlen(buffer) );

    for( y = 0; y < height; y++ )
    {
        const uchar* const data = img.ptr(y);
        if( isBinary )
        {
            if( _channels == 3 )
            {
                if( depth == 8 )
                    icvCvt_BGR2RGB_8u_C3R( (const uchar*)data, 0,
                        (uchar*)buffer, 0, cvSize(width,1) );
                else
                    icvCvt_BGR2RGB_16u_C3R( (const ushort*)data, 0,
                        (ushort*)buffer, 0, cvSize(width,1) );
            }

            // swap endianness if necessary
            if( depth == 16 && !isBigEndian() )
            {
                if( _channels == 1 )
                    memcpy( buffer, data, fileStep );
                for( x = 0; x < width*channels*2; x += 2 )
                {
                    uchar v = buffer[x];
                    buffer[x] = buffer[x + 1];
                    buffer[x + 1] = v;
                }
            }
            strm.putBytes( (channels > 1 || depth > 8) ? buffer : (const char*)data, fileStep );
        }
        else
        {
            char* ptr = buffer;

            if( channels > 1 )
            {
                if( depth == 8 )
                {
                    for( x = 0; x < width*channels; x += channels )
                    {
                        sprintf( ptr, "% 4d", data[x + 2] );
                        ptr += 4;
                        sprintf( ptr, "% 4d", data[x + 1] );
                        ptr += 4;
                        sprintf( ptr, "% 4d", data[x] );
                        ptr += 4;
                        *ptr++ = ' ';
                        *ptr++ = ' ';
                    }
                }
                else
                {
                    for( x = 0; x < width*channels; x += channels )
                    {
                        sprintf( ptr, "% 6d", ((const ushort *)data)[x + 2] );
                        ptr += 6;
                        sprintf( ptr, "% 6d", ((const ushort *)data)[x + 1] );
                        ptr += 6;
                        sprintf( ptr, "% 6d", ((const ushort *)data)[x] );
                        ptr += 6;
                        *ptr++ = ' ';
                        *ptr++ = ' ';
                    }
                }
            }
            else
            {
                if( depth == 8 )
                {
                    for( x = 0; x < width; x++ )
                    {
                        sprintf( ptr, "% 4d", data[x] );
                        ptr += 4;
                    }
                }
                else
                {
                    for( x = 0; x < width; x++ )
                    {
                        sprintf( ptr, "% 6d", ((const ushort *)data)[x] );
                        ptr += 6;
                    }
                }
            }

            *ptr++ = '\n';

            strm.putBytes( buffer, (int)(ptr - buffer) );
        }
    }

    strm.close();
    return true;
}