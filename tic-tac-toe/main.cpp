#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

class Cell {
public:
    int x, y;
    char symbol;
};

class Game {
public:
    Game() {
        player = 'O';
        turn = 1;
        complete = false;
        paused = false;
    }

    char player;
    int turn;
    int complete;
    bool paused;

    int checkWin(std::vector<std::vector<Cell>> grid);
};

std::vector<std::vector<Cell>> setup() {
    std::vector<std::vector<Cell>> grid(11, std::vector<Cell>(5));
    for (int i = 0; i < 11; i++) {
        for (int j = 0; j < 5; j++) {
            switch(i) {
                case 1:
                    switch(j) {
                        case 0:
                            grid.at(i).at(j).symbol = '1';
                            break;
                        case 2:
                            grid.at(i).at(j).symbol = '4';
                            break;
                        case 4:
                            grid.at(i).at(j).symbol = '7';
                            break;
                        case 1:
                        case 3:
                            grid.at(i).at(j).symbol = '-';
                            break;
                    }
                    break;
                case 5:
                    switch(j) {
                        case 0:
                            grid.at(i).at(j).symbol = '2';
                            break;
                        case 2:
                            grid.at(i).at(j).symbol = '5';
                            break;
                        case 4:
                            grid.at(i).at(j).symbol = '8';
                            break;
                        case 1:
                        case 3:
                            grid.at(i).at(j).symbol = '-';
                            break;
                    }
                    break;
                case 9:
                    switch(j) {
                        case 0:
                            grid.at(i).at(j).symbol = '3';
                            break;
                        case 2:
                            grid.at(i).at(j).symbol = '6';
                            break;
                        case 4:
                            grid.at(i).at(j).symbol = '9';
                            break;
                        case 1:
                        case 3:
                            grid.at(i).at(j).symbol = '-';
                            break;
                    }
                    break;
                case 0:
                case 2:
                case 4:
                case 6:
                case 8:
                case 10:
                    switch(j) {
                        case 0:
                        case 2:
                        case 4:
                            grid.at(i).at(j).symbol = ' ';
                            break;
                        case 1:
                        case 3:
                            grid.at(i).at(j).symbol = '-';
                            break;
                    }
                    break;
                case 3:
                case 7:
                    switch(j) {
                        case 0:
                        case 2:
                        case 4:
                            grid.at(i).at(j).symbol = '|';
                            break;
                        case 1:
                        case 3:
                            grid.at(i).at(j).symbol = '+';
                            break;
                    }
                    break;
            }
        }
    }
    return grid;
}

std::vector<int> translate(int i) {
    switch (i) {
        case 1: return {1, 0};
        case 2: return {5, 0};
        case 3: return {9, 0};
        case 4: return {1, 2};
        case 5: return {5, 2};
        case 6: return {9, 2};
        case 7: return {1, 4};
        case 8: return {5, 4};
        case 9: return {9, 4};
        default: return {0, 0};
    }
}

int Game::checkWin(std::vector<std::vector<Cell>> grid) {
    std::vector<std::vector<int>> winCombos = {
        {1, 2, 3}, {1, 4, 7}, {1, 5, 9},
        {3, 5, 7}, {3, 6, 9},
        {7, 8, 9},
        {4, 5, 6},
        {2, 5, 8},
    };

    if (turn > 9) {
        return 1;
    }

    for (int i = 0; i < 8; i++) {
        if (grid.at(translate(winCombos.at(i).at(0)).at(0)).at(translate(winCombos.at(i).at(0)).at(1)).symbol == grid.at(translate(winCombos.at(i).at(1)).at(0)).at(translate(winCombos.at(i).at(1)).at(1)).symbol && grid.at(translate(winCombos.at(i).at(1)).at(0)).at(translate(winCombos.at(i).at(1)).at(1)).symbol == grid.at(translate(winCombos.at(i).at(2)).at(0)).at(translate(winCombos.at(i).at(2)).at(1)).symbol) {
            return 2;
        }
    }

    return 0;
}

bool validate(std::vector<std::vector<Cell>> grid, std::vector<int> cells) {
    if (grid.at(cells.at(0)).at(cells.at(1)).symbol != 'X' && grid.at(cells.at(0)).at(cells.at(1)).symbol != 'O') {
        return true;
    }
    return false;
}

void run(std::vector<std::vector<Cell>> &grid, Game &game) {
    game.paused = false;
    int input;
    std::cin >> input;
    
    std::vector<int> cells = translate(input);
    if (cells.at(0) == 0) {
        std::cout << "Invalid input." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        return; // TODO: Zorgen dat alles hetzelfde is als voor de turn, zodat deze opnieuw kan worden gedaan.
    }

    if (!validate(grid, cells)) {
        std::cout << "That box is already filled." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        return;
    }

    grid.at(cells.at(0)).at(cells.at(1)).symbol = game.player;
    game.turn++;

    game.complete = game.checkWin(grid);
}

void render(std::vector<std::vector<Cell>> grid, Game &game) {
    system("clear");
    for (int j = 0; j < 5; j++) {
        for (int i = 0; i < 11; i++) {
            switch(grid.at(i).at(j).symbol) {
                case 'X':
                    std::cout << "\033[31mX\033[0m";
                    break;
                case 'O':
                    std::cout << "\033[34mO\033[0m";
                    break;
                default:
                    std::cout << grid.at(i).at(j).symbol;
                    break;
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    switch(game.complete) {
        case 0:
            game.player = (game.player == 'X') ? 'O' : 'X';
            std::cout << "Player " << game.player << ", choose your next move: ";
            break;
        case 1:
            std::cout << "It's a draw!" << std::endl;
            break;
        case 2:
            std::cout << "Congratulations player " << game.player << ", you won the game!" << std::endl;
            break;
    }
}

int main() {
    std::vector<std::vector<Cell>> grid = setup();
    Game game;

    render(grid, game);
    while (game.complete == 0) {
        run(grid, game);
        render(grid, game);
    }

}