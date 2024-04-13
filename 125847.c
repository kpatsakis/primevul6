void MP4_BoxDumpStructure( stream_t *s, MP4_Box_t *p_box )
{
    MP4_BoxDumpStructure_Internal( s, p_box, 0 );
}