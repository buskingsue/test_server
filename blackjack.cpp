#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <fstream>

using namespace std;

class Card {
public:
    enum Suit {
        DIAMOND,
        HEART,
        CLOVER,
        SPADE
    };
private:
    int value;
    string character;
    Suit suit;
    const string suitStrs[4] = {
        "Diamond",
        "Heart",
        "Clover",
        "Spade"
    };
    const string suitName(Suit s) {
        int n = static_cast<int>(s);
        return suitStrs[n];
    }

public:
    Card() = delete;
    Card(Suit suit, int value, string character)
        : suit(suit),
          value(value),
          character(character) {
    }
    int getValue() {
        return value;
    }
    string turn() {
        ostringstream cardDesc;
        cardDesc << "(" << suitName(suit)
                 << ": " << character
                 << ")";
        return cardDesc.str();
    }
};

inline Card::Suit& operator++(Card::Suit &r, int) {
    const int i = static_cast<int>(r);
    r = static_cast<Card::Suit>((i+1));
    return r;
}

class Deck {
private:
    vector<Card*> cardShoe;
public:
    void shuffle() {
        for (Card::Suit s=Card::Suit::DIAMOND; s <= Card::Suit::SPADE; s++) {
            cardShoe.push_back(new Card(s, 1, "A"));
            for (int n=2; n < 10; n++) {
                cardShoe.push_back(new Card(s, n, to_string(n)));
            }
            cardShoe.push_back(new Card(s, 10, "10"));
            cardShoe.push_back(new Card(s, 10, "J"));
            cardShoe.push_back(new Card(s, 10, "Q"));
            cardShoe.push_back(new Card(s, 10, "K"));
        }
        random_shuffle(cardShoe.begin(), cardShoe.end());
    };
    Card* pickCard() {
        auto firstIt = cardShoe.begin();
        Card *pickedCard = *firstIt;
        cardShoe.erase(firstIt);
        return pickedCard;
    };
};

class Player {
private:
    vector<Card*> privateDeck;
    string name;
    bool stay = false;
    bool busted = false;
    int sum = 0;
public:
    Player(int playerNumber=-1) {
        if (playerNumber < 0) {
            name = "Dealer";
        } else {
            name = "Player" + to_string(playerNumber);
        }
    }
    virtual ~Player() {
    }
    void giveCard(Card *card) {
        privateDeck.push_back(card);
    }
    string& getName() {
        return name;
    }
    void showCards() {
        cout << "[" << name << "] ";
        for (auto c : privateDeck) {
            cout << c->turn() << " ";
        }
        cout << "=> sum: " << getSum();
        if (getSum() > 21) {
            cout << " BUSTED!!";
        }
        cout << endl;
    }
    void putStay() {
        // Put player's status to stay.
        stay = true;
    }
    bool isStaying() {
        return stay;
    }
    void updateStatus() {
        sum = 0;
        for (auto c : privateDeck) {
            sum += c->getValue();
        }
        if (sum > 21) {
            busted = true;
        }
    }
    int getSum() {
        sum = 0;
        for (auto c : privateDeck) {
            sum += c->getValue();
        }
        return sum;
    }
    bool isBusted() {
        return busted;
    }
    /**
       Return true for heat.
     */
    virtual bool heatOrStay() {
        string userDec;
        cout << " (" << getSum() << ") stay?:";
        getline(cin, userDec);
        if (userDec == "s") {
            putStay();
            return false;
        }
        return true;
    }
};


class Dealer: public Player {
private:
    bool gameEnd = false;
    Player *winner = nullptr;
    void updateGameStatus(Deck &deck, vector<Player*>& players) {
        // All players are in staying.
        for (auto p : players) {
            if (!p->isStaying() && !p->isBusted()) {
                gameEnd = false;
            } else {
                gameEnd = true;
            }
        }

        if (gameEnd) {
            int curBest = 21;
            // Who's the winner?
            for (auto p : players) {
                if (p->isBusted()) {
                    continue;
                }
                int dist = 21 - p->getSum();
                if (dist < curBest) {
                    curBest = dist;
                    winner = p;
                }
            }
        }
    }
public:
    Dealer() : Player(-1){
    }
    void ready(Deck &deck, vector<Player*>& players) {
        // End the game when no players.
        if (players.size() < 2) {
            gameEnd = true;
            return;
        }
        deck.shuffle();
        for (auto p : players) {
            // Two cards per person.
            for (int i=0; i < 2; i++) {
                auto card = deck.pickCard();
                if (card != nullptr) {
                    p->giveCard(card);
                } else {
                    cerr << "ERROR: invalid card picked" << endl;
                    exit(-1);
                }
            }
        }
    }
    void proceed(Deck &deck, vector<Player*>& players) {
        checkAll(players);
        cout << endl;
        for (auto p: players) {
            // Skip player who is in stay.
            cout << p->getName();
            if (p->isBusted()) {
                cout << " is busted." << endl;
                continue;
            } else if (p->isStaying()) {
                cout << " is staying." << endl;
                continue;
            } else {
                if (p->heatOrStay()) {
                    // Heat
                    auto card = deck.pickCard();
                    p->giveCard(card);
                }
                p->updateStatus();
                p->showCards();
            }
            updateGameStatus(deck, players);
        }
    }
    void checkAll(vector<Player*>& players) {
        for (auto p : players) {
            p->showCards();
        }       
    }   
    bool isEnd() {
        return gameEnd;
    }
    Player *getWinner() {
        return winner;
    }
    bool heatOrStay() override {
        // Dealer stays over 17, heats otherwise.
        int sum = getSum();
        if (sum > 17) {
            cout << " (" << sum << ") stay" << endl;
            putStay();
            return false;
        }
        cout << " (" << sum << ") heat" << endl;
        return true;
    }
};

class Game {
private:
    vector<Player*> players;
    Dealer *dealer = nullptr;
    Deck deck;
    int numPlayers = 0;
public:
    // TODO: Block copy constructor and assign operator.
    Game(int numPlayers)
        : numPlayers(numPlayers) {
        for (int i=0; i < numPlayers; i++) {
            players.push_back(new Player(i+1));
        }
        dealer = new Dealer();
        players.push_back(dealer);      
    }
    ~Game() {
        // Delete the dealer at the end of the vector.
        delete dealer;
        if (players.size() > 2) {
            players.erase(players.end());
            for (auto p: players) {
                delete p;
            }
        }
    }

    void ready() {
        dealer->ready(deck, players);
    };

    bool isEnd() {
        return dealer->isEnd();
    }

    void proceed() {
        dealer->proceed(deck, players);
    }

    void announceWinner() {
        if (! dealer->getWinner()) {
            cout << "No winer" << endl;
            return;
        }
        cout << endl << "-=[Result]=-" << endl;
        dealer->checkAll(players);
        cout << "Winner is " << dealer->getWinner()->getName() << endl;
    }
};



int main(void) {
    // Print out logo.
    ifstream logoFile("logo.txt");
    try {
        if (logoFile.is_open()) {
            string line;
            while (getline(logoFile, line)) {
                cout << line << endl;
            }
        }
    } catch (...) {
        // Failed  to open logo.
    }

    string strNumPlayers;
    int numPlayers = 0;
    cout << "Number of players: ";
    getline(cin, strNumPlayers);
    numPlayers = atoi(strNumPlayers.c_str());
    
    Game *game = new Game(numPlayers);
    game->ready();

    while(! game->isEnd()) {
        game->proceed();
    }
    game->announceWinner();
    delete game;
    
    return 0;
}

