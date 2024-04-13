        nonce64 getNextNonce() {
            SimpleMutex::scoped_lock lk(_randMutex);
            return _random->nextInt64();
        }