DocumentSource::GetNextResult DocumentSourceGroup::getNextStandard() {
    // Not spilled, and not streaming.
    if (_groups->empty())
        return GetNextResult::makeEOF();

    Document out = makeDocument(groupsIterator->first, groupsIterator->second, pExpCtx->needsMerge);

    if (++groupsIterator == _groups->end())
        dispose();

    return out;
}