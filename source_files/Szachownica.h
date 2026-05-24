#pragma once

#include "model.h"
#include "shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

// Klasa `Szachownica` ładuje jeden model podstawy i model pola (czarny)
// i przygotowuje 32 transformacje dla czarnych pól.
class Szachownica
{
public:
    // Konstruktor ładuje modele z podanych ścieżek i generuje transformacje.
    // blackOffset: domyślnie (0.5,0.5,0.5) — podstawowe przesunięcie dla czarnych pól.
    Szachownica(const glm::vec3 &blackOffset = glm::vec3(0.5f, 0.0f, 0.5f));

    // Rysuje podstawę i wszystkie czarne pola przy użyciu dostarczonego shader'a.
    void Draw(Shader &shader);
    // Draw pieces according to current game state (positions come from Game::blackFields)
    void DrawPieces(const class Game &game, Shader &shader);

private:
    Model blackModel;
    std::vector<glm::mat4> blackTransforms;
    std::vector<glm::mat4> whiteTransforms;
    // pawn models are drawn from Game state
    Model pawnModel;
    Model pawnBlueModel;
};
