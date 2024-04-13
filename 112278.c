TIFFReadRawStripOrTile2(TIFF* tif, uint32 strip_or_tile, int is_strip,
                        tmsize_t size, const char* module)
{
        TIFFDirectory *td = &tif->tif_dir;

        assert( !isMapped(tif) );
        assert((tif->tif_flags&TIFF_NOREADRAW)==0);

        if (!SeekOK(tif, td->td_stripoffset[strip_or_tile])) {
            if( is_strip )
            {
                TIFFErrorExt(tif->tif_clientdata, module,
                    "Seek error at scanline %lu, strip %lu",
                    (unsigned long) tif->tif_row,
                    (unsigned long) strip_or_tile);
            }
            else
            {
                TIFFErrorExt(tif->tif_clientdata, module,
                    "Seek error at row %lu, col %lu, tile %lu",
                    (unsigned long) tif->tif_row,
                    (unsigned long) tif->tif_col,
                    (unsigned long) strip_or_tile);
            }
            return ((tmsize_t)(-1));
        }

        if( !TIFFReadAndRealloc( tif, size, 0, is_strip,
                                 strip_or_tile, module ) )
        {
            return ((tmsize_t)(-1));
        }

        return (size);
}