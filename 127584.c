		Generation(const string &serverInstanceDir, unsigned int number) {
			path = serverInstanceDir + "/generation-" + toString(number);
			this->number = number;
			owner = false;
		}