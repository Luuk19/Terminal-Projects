#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
#include <ctime>

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

std::vector<std::string> getWords(const std::string &filename) {
    std::vector<std::string> words;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return words;
    }

    std::string word;
    while (file >> word) {
        words.push_back(word);
    }

    file.close();
    return words;
}

std::string pickWord(const std::vector<std::string>& words) {
    return words.at(rand() % words.size());
}

std::string getDrawing(int level) {
    std::string stages[] = {
        "\n\n\n\n\n________",
        "\n |\n |\n |\n |\n_|______",
        "________\n |\n |\n |\n |\n_|______",
        "________\n |/\n |\n |\n |\n_|______",
        "________\n |/  |\n |\n |\n |\n_|______",
        "________\n |/  |\n |   O\n |\n |\n_|______",
        "________\n |/  |\n |   O\n |   |\n |\n_|______",
        "________\n |/  |\n |   O\n |   |\n |  /\n_|______",
        "________\n |/  |\n |   O\n |   |\n |  / \\\n_|______",
        "________\n |/  |\n |  \\O\n |   |\n |  / \\\n_|______",
        "________\n |/  |\n |  \\O/\n |   |\n |  / \\\n_|______"
    };

    return stages[level];
}

std::vector<char> getWrongGuesses(const std::vector<char>& guesses, std::string word) {
    std::vector<char> wrong;
    
    for (int i = 0; i < guesses.size(); i++) {
        for (int j = 0; j < word.length(); j++) {
            if (word.at(j) == guesses.at(i)) {
                break;
            } else if (j == word.length() - 1) {
                wrong.push_back(guesses.at(i));
            }
        }
    }

    return wrong;
}

std::vector<bool> processGuesses(const std::vector<char>& guesses, std::string word) {
    std::vector<bool> guessed(word.length(), false);
    for (int i = 0; i < guesses.size(); i++) {
        for (int j = 0; j < word.length(); j++) {
            if (guesses.at(i) == word.at(j)) {
                guessed.at(j) = true;
            }
        }
    }

    return guessed;
}

char render(int& level, std::string word, const std::vector<bool>& guessed, const std::vector<char>& guesses) {
    clearScreen();
    std::cout << getDrawing(level) << std::endl << std::endl;

    for (int i = 0; i < guessed.size(); i++) {
        char letter = (guessed.at(i)) ? word.at(i) : '_';
        std::cout << letter << " ";
    }
    std::cout << std::endl << std::endl;

    std::cout << "Foute letters: ";
    std::vector<char> wrong = getWrongGuesses(guesses, word);
    for (int i = 0; i < wrong.size(); i++) {
        std::cout << wrong.at(i);
    }
    std::cout << std::endl << std::endl;
    
    char guess;
    std::cout << "Voer een letter in: ";
    std::cin >> guess;

    for (int i = 0; i < word.length(); i++) {
        if (word.at(i) == guess) {
            return guess;
        }
    }
    
    level++;
    return guess;
}

int main() {
    srand(time(0));
    std::vector<std::string> words = getWords("woorden.txt");
    std::string word = pickWord(words);
    int level = 0;
    std::vector<char> guesses(0);
    std::vector<bool> guessed(word.length(), false);
    bool win = false;

    while (level < 11 && !win) {
        guesses.push_back(render(level, word, guessed, guesses));
        guessed = processGuesses(guesses, word);

        win = true;
        for (int i = 0; i < guessed.size(); i++) {
            if (!guessed.at(i)) {
                win = false;
            }
        }
    }

    if (win) {
        std::cout << "Gewonnen!" << std::endl;
    } else {
        std::cout << "Game over, het woord was: " << word << std::endl;
    }
    return 0;
}