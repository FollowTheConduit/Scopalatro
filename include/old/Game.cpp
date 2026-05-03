#include "Game.hpp"
#include "DeckBuilder.hpp"
#include "EffectSystem.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>

Game::Game()
    : player("Jaaj", 50),
      enemy("Badalisk", 30),
      scopaCount(0),
      turn(1)
{
    EnemyAction bite {"Bite", 3, 6, "", 0, 60, {}, false};
    EnemyAction venom {"Venom Spit", 2, 2, "Poisoned", 2, 25, {}, false};
    EnemyAction crush {"Crush", 10, 14, "", 0, 100, {}, true};
    EnemyAction windUp {"Wind Up", 0, 0, "", 0, 15, {crush}, false};

    enemy.pushActionTable({ bite, venom, windUp });
    enemy.rollIntent();

    deck = DeckBuilder::buildScopaDeck();

    std::cout << "Deck initialized with " << deck.size() << " cards." << std::endl;
    deck.shuffle();
    std::cout << "Deck shuffled.\n" << std::endl;

    std::cout << "Initializing Table..." << std::endl;
    for (int i = 0; i < 4; ++i) {
        if (!deck.isEmpty())
            table.push_back(deck.draw());
    }
}

void Game::run() {
    std::cout << "Cards Test\n" << std::endl;

    player.printStatus();
    enemy.printStatus();

    while (player.isAlive() && enemy.isAlive()) {
        std::cout << "\n--- Turn " << turn++ << " ---" << std::endl;

        player.updateStatuses();
        enemy.updateStatuses();
        drawPhase();

        if (hand.empty()) {
            std::cout << "Hand is empty and cannot draw." << std::endl;
            break;
        }
        
        displayState();
        int cardIdx = playerSelectCard();
        resolvePlay(cardIdx);

        if (!enemy.isAlive()) {
            std::cout << "\nEnemy Defeated!" << std::endl;
            calculateScoring();
            break;
        }

        enemyTurnPhase();

        if (!player.isAlive()) {
            std::cout << "\nYou have been defeated!" << std::endl;
            break;
        }
        endTurn();
    }
    if (player.isAlive() && enemy.isAlive()) {
        std::cout << "\nDeck depleted! Combat End" << std::endl;
        calculateScoring();
    }
}

void Game::drawPhase() {
    while (hand.size() < 3 && !deck.isEmpty()) {
        auto card = deck.draw();
        std::cout << "Drew: " << card->GetName() << std::endl;
        hand.push_back(std::move(card));
    }
}

void Game::displayState() const {
    std::cout << "\n[TABLE]: ";
    if (table.empty()) std::cout << "(Empty)";
    for (const auto& c : table)
        std::cout << "[" << c->GetName() << "(" << c->GetNumericValue() << ")] ";
    std::cout << std::endl;

    std::cout << "[HAND]: ";
    for (const auto& c : hand) {
        std::cout << "[" << c->GetName() << "(" << c->GetNumericValue() << ")";
        if (c->HasEffects()) std::cout << " | " << c->GetEffectsSummary();
        std::cout << "] ";
    }
    std::cout << "\n" << std::endl; 

    const auto& intent = enemy.getIntent();
    std::cout << "[" << enemy.getName() << " intends]: " << intent.name;
    if (intent.maxDamage > 0)
        std::cout << " (" << intent.minDamage << "-" << intent.maxDamage << "dmg)";
    if (!intent.status.empty())
        std::cout << " + " << intent.status << " (" << intent.statusDuration << "turns)";
    std::cout << "\n" << std::endl;
}

void Game::enemyTurnPhase() {
    enemy.executeIntent(player);
    enemy.rollIntent();
}

int Game::playerSelectCard() {
    int choice = -1;
    while (true) {
        std::cout << "Play which card? (1-" << hand.size() << "): ";
        if (std::cin >> choice && choice >= 1 && choice <= static_cast<int>(hand.size())) {
            return choice - 1;
        }
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        std::cout << "Invalid Input. ";
    }
}

void Game::resolvePlay(int cardIdx) {
    auto& cardToPlay = hand[cardIdx];

    std::vector<Entity*> targets = { &enemy };

    std::cout << "[PLAYER] " << player.getName()
              << " plays " << cardToPlay->GetName() << "!" << std::endl;

    cardToPlay->Play(player, targets);

    std::vector<int> capturedIndices = 
        DeckBuilder::findCaptureIndices(cardToPlay.get(), table);

    if (!capturedIndices.empty()) {
        std::cout << " -> CAPTURE! ";
        for (int idx : capturedIndices) {
            std::cout << table[idx]->GetName() << " ";
            scorePile.push_back(std::move(table[idx]));
        }
        std::cout << "!" << std::endl;

        std::sort(capturedIndices.rbegin(), capturedIndices.rend());
        for (int idx : capturedIndices)
            table.erase(table.begin() + idx);
        
        if (table.empty()) {
            ++scopaCount;
            std::cout << " SCOPA! Table cleared (" << scopaCount
                      << " sweep" << (scopaCount > 1 ? "s" : "") << " total) " << std::endl;
        }

        scorePile.push_back(std::move(hand[cardIdx]));
        hand.erase(hand.begin() + cardIdx);
    
    } else {
        std::cout << " -> No capture. Card added to table." << std::endl;
        table.push_back(std::move(hand[cardIdx]));
        hand.erase(hand.begin() + cardIdx);
    }
}

void Game::endTurn() {
    player.printStatus();
    enemy.printStatus();
    std::cout << "[End of Turn] Hand: " << hand.size()
              << " | Table: " << table.size()
              << " | Score: " << scorePile.size() / 2 << " pairs\n" << std::endl;
}

void Game::calculateScoring() {
    std::cout << " VICTORY ! \n";

    int numCards = 0;
    int numDenari = 0;
    bool hasSettebello = false;

    for (const auto& card : scorePile) {
        if (!card) continue;
        numCards++;
        if (card->GetSuit() == Suit::Denari) {
            numDenari++;
            if (card->GetNumericValue() == 7) {
                hasSettebello = true;
            }
        }
    }

    int goldGained = 0;

    bool wonCards = (numCards > 10);
    bool wonDenari = (numDenari > 3);

    std::cout << "- Most Cards (" << std::setw(2) << numCards << ")   : " << 
(wonCards ? "YES (+1 Gold)" : "NO") << "\n";
    std::cout << "- Most Denari (" << std::setw(2) << numDenari << ")  : " << 
(wonDenari ? "YES (+1 Gold)" : "NO") << "\n";
    std::cout << "- Settebello        : " << (hasSettebello ? "YES (+1 Gold)" : "NO") 
<< "\n";
    std::cout << "- Scopas (Sweeps)   : " << scopaCount << "   (+" << scopaCount 
<< " Gold)\n";
    std::cout << "--------------------------------------------\n";

    if (wonCards) goldGained++;
    if (wonDenari) goldGained++;
    if (hasSettebello) goldGained++;
    goldGained += scopaCount;

    player.addGold(goldGained);

    std::cout << "Total Earned: " << goldGained << " Gold\n";
    std::cout << "Current Gold: " << player.getGold() << " Gold\n";
    std::cout << "============================================\n";
} 