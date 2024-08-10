#include <array>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <cstdlib> // Dla rand() i srand()
#include <ctime>   // Dla time()
using namespace std;

const int screenWidth = 400;
const int screenHeight = 320;
enum ParticleType{
    Air,
    Sand,
    Rock
};
struct Particle{
    sf::Color color;
    ParticleType type;
};

// Sprawdza, czy współrzędne znajdują się w granicach mapy
bool isInMapBounds(int x, int y) {
    return (x >= 0 && x < screenWidth && y >= 0 && y < screenHeight);
}

// Określa ruch piasku w dół (jeśli możliwe)
array<int, 2> sandMovement(array<array<Particle, screenHeight>, screenWidth>& gameMap, int particle_x, int particle_y) {
    if (isInMapBounds(particle_x, particle_y + 1) && gameMap[particle_x][particle_y + 1].type == Air) {
        return {particle_x, particle_y + 1}; // Piasek porusza się w dół
    }
    int random_number = (std::rand() % 2) * 2 - 1;
    if (isInMapBounds(particle_x - random_number, particle_y + 1) && gameMap[particle_x - random_number][particle_y + 1].type == Air){
            return {particle_x - random_number, particle_y + 1};
        }
    if (isInMapBounds(particle_x + random_number, particle_y + 1) && gameMap[particle_x + random_number][particle_y + 1].type == Air){
            return {particle_x + random_number, particle_y + 1};
        }
    return {particle_x, particle_y}; // Piasek nie porusza się
}

void cellularAutomata(array<array<Particle, screenHeight>, screenWidth>& gameMap){
    for (int x = 0; x < screenWidth; x++){
        for (int y = screenHeight - 1; y >= 0; y--){
            if (gameMap[x][y].type == Sand){
                array<int, 2> sandMove = sandMovement(gameMap, x, y);
                if (sandMove[0] != x || sandMove[1] != y) {
                    // Zamień miejscami piasek i powietrze
                    gameMap[sandMove[0]][sandMove[1]] = gameMap[x][y];
                    gameMap[x][y].type = Air;
                    gameMap[x][y].color = sf::Color::Black;
                }
            }
        }
    }
}

void mapGenerator(array<array<Particle, screenHeight>, screenWidth>& gameMap){
    //init clear map
    for (int x = 0; x < screenWidth; ++x) {
        for (int y = 0; y < screenHeight; ++y) {
            gameMap[x][y].type = Air;
            gameMap[x][y].color = sf::Color::Black; // czarny kolor dla powietrza
        }
    }
    for (int x = 0; x < screenWidth; ++x){
        gameMap[x][250].type = Rock;
        gameMap[x][250].color = sf::Color::Cyan;
    }
}

void onClickCreateSand(array<array<Particle, screenHeight>, screenWidth>& gameMap, sf::Vector2i clickPos){
    gameMap[clickPos.x][clickPos.y].type = Sand;
    gameMap[clickPos.x][clickPos.y].color = sf::Color::Yellow;
}

int main(){
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    // Utwórz okno SFML
    sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "Sand simulation");

    window.setFramerateLimit(60);

    array<array<Particle, screenHeight>, screenWidth> gameMap;

    mapGenerator(gameMap);

   // Główna pętla
    while (window.isOpen())
    {
        // Obsługa zdarzeń
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                // Pobieramy pozycję kursora myszki w oknie
                sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

                // Wywołujemy funkcję, gdy przycisk jest trzymany
                onClickCreateSand(gameMap, mousePosition);
            }
        }

        // Czyszczenie okna
        window.clear();

        cellularAutomata(gameMap);

        // Renderowanie pikseli z tablicy
        for (int x = 0; x < screenWidth; ++x) {
            for (int y = 0; y < screenHeight; ++y) {
                sf::RectangleShape pixel(sf::Vector2f(1, 1));
                pixel.setPosition(static_cast<float>(x), static_cast<float>(y));
                pixel.setFillColor(gameMap[x][y].color);
                window.draw(pixel);
            }
        }

        // Wyświetlenie zawartości okna
        window.display();
    }
    return 0;
}