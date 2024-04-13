QPDFWriter::interpretR3EncryptionParameters(
    std::set<int>& clear,
    char const* user_password, char const* owner_password,
    bool allow_accessibility, bool allow_extract,
    qpdf_r3_print_e print, qpdf_r3_modify_e modify)
{
    // Acrobat 5 security options:

    // Checkboxes:
    //   Enable Content Access for the Visually Impaired
    //   Allow Content Copying and Extraction

    // Allowed changes menu:
    //   None
    //   Only Document Assembly
    //   Only Form Field Fill-in or Signing
    //   Comment Authoring, Form Field Fill-in or Signing
    //   General Editing, Comment and Form Field Authoring

    // Allowed printing menu:
    //   None
    //   Low Resolution
    //   Full printing

    if (! allow_accessibility)
    {
	clear.insert(10);
    }
    if (! allow_extract)
    {
	clear.insert(5);
    }

    // Note: these switch statements all "fall through" (no break
    // statements).  Each option clears successively more access bits.
    switch (print)
    {
      case qpdf_r3p_none:
	clear.insert(3);	// any printing

      case qpdf_r3p_low:
	clear.insert(12);	// high resolution printing

      case qpdf_r3p_full:
	break;

	// no default so gcc warns for missing cases
    }

    switch (modify)
    {
      case qpdf_r3m_none:
	clear.insert(11);	// document assembly

      case qpdf_r3m_assembly:
	clear.insert(9);	// filling in form fields

      case qpdf_r3m_form:
	clear.insert(6);	// modify annotations, fill in form fields

      case qpdf_r3m_annotate:
	clear.insert(4);	// other modifications

      case qpdf_r3m_all:
	break;

	// no default so gcc warns for missing cases
    }
}