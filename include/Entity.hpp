#pragma once
#include <iostream>
#include <map>
#include <string>

class Entity {
public:
    Entity(std::string name, int hp)
        : name(name), currentHp(hp), maxHp(hp), gold(0), block(0) {}
    
    void takeDamage(int amount) {
        if (block > 0) {
            int absorbed = std::min(block, amount);
            block -= absorbed;
            amount -= absorbed;
            std::cout << name << " blocked " << absorbed << " damage! ";
            if (block > 0) std::cout << "(Shield remaining: " << block << ") \n";
            else std::cout << "(Shield broke!) \n";
        }

        if (amount > 0) {
            currentHp -= amount;
            if (currentHp < 0) currentHp = 0;
            std::cout << name << " took " << amount << " damage (HP: " << currentHp 
<< "/" << maxHp << ")" << std::endl;
        } else {
            std::cout << std::endl;
        }
    }

    void gainBlock(int amount) {
        block += amount;
        std::cout << name << " gained " << amount << " Shield! (Total: " << block <<
")" << std::endl;
    }

    void heal(int amount) {
        currentHp += amount;
        if (currentHp > maxHp) currentHp = maxHp;
        std::cout << name << " healed for " << amount << " (HP: " << currentHp << "/" << maxHp << ")" << std::endl;
    }

    void addStatus(const std::string& status, int duration) {
        if (statuses.find(status) != statuses.end()) {
            statuses[status] += duration;
            std::cout << name << " is already " << status << ". Extended by " << duration << " turns." << std::endl;
        } else {
            statuses[status] = duration;
            std::cout << name << " is now " << status << " for " << duration << " turns." << std::endl;
        }
    }

    bool isAlive() const {
        return currentHp > 0;
    }

    std::string getName() const { return name; }

    void printStatus() const {
        std::cout << "[" << name << "] HP: " << currentHp << "/" << maxHp;
        if (block > 0) {
            std::cout << " | Shield: " << block;
        }
    
        if (!statuses.empty()) {
            std::cout << " | Statuses: ";
            for (const auto& pair : statuses) {
                std::cout << pair.first << " (" << pair.second << " turns) ";
            }
        }
        std::cout << std::endl;
    }

    void updateStatuses() {
        auto it = statuses.begin();
        while (it != statuses.end()) {
            it->second--;
            if (it->second <= 0) {
                std::cout << name << " is no longer " << it->first << "." << std::endl;
                it = statuses.erase(it);
            } else {
                ++it;
            }
        }
    }

    int getGold() const { return gold; }
        void addGold(int amount) { gold += amount; }

private:
    std::string name;
    int currentHp;
    int maxHp;
    int gold;
    int block;
    std::map<std::string, int> statuses;
};