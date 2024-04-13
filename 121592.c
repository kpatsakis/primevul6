std::string postfix_uniq(const std::string &filename, const char * ext) {
    std::string::size_type where =filename.find_last_of("./\\");
    if (where == std::string::npos || filename[where] != '.') {
        return uniq_filename(filename + ext);
    }
    return uniq_filename(filename.substr(0, where) + ext);
}