static int cbs_av1_assemble_fragment(CodedBitstreamContext *ctx,
                                     CodedBitstreamFragment *frag)
{
    size_t size, pos;
    int i;

    size = 0;
    for (i = 0; i < frag->nb_units; i++)
        size += frag->units[i].data_size;

    frag->data_ref = av_buffer_alloc(size + AV_INPUT_BUFFER_PADDING_SIZE);
    if (!frag->data_ref)
        return AVERROR(ENOMEM);
    frag->data = frag->data_ref->data;
    memset(frag->data + size, 0, AV_INPUT_BUFFER_PADDING_SIZE);

    pos = 0;
    for (i = 0; i < frag->nb_units; i++) {
        memcpy(frag->data + pos, frag->units[i].data,
               frag->units[i].data_size);
        pos += frag->units[i].data_size;
    }
    av_assert0(pos == size);
    frag->data_size = size;

    return 0;
}