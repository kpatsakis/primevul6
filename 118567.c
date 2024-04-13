void PngImg::ReadInfo_(PngReadStruct& rs) {
    png_read_info(rs.pngPtr, rs.infoPtr);
    info_.width = png_get_image_width(rs.pngPtr, rs.infoPtr);
    info_.height = png_get_image_height(rs.pngPtr, rs.infoPtr);
    info_.bit_depth = png_get_bit_depth(rs.pngPtr, rs.infoPtr);
    info_.color_type = png_get_color_type(rs.pngPtr, rs.infoPtr);
    info_.interlace_type = png_get_interlace_type(rs.pngPtr, rs.infoPtr);
    info_.compression_type = png_get_compression_type(rs.pngPtr, rs.infoPtr);
    info_.filter_type = png_get_filter_type(rs.pngPtr, rs.infoPtr);
    info_.rowbytes = png_get_rowbytes(rs.pngPtr, rs.infoPtr);
    info_.pxlsize = info_.rowbytes / info_.width;
}