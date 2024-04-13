int validAddress(struct READER *reader, uint64_t address) {
	return address > 0 && address < reader->superblock.end_of_file_address;
}