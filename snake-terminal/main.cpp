#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctime>

#define ROWS 10
#define COLUMNS 20
#define FPS 5

class Cell {
public:
    Cell() {
        occupied = false;
    }
    int x, y;
    bool occupied;
};

class Snake {
public:
    Snake(std::vector<std::vector<Cell>> &grid) {
        positions.resize(3);
        positions.at(0).x = 2;
        positions.at(1).x = 3;
        positions.at(2).x = 4;
        positions.at(0).y = 3;
        positions.at(1).y = 3;
        positions.at(2).y = 3;
        grid.at(positions.at(0).x).at(positions.at(0).y).occupied = true;
        grid.at(positions.at(1).x).at(positions.at(1).y).occupied = true;
        grid.at(positions.at(2).x).at(positions.at(2).y).occupied = true;
        head = positions.at(2);
        direction = 'E';
        eaten = false;
        gameOver = false;
    }

    std::vector<Cell> positions;
    Cell head;
    char direction;
    bool eaten;
    bool gameOver;

    void move(std::vector<std::vector<Cell>> &grid);
    bool has_collided(std::vector<std::vector<Cell>> grid);
};

class Fruit {
public:
    Cell position;
    
    void resetFruit(std::vector<std::vector<Cell>> grid);
};

void Snake::move(std::vector<std::vector<Cell>> &grid) {
    if (eaten) {
        positions.push_back(positions.at(positions.size() - 1));
        eaten = false;
    } else {
        grid.at(positions.at(0).x).at(positions.at(0).y).occupied = false;
        for (int i = 0; i < positions.size() - 1; i++) {
            positions.at(i).x = positions.at(i + 1).x;
            positions.at(i).y = positions.at(i + 1).y;
        }
    }

    switch(direction) {
        case 'N':
            positions.at(positions.size() - 1).y++;
            break;
        case 'E':
            positions.at(positions.size() - 1).x++;
            break;
        case 'S':
            positions.at(positions.size() - 1).y--;
            break;
        case 'W':
            positions.at(positions.size() - 1).x--;
            break;
    }

    head = positions.at(positions.size() - 1);

    if (has_collided(grid)) {
        gameOver = true;
        return;
    }

    grid.at(head.x).at(head.y).occupied = true;

    return;
}

bool Snake::has_collided(std::vector<std::vector<Cell>> grid) {
    if (head.x < 1 || head.x > COLUMNS - 2 || head.y < 1 || head.y > ROWS - 2 || grid.at(head.x).at(head.y).occupied) {
        return true;
    }
    return false;
}

void Fruit::resetFruit(std::vector<std::vector<Cell>> grid) {
    bool success = false;

    while(!success) {
        int x = rand() % (COLUMNS - 2) + 1;
        int y = rand() % (ROWS - 2) + 1;

        if (!grid.at(x).at(y).occupied) {
            success = true;
            position = grid.at(x).at(y);
        }
    }
}

void enableRawMode(termios &oldSettings) {
    termios newSettings;

    tcgetattr(STDIN_FILENO, &oldSettings);

    newSettings = oldSettings;
    newSettings.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);
}

void disableRawMode(const termios &oldSettings) {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldSettings);
}

void enableNonBlockingInput() {
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}

void disableNonBlockingInput() {
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    flags &= ~O_NONBLOCK;
    fcntl(STDIN_FILENO, F_SETFL, flags);
}

std::vector<std::vector<Cell>> setup() {
    std::vector<std::vector<Cell>> grid(COLUMNS, std::vector<Cell>(ROWS));
    
    for (int i = 0; i < COLUMNS; i++) {
        for (int j = 0; j < ROWS; j++) {
            grid.at(i).at(j).x = i;
            grid.at(i).at(j).y = j;
        }
    }

    return grid;
}

char input(Snake snake) {
    char input;
    ssize_t result = read(STDIN_FILENO, &input, 1);

    if (result == 1) {
        switch(input) {
            case 'w':
                if (snake.direction != 'S') {
                    return 'N';
                }
                break;
            case 'a':
                if (snake.direction != 'E') {
                    return 'W';
                }
                break;
            case 's':
                if (snake.direction != 'N') {
                    return 'S';
                }
                break;
            case 'd':
                if (snake.direction != 'W') {
                    return 'E';
                }
                break;
            default:
                return '0';
        }
    }
    return 0;
}

void update(char &direction, char input, Snake &snake, Fruit &fruit, std::vector<std::vector<Cell>> &grid) {
    if (input != 0) {
        snake.direction = input;
        direction = input;
    } else {
        snake.direction = direction;
    }

    snake.move(grid);
    
    if (snake.head.x == fruit.position.x && snake.head.y == fruit.position.y) {
        snake.eaten = true;
        fruit.resetFruit(grid);
    }
}

void render(Snake snake, Fruit fruit, std::vector<std::vector<Cell>> grid) {
    system("clear");
    for (int j = ROWS - 1; j >= 0; j--) {
        for (int i = 0; i < COLUMNS; i++) {
            if (i < 1 || i > COLUMNS - 2 || j < 1 || j > ROWS - 2) {
                std::cout << "#";
            } else if (grid.at(i).at(j).occupied) {
                std::cout << "O";
            } else if (fruit.position.x == i && fruit.position.y == j) {
                std::cout << "@";
            } else {
                std::cout << " ";
            }
        }
        std::cout << std::endl;
    }
}

int main() {
    srand(time(0));   
    termios oldSettings;
    enableRawMode(oldSettings);
    enableNonBlockingInput();

    std::vector<std::vector<Cell>> grid = setup();
    Snake snake(grid);
    Fruit fruit;
    fruit.resetFruit(grid);
    char direction = 'E';

    while(!snake.gameOver) {
        auto frameStart = std::chrono::high_resolution_clock::now();

        update(direction, input(snake), snake, fruit, grid);
        render(snake, fruit, grid);

        auto frameEnd = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart).count();
        int sleep = 1000 / FPS - elapsed;
        
        std::cout << "FPS: " << 1000 / (sleep + elapsed) << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep));        
    }
    disableRawMode(oldSettings);
    disableNonBlockingInput();
}