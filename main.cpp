#include <SFML/Graphics.hpp>
#include <array>
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace std;

const int gameMapSize = 200;
const int cellSize = 4;
const int windowSize = gameMapSize * cellSize;

enum CellType {
    Air,
    Sand,
    Water,
    Rock
};

struct Cell {
    sf::Color color;
    CellType type;
    int directionX;
    int frictionRate;
    int dispersionRate;
    bool isFreeFalling;
};

bool isInMapBounds(int mx, int my, array<int, 2> xy){
    if (xy[0] >= 0 && xy[0] < mx && xy[1] >= 0 && xy[1] < my) return true;
    return false;
}

void cellularAutomata(array<array<Cell, gameMapSize>, gameMapSize>& gameMap){
    auto sandMove = [=, &gameMap](int sand_x, int sand_y, Cell& sand_obj) -> array<int, 2> {
        if (isInMapBounds(gameMapSize, gameMapSize, {sand_x, sand_y+1})){
            if(gameMap[sand_x][sand_y+1].type == Air) return {sand_x, sand_y+1};
            if(gameMap[sand_x][sand_y+1].type == Sand) gameMap[sand_x][sand_y+1].frictionRate = sand_obj.frictionRate;
        }
        if (sand_obj.isFreeFalling) sand_x += sand_obj.directionX * sand_obj.dispersionRate;
        sand_obj.isFreeFalling = false;
        sand_obj.frictionRate--;
        if(isInMapBounds(gameMapSize, gameMapSize, {sand_x+(sand_obj.dispersionRate*sand_obj.directionX), sand_y+1}) && sand_obj.frictionRate > 0){
            if (gameMap[sand_x+(sand_obj.dispersionRate*sand_obj.directionX)][sand_y+1].type == Air) return {sand_x+(sand_obj.dispersionRate*sand_obj.directionX), sand_y+1};
            if (gameMap[sand_x+(sand_obj.dispersionRate*sand_obj.directionX)][sand_y+1].type == Sand) gameMap[sand_x+(sand_obj.dispersionRate*sand_obj.directionX)][sand_y+1].frictionRate = sand_obj.frictionRate;
        }
        if(isInMapBounds(gameMapSize, gameMapSize, {sand_x+(sand_obj.dispersionRate*(-sand_obj.directionX)), sand_y+1}) && sand_obj.frictionRate > 0){
            if (gameMap[sand_x+(sand_obj.dispersionRate*(-sand_obj.directionX))][sand_y+1].type == Air) return {sand_x+(sand_obj.dispersionRate*(-sand_obj.directionX)), sand_y+1};
            if (gameMap[sand_x+(sand_obj.dispersionRate*(-sand_obj.directionX))][sand_y+1].type == Sand) gameMap[sand_x+(sand_obj.dispersionRate*(-sand_obj.directionX))][sand_y+1].frictionRate = sand_obj.frictionRate;
        }
        return {sand_x, sand_y};
    };
    for (int i = gameMapSize; i >= 0; i--) {
        for (int j = gameMapSize; j >= 0 ; j--) {
            if (gameMap[i][j].type == Sand){
                array<int, 2> new_cords = sandMove(i, j, gameMap[i][j]);
                auto temp = gameMap[new_cords[0]][new_cords[1]];
                gameMap[new_cords[0]][new_cords[1]] = gameMap[i][j];
                gameMap[i][j] = temp;
            }
        }
    }
}

void spawnCell(Cell& cell, CellType cellType) {
    if (cellType == Sand) {
        int red = 255;
        int green = 192 + rand() % 64;
        int blue = std::rand() % 128;
        cell.color = sf::Color(red, green, blue);
        cell.type = Sand;
        cell.directionX = std::rand() % 2 ? 1 : -1;
        cell.frictionRate = 1 + std::rand() % 8;
        cell.dispersionRate = 1 + std::rand() % 4;
        cell.isFreeFalling = true;
    }
    if (cellType == Water) {
        int red = std::rand() % 70;
        int green = std::rand() % 70;
        int blue = 255;
        cell.color = sf::Color(red, green, blue);
        cell.type = Water;
        cell.directionX = std::rand() % 2 ? 1 : -1;
        cell.dispersionRate = 1 + rand()%5;
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(windowSize, windowSize), "Game Map");
    window.setFramerateLimit(60);
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    array<array<Cell, gameMapSize>, gameMapSize> gameMap;

    // Inicjalizacja mapy
    for (int i = 0; i < gameMapSize; i++) {
        for (int j = 0; j < gameMapSize; j++) {
            gameMap[i][j].color = sf::Color::Black;
            gameMap[i][j].type = Air;
            gameMap[i][j].directionX = 0;
            gameMap[i][j].dispersionRate = 0;
            gameMap[i][j].isFreeFalling = false;
        }
    }

    // Zmieniamy tryb na sf::Quads i ustawiamy poprawną liczbę wierzchołków
    sf::VertexArray pixels(sf::Quads, gameMapSize * gameMapSize * 4);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                int local_x = mousePosition.x / cellSize;
                int local_y = mousePosition.y / cellSize;

                if (local_x >= 0 && local_x < gameMapSize && local_y >= 0 && local_y < gameMapSize) {
                    spawnCell(gameMap[local_x][local_y], Sand);
                }
            }
            if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
                sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                int local_x = mousePosition.x / cellSize;
                int local_y = mousePosition.y / cellSize;

                if (local_x >= 0 && local_x < gameMapSize && local_y >= 0 && local_y < gameMapSize) {
                    spawnCell(gameMap[local_x][local_y], Water);
                }
            }
        }

        window.clear();

        cellularAutomata(gameMap);

        // Aktualizacja sf::VertexArray na podstawie gameMap
        for (int i = 0; i < gameMapSize; i++) {
            for (int j = 0; j < gameMapSize; j++) {
                int index = (i + j * gameMapSize) * 4;

                // Ustawienie pozycji wierzchołków dla każdego quada (kwadratu)
                pixels[index].position = sf::Vector2f(i * cellSize, j * cellSize);
                pixels[index + 1].position = sf::Vector2f((i + 1) * cellSize, j * cellSize);
                pixels[index + 2].position = sf::Vector2f((i + 1) * cellSize, (j + 1) * cellSize);
                pixels[index + 3].position = sf::Vector2f(i * cellSize, (j + 1) * cellSize);

                // Ustawienie koloru wierzchołków na podstawie koloru komórki
                sf::Color cellColor = gameMap[i][j].color;
                pixels[index].color = cellColor;
                pixels[index + 1].color = cellColor;
                pixels[index + 2].color = cellColor;
                pixels[index + 3].color = cellColor;
            }
        }

        window.draw(pixels);
        window.display();
    }

    return 0;
}
