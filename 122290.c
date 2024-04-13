QPDFWriter::setEncryptionParametersInternal(
    int V, int R, int key_len, long P,
    std::string const& O, std::string const& U,
    std::string const& OE, std::string const& UE, std::string const& Perms,
    std::string const& id1, std::string const& user_password,
    std::string const& encryption_key)
{
    this->encryption_V = V;
    this->encryption_R = R;
    encryption_dictionary["/Filter"] = "/Standard";
    encryption_dictionary["/V"] = QUtil::int_to_string(V);
    encryption_dictionary["/Length"] = QUtil::int_to_string(key_len * 8);
    encryption_dictionary["/R"] = QUtil::int_to_string(R);
    encryption_dictionary["/P"] = QUtil::int_to_string(P);
    encryption_dictionary["/O"] = QPDF_String(O).unparse(true);
    encryption_dictionary["/U"] = QPDF_String(U).unparse(true);
    if (V >= 5)
    {
        encryption_dictionary["/OE"] = QPDF_String(OE).unparse(true);
        encryption_dictionary["/UE"] = QPDF_String(UE).unparse(true);
        encryption_dictionary["/Perms"] = QPDF_String(Perms).unparse(true);
    }
    if (R >= 6)
    {
        setMinimumPDFVersion("1.7", 8);
    }
    else if (R == 5)
    {
        setMinimumPDFVersion("1.7", 3);
    }
    else if (R == 4)
    {
        setMinimumPDFVersion(this->encrypt_use_aes ? "1.6" : "1.5");
    }
    else if (R == 3)
    {
        setMinimumPDFVersion("1.4");
    }
    else
    {
        setMinimumPDFVersion("1.3");
    }

    if ((R >= 4) && (! encrypt_metadata))
    {
	encryption_dictionary["/EncryptMetadata"] = "false";
    }
    if ((V == 4) || (V == 5))
    {
	// The spec says the value for the crypt filter key can be
	// anything, and xpdf seems to agree.  However, Adobe Reader
	// won't open our files unless we use /StdCF.
	encryption_dictionary["/StmF"] = "/StdCF";
	encryption_dictionary["/StrF"] = "/StdCF";
	std::string method = (this->encrypt_use_aes
                              ? ((V < 5) ? "/AESV2" : "/AESV3")
                              : "/V2");
        // The PDF spec says the /Length key is optional, but the PDF
        // previewer on some versions of MacOS won't open encrypted
        // files without it.
	encryption_dictionary["/CF"] =
	    "<< /StdCF << /AuthEvent /DocOpen /CFM " + method +
            " /Length " + std::string((V < 5) ? "16" : "32") + " >> >>";
    }

    this->encrypted = true;
    QPDF::EncryptionData encryption_data(
	V, R, key_len, P, O, U, OE, UE, Perms, id1, this->encrypt_metadata);
    if (V < 5)
    {
        this->encryption_key = QPDF::compute_encryption_key(
            user_password, encryption_data);
    }
    else
    {
        this->encryption_key = encryption_key;
    }
}