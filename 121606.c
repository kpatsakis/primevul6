void early_eof(abytewriter* hdrw, abytewriter* huffw) {
    early_eof_encountered = true;
    standard_eof(hdrw, huffw);
}