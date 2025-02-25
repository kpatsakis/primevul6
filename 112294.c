TIFFFillStripPartial( TIFF *tif, int strip, tmsize_t read_ahead, int restart )
{
	static const char module[] = "TIFFFillStripPartial";
	register TIFFDirectory *td = &tif->tif_dir;
        tmsize_t unused_data;
        uint64 read_offset;
        tmsize_t to_read;
        tmsize_t read_ahead_mod;
        /* tmsize_t bytecountm; */
        
        if (!_TIFFFillStriles( tif ) || !tif->tif_dir.td_stripbytecount)
            return 0;
        
        /*
         * Expand raw data buffer, if needed, to hold data
         * strip coming from file (perhaps should set upper
         * bound on the size of a buffer we'll use?).
         */

        /* bytecountm=(tmsize_t) td->td_stripbytecount[strip]; */

        /* Not completely sure where the * 2 comes from, but probably for */
        /* an exponentional growth strategy of tif_rawdatasize */
        if( read_ahead < TIFF_TMSIZE_T_MAX / 2 )
                read_ahead_mod = read_ahead * 2;
        else
                read_ahead_mod = read_ahead;
        if (read_ahead_mod > tif->tif_rawdatasize) {
                assert( restart );
                
                tif->tif_curstrip = NOSTRIP;
                if ((tif->tif_flags & TIFF_MYBUFFER) == 0) {
                        TIFFErrorExt(tif->tif_clientdata, module,
                                     "Data buffer too small to hold part of strip %lu",
                                     (unsigned long) strip);
                        return (0);
                }
        }

        if( restart )
        {
                tif->tif_rawdataloaded = 0;
                tif->tif_rawdataoff = 0;
        }

        /*
        ** If we are reading more data, move any unused data to the
        ** start of the buffer.
        */
        if( tif->tif_rawdataloaded > 0 )
                unused_data = tif->tif_rawdataloaded - (tif->tif_rawcp - tif->tif_rawdata);
        else
                unused_data = 0;
        
        if( unused_data > 0 )
        {
		assert((tif->tif_flags&TIFF_BUFFERMMAP)==0);
                memmove( tif->tif_rawdata, tif->tif_rawcp, unused_data );
        }

        /*
        ** Seek to the point in the file where more data should be read.
        */
        read_offset = td->td_stripoffset[strip]
                + tif->tif_rawdataoff + tif->tif_rawdataloaded;

        if (!SeekOK(tif, read_offset)) {
                TIFFErrorExt(tif->tif_clientdata, module,
                             "Seek error at scanline %lu, strip %lu",
                             (unsigned long) tif->tif_row, (unsigned long) strip);
                return 0;
        }

        /*
        ** How much do we want to read?
        */
        if( read_ahead_mod > tif->tif_rawdatasize )
                to_read = read_ahead_mod - unused_data;
        else
                to_read = tif->tif_rawdatasize - unused_data;
        if( (uint64) to_read > td->td_stripbytecount[strip] 
            - tif->tif_rawdataoff - tif->tif_rawdataloaded )
        {
                to_read = (tmsize_t) td->td_stripbytecount[strip]
                        - tif->tif_rawdataoff - tif->tif_rawdataloaded;
        }

	assert((tif->tif_flags&TIFF_BUFFERMMAP)==0);
        if( !TIFFReadAndRealloc( tif, to_read, unused_data,
                                 1, /* is_strip */
                                 0, /* strip_or_tile */
                                 module) )
        {
                return 0;
        }

        tif->tif_rawdataoff = tif->tif_rawdataoff + tif->tif_rawdataloaded - unused_data ;
        tif->tif_rawdataloaded = unused_data + to_read;

        tif->tif_rawcc = tif->tif_rawdataloaded;
        tif->tif_rawcp = tif->tif_rawdata;
                        
        if (!isFillOrder(tif, td->td_fillorder) &&
            (tif->tif_flags & TIFF_NOBITREV) == 0) {
		assert((tif->tif_flags&TIFF_BUFFERMMAP)==0);
                TIFFReverseBits(tif->tif_rawdata + unused_data, to_read );
	}

        /*
        ** When starting a strip from the beginning we need to
        ** restart the decoder.
        */
        if( restart )
        {

#ifdef JPEG_SUPPORT
            /* A bit messy since breaks the codec abstraction. Ultimately */
            /* there should be a function pointer for that, but it seems */
            /* only JPEG is affected. */
            /* For JPEG, if there are multiple scans (can generally be known */
            /* with the  read_ahead used), we need to read the whole strip */
            if( tif->tif_dir.td_compression==COMPRESSION_JPEG &&
                (uint64)tif->tif_rawcc < td->td_stripbytecount[strip] )
            {
                if( TIFFJPEGIsFullStripRequired(tif) )
                {
                    return TIFFFillStrip(tif, strip);
                }
            }
#endif

            return TIFFStartStrip(tif, strip);
        }
        else
        {
                return 1;
        }
}