bool PxMDecoder::readHeader()
{
    bool result = false;

    if( !m_buf.empty() )
    {
        if( !m_strm.open(m_buf) )
            return false;
    }
    else if( !m_strm.open( m_filename ))
        return false;

    CV_TRY
    {
        int code = m_strm.getByte();
        if( code != 'P' )
            CV_THROW (RBS_BAD_HEADER);

        code = m_strm.getByte();
        switch( code )
        {
        case '1': case '4': m_bpp = 1; break;
        case '2': case '5': m_bpp = 8; break;
        case '3': case '6': m_bpp = 24; break;
        default: CV_THROW (RBS_BAD_HEADER);
        }

        m_binary = code >= '4';
        m_type = m_bpp > 8 ? CV_8UC3 : CV_8UC1;

        m_width = ReadNumber(m_strm);
        m_height = ReadNumber(m_strm);

        m_maxval = m_bpp == 1 ? 1 : ReadNumber(m_strm);
        if( m_maxval > 65535 )
            CV_THROW (RBS_BAD_HEADER);

        //if( m_maxval > 255 ) m_binary = false; nonsense
        if( m_maxval > 255 )
            m_type = CV_MAKETYPE(CV_16U, CV_MAT_CN(m_type));

        if( m_width > 0 && m_height > 0 && m_maxval > 0 && m_maxval < (1 << 16))
        {
            m_offset = m_strm.getPos();
            result = true;
        }
    }
    CV_CATCH (cv::Exception, e)
    {
        CV_UNUSED(e);
        CV_RETHROW();
    }
    CV_CATCH_ALL
    {
        std::cerr << "PXM::readHeader(): unknown C++ exception" << std::endl << std::flush;
        CV_RETHROW();
    }

    if( !result )
    {
        m_offset = -1;
        m_width = m_height = -1;
        m_strm.close();
    }
    return result;
}