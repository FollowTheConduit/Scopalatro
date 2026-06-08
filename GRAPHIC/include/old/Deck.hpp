#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include <random>
#include <chrono>
#include "Card.hpp"

class Deck {
public:
    Deck() = default;

    void addCard(std::unique_ptr<Card> card);
    void shuffle();
    std::unique_ptr<Card> draw();
    bool isEmpty() const;
    size_t size() const;
    void addCards(std::vector<std::unique_ptr<Card>>& pile);

private:
    std::vector<std::unique_ptr<Card>> cards;
};