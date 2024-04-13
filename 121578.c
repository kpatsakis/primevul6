void standard_eof(abytewriter* hdrw, abytewriter* huffw) {
    // get pointer for header data & size
    hdrdata  = hdrw->getptr_aligned();
    hdrs     = hdrw->getpos();
    // get pointer for huffman data & size
    huffdata = huffw->getptr_aligned();
    hufs     = huffw->getpos();
}