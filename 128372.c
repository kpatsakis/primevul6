    std::string upper(const std::string& str)
    {
        std::string result;
        transform(str.begin(), str.end(), std::back_inserter(result), toupper);
        return result;
    }