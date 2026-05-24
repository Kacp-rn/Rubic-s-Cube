#include "Szachownica.h"

#include <glm/gtc/matrix_transform.hpp>
#include "Game.h"

// Ścieżki do modeli (relatywne względem katalogu projektu)
static const std::string BLACK_CUBE_PATH = "source_files/models/szachownica_cube_black/Szachownica_cube_black.obj";
static const std::string PAWN_RED_PATH = "source_files/models/pionek_czerwony/Szachownica_cube_black.obj";
static const std::string PAWN_BLUE_PATH = "source_files/models/pionek niebieski/Szachownica_cube_black.obj";

Szachownica::Szachownica(const glm::vec3 &blackOffset)
    : blackModel(BLACK_CUBE_PATH), pawnModel(PAWN_RED_PATH), pawnBlueModel(PAWN_BLUE_PATH)
{
    // Generuj 64 kostki na siatce 8x8, po 8 w każdym rzędzie
    for (int row = 0; row < 8; ++row)
    {
        for (int col = 0; col < 8; ++col)
        {
            glm::mat4 model = glm::mat4(1.0f);
            glm::vec3 pos = glm::vec3(static_cast<float>(col) + blackOffset.x,
                                      blackOffset.y,
                                      static_cast<float>(row) + blackOffset.z);
            model = glm::translate(model, pos);

            if ((row + col) % 2 == 0)
                blackTransforms.push_back(model);
            else
                whiteTransforms.push_back(model);
            // place red pawn on start rows (row 0 and 1) on black squares
        }
    }
}

void Szachownica::Draw(Shader &shader)
{
    shader.use();
    // Rysuj 64 kostki naprzemiennie — oba używają tego samego modelu ale z różnym tintem
    // Ustaw tint dla jednego koloru pól (ciemniejsze)
    shader.setBool("useTint", true);
    shader.setVec3("tintColor", 0.2f, 0.2f, 0.2f);
    for (const auto &m : blackTransforms)
    {
        shader.setMat4("model", m);
        blackModel.Draw(shader);
    }

    // Ustaw tint dla drugiego koloru pól (jaśniejsze)
    shader.setVec3("tintColor", 1.0f, 1.0f, 1.0f);
    for (const auto &m : whiteTransforms)
    {
        shader.setMat4("model", m);
        blackModel.Draw(shader);
    }

    shader.setBool("useTint", false);
}

// Draw pieces based on Game state
void Szachownica::DrawPieces(const Game &game, Shader &shader)
{
    shader.use();
    const auto &fields = game.getBoardFields();
    for (const auto &f : fields)
    {
        if (f.piece == -1) continue;
        glm::mat4 model = glm::mat4(1.0f);
        // Game positions are stored as centers at (col+0.5, 0, row+0.5)
        model = glm::translate(model, glm::vec3(f.position.x, 0.6f, f.position.z));
        // scale pieces down by factor of 3
        model = glm::scale(model, glm::vec3(1.0f / 3.0f));
        // set model matrix for shader before drawing the pawn
        shader.setMat4("model", model);
        if (f.piece == 2) // black / red pawns originally mapped as 2
        {
            shader.setBool("useTint", true);
            shader.setVec3("tintColor", 1.0f, 0.2f, 0.2f);
            pawnModel.Draw(shader);
            shader.setBool("useTint", false);
        }
        else if (f.piece == 1) // white / blue pawns mapped as 1
        {
            shader.setBool("useTint", true);
            shader.setVec3("tintColor", 0.2f, 0.4f, 1.0f);
            pawnBlueModel.Draw(shader);
            shader.setBool("useTint", false);
        }
    }
}
