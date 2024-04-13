        CmdGetNonce() :
            Command("getnonce"),
            _randMutex("getnonce"),
            _random(SecureRandom::create()) {
        }