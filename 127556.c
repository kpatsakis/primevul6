parseModeString(const StaticString &mode) {
	mode_t modeBits = 0;
	vector<string> clauses;
	vector<string>::iterator it;
	
	split(mode, ',', clauses);
	for (it = clauses.begin(); it != clauses.end(); it++) {
		const string &clause = *it;
		
		if (clause.empty()) {
			continue;
		} else if (clause.size() < 2 || (clause[0] != '+' && clause[1] != '=')) {
			throw InvalidModeStringException("Invalid mode clause specification '" + clause + "'");
		}
		
		switch (clause[0]) {
		case 'u':
			for (string::size_type i = 2; i < clause.size(); i++) {
				switch (clause[i]) {
				case 'r':
					modeBits |= S_IRUSR;
					break;
				case 'w':
					modeBits |= S_IWUSR;
					break;
				case 'x':
					modeBits |= S_IXUSR;
					break;
				case 's':
					modeBits |= S_ISUID;
					break;
				default:
					throw InvalidModeStringException("Invalid permission '" +
						string(1, clause[i]) +
						"' in mode clause specification '" +
						clause + "'");
				}
			}
			break;
		case 'g':
			for (string::size_type i = 2; i < clause.size(); i++) {
				switch (clause[i]) {
				case 'r':
					modeBits |= S_IRGRP;
					break;
				case 'w':
					modeBits |= S_IWGRP;
					break;
				case 'x':
					modeBits |= S_IXGRP;
					break;
				case 's':
					modeBits |= S_ISGID;
					break;
				default:
					throw InvalidModeStringException("Invalid permission '" +
						string(1, clause[i]) +
						"' in mode clause specification '" +
						clause + "'");
				}
			}
			break;
		case 'o':
			for (string::size_type i = 2; i < clause.size(); i++) {
				switch (clause[i]) {
				case 'r':
					modeBits |= S_IROTH;
					break;
				case 'w':
					modeBits |= S_IWOTH;
					break;
				case 'x':
					modeBits |= S_IXOTH;
					break;
				default:
					throw InvalidModeStringException("Invalid permission '" +
						string(1, clause[i]) +
						"' in mode clause specification '" +
						clause + "'");
				}
			}
			break;
		case '+':
			for (string::size_type i = 1; i < clause.size(); i++) {
				switch (clause[i]) {
				case 't':
					modeBits |= S_ISVTX;
					break;
				default:
					throw InvalidModeStringException("Invalid permission '" +
						string(1, clause[i]) +
						"' in mode clause specification '" +
						clause + "'");
				}
			}
			break;
		default:
			throw InvalidModeStringException("Invalid owner '" + string(1, clause[0]) +
				"' in mode clause specification '" + clause + "'");
		}
	}
	
	return modeBits;
}