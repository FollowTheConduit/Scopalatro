#pragma once

#include <vector>
#include <memory>
#include "Entity.hpp"
#include "Card.hpp"
#include "Deck.hpp"
#include "Enemy.hpp"

class Game {
public:
    Game();
    void run();

private:
    void drawPhase();
    void displayState() const;
    int playerSelectCard();
    void resolvePlay(int cardIdx);
    void enemyTurnPhase();
    void endTurn();
    void calculateScoring();

    Entity player;
    Enemy enemy;
    Deck deck;
    std::vector<std::unique_ptr<Card>> hand;
    std::vector<std::unique_ptr<Card>> table;
    std::vector<std::unique_ptr<Card>> scorePile;
    int scopaCount;
    int turn;
};