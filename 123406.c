AP4_HvccAtom::GetChromaFormatName(AP4_UI08 chroma_format)
{
    switch (chroma_format) {
        case AP4_HEVC_CHROMA_FORMAT_MONOCHROME: return "Monochrome";
        case AP4_HEVC_CHROMA_FORMAT_420:        return "4:2:0";
        case AP4_HEVC_CHROMA_FORMAT_422:        return "4:2:2";
        case AP4_HEVC_CHROMA_FORMAT_444:        return "4:4:4";
    }

    return NULL;
}