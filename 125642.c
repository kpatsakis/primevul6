static void cbs_av1_free_metadata(AV1RawMetadata *md)
{
    switch (md->metadata_type) {
    case AV1_METADATA_TYPE_ITUT_T35:
        av_buffer_unref(&md->metadata.itut_t35.payload_ref);
        break;
    }
}