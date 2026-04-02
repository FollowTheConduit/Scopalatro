#include "DeckBuilder.hpp"
#include "EffectSystem.hpp"
#include <algorithm>

std::unique_ptr<Card> DeckBuilder::createScopaCard(Suit s, CardValue v) {
    std::string name = valueToString(v) + " of " + suitToString(s);
    return std::make_unique<Card>(name, "", s, v);
}

Deck DeckBuilder::buildScopaDeck() {
    Deck deck;

    const std::vector<Suit> suits = {
        Suit::Denari, Suit::Coppe, Suit::Spada, Suit::Bastoni
    };
    const std::vector<CardValue> values = {
        CardValue::Asso,    CardValue::Due,     CardValue::Tre,
        CardValue::Quattro, CardValue::Cinque,  CardValue::Sei,
        CardValue::Sette,   CardValue::Fante,   CardValue::Cavallo,
        CardValue::Re
    };

    for (Suit s : suits) {
        for (CardValue v : values) {
            auto card = createScopaCard(s, v);

            // TESTING ONLY (this is shit implementation)
            if (s == Suit::Spada && v == CardValue::Asso)
                card->addEffect(new DamageEffect(5, TargetType::Enemy));
            else if (s == Suit::Denari && v == CardValue::Sette)
                card->addEffect(new HealEffect(5, TargetType::Self));
            else if (v == CardValue::Re) {
                if (s == Suit::Denari) {
                    card->addEffect(new DamageEffect(5, TargetType::Enemy));
                    card->addEffect(new BlockEffect(5, TargetType::Self));
                } else {
                    card->addEffect(new DamageEffect(10, TargetType::Enemy));
                }
            }
            else if (s == Suit::Coppe && v == CardValue::Asso) {
                card->addEffect(new BlockEffect(5, TargetType::Self));
            }
            else if (s == Suit::Denari && v == CardValue::Cavallo) {
                card->addEffect(new BlockEffect(4, TargetType::Self));
                card->addEffect(new HealEffect(2, TargetType::Self));
            }
            else if (s == Suit::Coppe && v == CardValue::Cavallo) {
                card->addEffect(new DamageEffect(5, TargetType::AllEnemies));
                card->addEffect(new HealEffect(5, TargetType::Self));
            } else if (s == Suit::Bastoni && v == CardValue::Fante) {
                card->addEffect(new DamageEffect(3, TargetType::Enemy));
                card->addEffect(new ApplyStatusEffect("Poisoned", 2, TargetType::Enemy));
            }
                
            deck.addCard(std::move(card));
        }
    }
    
    return deck;
}

std::vector<int> DeckBuilder::findCaptureIndices(const Card* playedCard,
                                                 const std::vector<std::unique_ptr<Card>>& table) {
    int targetVal = playedCard->getNumericValue();

    for (size_t i = 0; i < table.size(); ++i) {
        if (table[i]->getNumericValue() == targetVal) {
            return { static_cast<int>(i) };
        }
    }

    int n = table.size();
    for (int i = 1; i < (1 << n); ++i) {
        int currentSum = 0;
        std::vector<int> indices;
        for (int j = 0; j < n; ++j) {
            if ((i >> j) & 1) {
                currentSum += table[j]->getNumericValue();
                indices.push_back(j);
            }
        }
        if (currentSum == targetVal) {
            return indices;
        }
    }        

    return {};
}