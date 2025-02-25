static enum TIFFReadDirEntryErr TIFFReadDirEntryDataAndRealloc(
                    TIFF* tif, uint64 offset, tmsize_t size, void** pdest)
{
#if SIZEOF_SIZE_T == 8
        tmsize_t threshold = INITIAL_THRESHOLD;
#endif
        tmsize_t already_read = 0;

        assert( !isMapped(tif) );

        if (!SeekOK(tif,offset))
                return(TIFFReadDirEntryErrIo);

        /* On 64 bit processes, read first a maximum of 1 MB, then 10 MB, etc */
        /* so as to avoid allocating too much memory in case the file is too */
        /* short. We could ask for the file size, but this might be */
        /* expensive with some I/O layers (think of reading a gzipped file) */
        /* Restrict to 64 bit processes, so as to avoid reallocs() */
        /* on 32 bit processes where virtual memory is scarce.  */
        while( already_read < size )
        {
            void* new_dest;
            tmsize_t bytes_read;
            tmsize_t to_read = size - already_read;
#if SIZEOF_SIZE_T == 8
            if( to_read >= threshold && threshold < MAX_THRESHOLD )
            {
                to_read = threshold;
                threshold *= THRESHOLD_MULTIPLIER;
            }
#endif

            new_dest = (uint8*) _TIFFrealloc(
                            *pdest, already_read + to_read);
            if( new_dest == NULL )
            {
                TIFFErrorExt(tif->tif_clientdata, tif->tif_name,
                            "Failed to allocate memory for %s "
                            "(%ld elements of %ld bytes each)",
                            "TIFFReadDirEntryArray",
                             (long) 1, (long) (already_read + to_read));
                return TIFFReadDirEntryErrAlloc;
            }
            *pdest = new_dest;

            bytes_read = TIFFReadFile(tif,
                (char*)*pdest + already_read, to_read);
            already_read += bytes_read;
            if (bytes_read != to_read) {
                return TIFFReadDirEntryErrIo;
            }
        }
        return TIFFReadDirEntryErrOk;
}