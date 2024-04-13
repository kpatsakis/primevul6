static int pix_fmt_match(enum AVPixelFormat pix_fmt, int components,
                         int bpc, uint32_t log2_chroma_wh, int pal8)
{
    int match = 1;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(pix_fmt);

    if (desc->nb_components != components) {
        return 0;
    }

    switch (components) {
    case 4:
        match = match && desc->comp[3].depth_minus1 + 1 >= bpc &&
                         (log2_chroma_wh >> 14 & 3) == 0 &&
                         (log2_chroma_wh >> 12 & 3) == 0;
    case 3:
        match = match && desc->comp[2].depth_minus1 + 1 >= bpc &&
                         (log2_chroma_wh >> 10 & 3) == desc->log2_chroma_w &&
                         (log2_chroma_wh >>  8 & 3) == desc->log2_chroma_h;
    case 2:
        match = match && desc->comp[1].depth_minus1 + 1 >= bpc &&
                         (log2_chroma_wh >>  6 & 3) == desc->log2_chroma_w &&
                         (log2_chroma_wh >>  4 & 3) == desc->log2_chroma_h;

    case 1:
        match = match && desc->comp[0].depth_minus1 + 1 >= bpc &&
                         (log2_chroma_wh >>  2 & 3) == 0 &&
                         (log2_chroma_wh       & 3) == 0 &&
                         (desc->flags & AV_PIX_FMT_FLAG_PAL) == pal8 * AV_PIX_FMT_FLAG_PAL;
    }
    return match;
}