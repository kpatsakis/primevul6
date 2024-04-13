init_gs_simple_font(gs_font_base *pfont, const double bbox[4],
                    const gs_uid *puid)
{
    pfont->FontBBox.p.x = bbox[0];
    pfont->FontBBox.p.y = bbox[1];
    pfont->FontBBox.q.x = bbox[2];
    pfont->FontBBox.q.y = bbox[3];
    pfont->UID = *puid;
}