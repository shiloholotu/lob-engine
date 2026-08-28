#include "MatchingEngine.h"

std::vector<Trade> MatchingEngine::submit(Order /*order*/) {
    return {};
}

bool MatchingEngine::cancel(OrderId id) {
    return book_.cancel(id);
}