cpTags(TIFF* in, TIFF* out)
{
    struct cpTag *p;
    for (p = tags; p < &tags[NTAGS]; p++)
    {
        if( p->tag == TIFFTAG_GROUP3OPTIONS )
        {
            uint16 compression;
            if( !TIFFGetField(in, TIFFTAG_COMPRESSION, &compression) ||
                    compression != COMPRESSION_CCITTFAX3 )
                continue;
        }
        if( p->tag == TIFFTAG_GROUP4OPTIONS )
        {
            uint16 compression;
            if( !TIFFGetField(in, TIFFTAG_COMPRESSION, &compression) ||
                    compression != COMPRESSION_CCITTFAX4 )
                continue;
        }
        cpTag(in, out, p->tag, p->count, p->type);
    }
}