QPDFWriter::setR3EncryptionParameters(
    char const* user_password, char const* owner_password,
    bool allow_accessibility, bool allow_extract,
    qpdf_r3_print_e print, qpdf_r3_modify_e modify)
{
    std::set<int> clear;
    interpretR3EncryptionParameters(
	clear, user_password, owner_password,
	allow_accessibility, allow_extract, print, modify);
    setEncryptionParameters(user_password, owner_password, 2, 3, 16, clear);
}