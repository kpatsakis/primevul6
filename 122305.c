QPDFWriter::setR5EncryptionParameters(
    char const* user_password, char const* owner_password,
    bool allow_accessibility, bool allow_extract,
    qpdf_r3_print_e print, qpdf_r3_modify_e modify,
    bool encrypt_metadata)
{
    std::set<int> clear;
    interpretR3EncryptionParameters(
	clear, user_password, owner_password,
	allow_accessibility, allow_extract, print, modify);
    this->encrypt_use_aes = true;
    this->encrypt_metadata = encrypt_metadata;
    setEncryptionParameters(user_password, owner_password, 5, 5, 32, clear);
}