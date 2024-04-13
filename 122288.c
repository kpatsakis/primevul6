QPDFWriter::writeEncryptionDictionary()
{
    this->encryption_dict_objid = openObject(this->encryption_dict_objid);
    writeString("<<");
    for (std::map<std::string, std::string>::iterator iter =
	     this->encryption_dictionary.begin();
	 iter != this->encryption_dictionary.end(); ++iter)
    {
	writeString(" ");
	writeString((*iter).first);
	writeString(" ");
	writeString((*iter).second);
    }
    writeString(" >>");
    closeObject(this->encryption_dict_objid);
}