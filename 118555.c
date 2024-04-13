bool PngImg::Write(const string& file) {
    auto fileClose = [](FILE* fp){ if(fp) fclose(fp); };
    unique_ptr<FILE, decltype(fileClose)> fp(fopen(file.c_str(), "wb"), fileClose);
    if(!fp) {
        error_ = "Can't open file for writing";
        return false;
    }

    PngWriteStruct pws;
    if(!pws.Valid()) {
        error_ = "Can't create png structs";
        return false;
    }

    if(setjmp(png_jmpbuf(pws.pngPtr))) {
        error_ = "Can't write file";
        return false;
    }

    png_init_io(pws.pngPtr, fp.get());
    png_set_IHDR(pws.pngPtr, pws.infoPtr,
        info_.width,
        info_.height,
        info_.bit_depth,
        info_.color_type,
        info_.interlace_type,
        info_.compression_type,
        info_.filter_type
    );
    png_set_rows(pws.pngPtr, pws.infoPtr, &rowPtrs_[0]);
    png_write_png(pws.pngPtr, pws.infoPtr, PNG_TRANSFORM_IDENTITY, NULL);

    return true;
}