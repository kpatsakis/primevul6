std::string uniq_filename(std::string filename) {
    FILE * fp = fopen(filename.c_str(), "rb");
    while (fp != NULL) {
        fclose(fp);
        filename += "_";
        fp = fopen(filename.c_str(), "rb");
    }
    return filename;
}