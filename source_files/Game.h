#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <array>

// ! Game state enumeration for turn-based game logic
enum class GameState 
{
    WAITING,          // ! Waiting for player input (initial state, after move)
    PIECE_SELECTED,   // ! A piece has been selected, waiting for field click
    MOVE_VALIDATED    // ! Move validated, animation/processing phase
};

// ! Player turn enumeration
enum class PlayerTurn 
{
    WHITE,
    BLACK
};

// ! Represents a single black field on the checkerboard (32 total)
// ! Each field is clickable and can hold a piece
struct BoardField
{
    int fieldId;              // ! Unique ID: 0-31 (row-major order on black fields)
    glm::vec3 position;       // ! 3D world position (center of the field)
    int row;                  // ! Row on 8x8 board (0-7)
    int col;                  // ! Column on 8x8 board (0-7)
    int piece;                // ! Piece ID: -1=empty, 1=white, 2=black

    BoardField() 
        : fieldId(-1), position(0.0f), row(0), col(0), piece(-1) {}
    
    BoardField(int id, const glm::vec3& pos, int r, int c)
        : fieldId(id), position(pos), row(r), col(c), piece(-1) {}
};

// ! Main game class managing checkers logic, board state, and game flow
class Game
{
public:
    // * Constructor - initializes board and game state
    Game();
    
    // * Destructor
    ~Game() = default;

    // ! Initializes the board with 32 black fields and starting piece positions
    void initializeBoard();

    // ! Main update loop (currently minimal, for future game logic)
    void update(float deltaTime);

    // ! Render function for future board visualization (debug/UI)
    void render();

    // ! Handles player clicking on a board field
    // ? @param fieldId - ID of the clicked field (0-31)
    void handleFieldClick(int fieldId);

    // ! Checks if a move from 'from' to 'to' is valid according to checkers rules
    // ? @param from - source field ID, @param to - destination field ID
    // ? @return true if move is valid, false otherwise
    bool validateMove(int from, int to);

    // ! Returns capture moves for a given field: pairs of (landingFieldId, capturedFieldId)
    std::vector<std::pair<int,int>> getCaptureMoves(int fieldId) const;

    // ! Gets all legal moves from a given field
    // ? @param fieldId - starting field ID
    // ? @return vector of valid destination field IDs
    std::vector<int> getLegalMoves(int fieldId) const;

    // ! Executes a validated move: updates piece position and switches turn
    // ? @param from - source field ID, @param to - destination field ID
    // ? @param capturedField - if move is a capture, pass captured field ID (-1 otherwise)
    void executeMove(int from, int to, int capturedField = -1);

    // ! Checks if the current player has any available capture anywhere on the board
    bool hasAnyCaptureForCurrentPlayer() const;

    // ! Returns the current player's turn
    PlayerTurn getCurrentTurn() const { return currentTurn; }

    // ! Convert board (row,col) to fieldId publicly
    int coordsToFieldId(int row, int col) const { return boardCoordsToFieldId(row, col); }

    // ! Returns the current game state
    GameState getCurrentState() const { return currentState; }

    // ! Returns ID of selected field (-1 if none selected)
    int getSelectedFieldId() const { return selectedFieldId; }

    // ! Returns constant reference to the board fields
    const std::array<BoardField, 32>& getBoardFields() const { return blackFields; }

    // ! Returns reference to mutable board fields (for debugging/modification)
    std::array<BoardField, 32>& getBoardFieldsMutable() { return blackFields; }

private:
    // * Board fields: 32 clickable black squares in 8x8 checkerboard
    std::array<BoardField, 32> blackFields;

    // * Current game state (WAITING, PIECE_SELECTED, etc.)
    GameState currentState;

    // * Current player's turn (WHITE or BLACK)
    PlayerTurn currentTurn;

    // * ID of currently selected field (-1 if none)
    int selectedFieldId;

    // * Stores the ID of the piece/field we started moving from
    int selectedPieceFieldId;

    // ! Helper: Converts (row, col) board coordinates to linear field ID (0-31)
    // ? @param row - row on 8x8 board (0-7), @param col - column on 8x8 board (0-7)
    // ? @return field ID (0-31) or -1 if white field
    int boardCoordsToFieldId(int row, int col) const;

    // ! Helper: Converts field ID (0-31) to (row, col) board coordinates
    // ? @param fieldId - linear ID on black fields
    // ? @return pair of (row, col)
    std::pair<int, int> fieldIdToBoardCoords(int fieldId) const;

    // ! Helper: Checks if adjacent diagonals from a field are valid and unoccupied
    // ? @param fieldId - starting field
    // ? @return vector of valid diagonal field IDs
    std::vector<int> getSimpleMoves(int fieldId) const;

    // ! Helper: Processes move state transition when field is clicked
    void processMoveTransition(int clickedFieldId);

    // ! Helper: Switches the current player turn
    void switchTurn();
};
