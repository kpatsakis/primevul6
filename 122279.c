QPDFWriter::setEncryptionParameters(
    char const* user_password, char const* owner_password,
    int V, int R, int key_len, std::set<int>& bits_to_clear)
{
    // PDF specification refers to bits with the low bit numbered 1.
    // We have to convert this into a bit field.

    // Specification always requires bits 1 and 2 to be cleared.
    bits_to_clear.insert(1);
    bits_to_clear.insert(2);

    if (R > 3)
    {
        // Bit 10 is deprecated and should always be set.  This used
        // to mean accessibility.  There is no way to disable
        // accessibility with R > 3.
        bits_to_clear.erase(10);
    }

    int P = 0;
    // Create the complement of P, then invert.
    for (std::set<int>::iterator iter = bits_to_clear.begin();
	 iter != bits_to_clear.end(); ++iter)
    {
	P |= (1 << ((*iter) - 1));
    }
    P = ~P;

    generateID();
    std::string O;
    std::string U;
    std::string OE;
    std::string UE;
    std::string Perms;
    std::string encryption_key;
    if (V < 5)
    {
        QPDF::compute_encryption_O_U(
            user_password, owner_password, V, R, key_len, P,
            this->encrypt_metadata, this->id1, O, U);
    }
    else
    {
        QPDF::compute_encryption_parameters_V5(
            user_password, owner_password, V, R, key_len, P,
            this->encrypt_metadata, this->id1,
            encryption_key, O, U, OE, UE, Perms);
    }
    setEncryptionParametersInternal(
	V, R, key_len, P, O, U, OE, UE, Perms,
        this->id1, user_password, encryption_key);
}