void readFromBuf(png_structp pngPtr, png_bytep data, png_size_t length) {
    BufPtr* bufPtr = (BufPtr*)png_get_io_ptr(pngPtr);

    memcpy((char*)data, bufPtr->ptr, length);
    bufPtr->ptr += length;
    bufPtr->len -= length;
}