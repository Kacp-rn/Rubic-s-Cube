#include "Game.h"
#include <iostream>
#include <cmath>

// * Constructor - initializes game state and board
Game::Game() 
    : currentState(GameState::WAITING),
      currentTurn(PlayerTurn::WHITE),
      selectedFieldId(-1),
      selectedPieceFieldId(-1)
{
    initializeBoard();
}

// ! Initializes the 32 black fields on an 8x8 checkerboard
// ! Fields are positioned at unit intervals starting from (0, 0, 0)
// ! Black fields are on alternating squares (like a real checkerboard)
void Game::initializeBoard()
{
    int fieldId = 0;

    // * Iterate through 8x8 board, only process black fields
    for (int row = 0; row < 8; ++row)
    {
        for (int col = 0; col < 8; ++col)
        {
            // * On a checkerboard, black fields are where (row + col) is even
            if ((row + col) % 2 == 0)
            {
                // * Position in 3D: x = col, y = 0 (on board plane), z = row
                // * Center of each field at unit offset
                glm::vec3 position(col + 0.5f, 0.0f, row + 0.5f);
                
                blackFields[fieldId] = BoardField(fieldId, position, row, col);
                fieldId++;
            }
        }
    }

    // * Initialize starting piece positions
    // * Red pieces (player2) on top rows 0-2
    for (int i = 0; i < 12; ++i)
    {
        blackFields[i].piece = 2; // ! 2 = red piece (player2)
    }

    // * Blue pieces (player1) on bottom rows 5-7
    for (int i = 20; i < 32; ++i)
    {
        blackFields[i].piece = 1; // ! 1 = blue piece (player1)
    }
}

// ! Update game logic per frame
void Game::update(float deltaTime)
{
    // * Currently minimal - can add animations, timed events, AI logic, etc.
    // * deltaTime parameter provided for time-based animations
}

// ! Render function for future debug/UI visualization
void Game::render()
{
    // * Placeholder for future rendering of board highlights, selected pieces, etc.
    // * Actual mesh rendering is handled in main.cpp with Model::Draw()
}

// ! Main entry point for handling field clicks from raycast selection
void Game::handleFieldClick(int fieldId)
{
    // * Validate field ID
    if (fieldId < 0 || fieldId >= 32)
    {
        std::cerr << "Invalid field ID: " << fieldId << std::endl;
        return;
    }

    // * Get the clicked field
    const BoardField& clickedField = blackFields[fieldId];

    // * State machine: handle click based on current state
    switch (currentState)
    {
    case GameState::WAITING:
        // * In WAITING state, only allow clicking own pieces
        if (clickedField.piece != -1)
        {
            int pieceOwner = clickedField.piece;
            int currentPlayer = (currentTurn == PlayerTurn::WHITE) ? 1 : 2;

            // * Only allow selecting your own piece
            if (pieceOwner == currentPlayer)
            {
                selectedFieldId = fieldId;
                selectedPieceFieldId = fieldId;
                currentState = GameState::PIECE_SELECTED;

                std::cout << "Piece selected from field " << fieldId 
                          << " (row: " << clickedField.row << ", col: " << clickedField.col << ")" 
                          << std::endl;
            }
            else
            {
                std::cerr << "Cannot select opponent's piece!" << std::endl;
            }
        }
        else
        {
            std::cerr << "No piece on field " << fieldId << std::endl;
        }
        break;

    case GameState::PIECE_SELECTED:
        // * In PIECE_SELECTED state, validate move or deselect
        processMoveTransition(fieldId);
        break;

    case GameState::MOVE_VALIDATED:
        // * After move validation, back to WAITING
        currentState = GameState::WAITING;
        selectedFieldId = -1;
        selectedPieceFieldId = -1;
        break;

    default:
        break;
    }
}

// ! Validates if a move from 'from' to 'to' is legal
bool Game::validateMove(int from, int to)
{
    // * Ensure both field IDs are valid
    if (from < 0 || from >= 32 || to < 0 || to >= 32)
        return false;

    // * Destination must be empty
    if (blackFields[to].piece != -1)
        return false;

    // * Capture is forced if any capture exists for current player
    bool captureAvailable = hasAnyCaptureForCurrentPlayer();
    auto captureMoves = getCaptureMoves(from);
    bool isCaptureMove = false;
    for (auto &p : captureMoves)
    {
        if (p.first == to)
        {
            isCaptureMove = true;
            break;
        }
    }

    if (captureAvailable)
    {
        return isCaptureMove;
    }

    // * Otherwise allow any legal non-capturing move or capture move for this piece
    std::vector<int> legalMoves = getLegalMoves(from);
    for (int move : legalMoves)
    {
        if (move == to)
            return true;
    }

    return false;
}


// ! Returns capture moves for a given field: pairs (landingFieldId, capturedFieldId)
std::vector<std::pair<int,int>> Game::getCaptureMoves(int fieldId) const
{
    std::vector<std::pair<int,int>> captures;
    if (fieldId < 0 || fieldId >= 32) return captures;

    auto [row, col] = fieldIdToBoardCoords(fieldId);
    if (row < 0) return captures;

    int player = blackFields[fieldId].piece;
    if (player == -1) return captures;

    int directions[2][2];
    if (player == 1)
    {
        directions[0][0] = -1; directions[0][1] = -1;
        directions[1][0] = -1; directions[1][1] =  1;
    }
    else
    {
        directions[0][0] =  1; directions[0][1] = -1;
        directions[1][0] =  1; directions[1][1] =  1;
    }

    for (int d = 0; d < 2; ++d)
    {
        int midRow = row + directions[d][0];
        int midCol = col + directions[d][1];
        int landRow = row + 2 * directions[d][0];
        int landCol = col + 2 * directions[d][1];

        if (midRow < 0 || midRow >= 8 || midCol < 0 || midCol >= 8) continue;
        if (landRow < 0 || landRow >= 8 || landCol < 0 || landCol >= 8) continue;

        int midField = boardCoordsToFieldId(midRow, midCol);
        int landField = boardCoordsToFieldId(landRow, landCol);
        if (midField < 0 || landField < 0) continue;

        if (blackFields[landField].piece != -1) continue; // landing must be empty

        int midPiece = blackFields[midField].piece;
        if (midPiece == -1) continue;

        // capture only opponent pieces
        if (midPiece != player)
        {
            captures.emplace_back(landField, midField);
        }
    }

    return captures;
}


bool Game::hasAnyCaptureForCurrentPlayer() const
{
    int currentPlayer = (currentTurn == PlayerTurn::WHITE) ? 1 : 2;
    for (int i = 0; i < 32; ++i)
    {
        if (blackFields[i].piece == currentPlayer)
        {
            auto caps = getCaptureMoves(i);
            if (!caps.empty()) return true;
        }
    }
    return false;
}

// ! Returns list of legal move destinations from a given field
std::vector<int> Game::getLegalMoves(int fieldId) const
{
    std::vector<int> moves;
    auto caps = getCaptureMoves(fieldId);
    bool captureAvailable = hasAnyCaptureForCurrentPlayer();

    if (captureAvailable)
    {
        if (!caps.empty())
        {
            for (auto &p : caps)
                moves.push_back(p.first);
        }
        return moves;
    }

    if (!caps.empty())
    {
        for (auto &p : caps)
            moves.push_back(p.first);
    }
    else
    {
        moves = getSimpleMoves(fieldId);
    }
    return moves;
}

// ! Executes a validated move
void Game::executeMove(int from, int to, int capturedField)
{
    // * Move piece from source to destination
    blackFields[to].piece = blackFields[from].piece;
    blackFields[from].piece = -1;

    // if capture, remove captured piece
    if (capturedField >= 0 && capturedField < 32)
    {
        std::cout << "Capture executed: removing piece at " << capturedField << std::endl;
        blackFields[capturedField].piece = -1;
    }

    // * Switch to other player
    switchTurn();

    // * Reset selection and return to waiting state
    selectedFieldId = -1;
    selectedPieceFieldId = -1;
    currentState = GameState::WAITING;

    std::cout << "Move executed: " << from << " -> " << to << std::endl;
}

// ! Converts field ID (0-31) to board coordinates (row, col)
std::pair<int, int> Game::fieldIdToBoardCoords(int fieldId) const
{
    if (fieldId < 0 || fieldId >= 32)
        return { -1, -1 };

    int currentId = 0;
    for (int row = 0; row < 8; ++row)
    {
        for (int col = 0; col < 8; ++col)
        {
            if ((row + col) % 2 == 0)
            {
                if (currentId == fieldId)
                    return { row, col };
                currentId++;
            }
        }
    }

    return { -1, -1 };
}

// ! Converts board coordinates (row, col) to field ID (0-31)
int Game::boardCoordsToFieldId(int row, int col) const
{
    // * Validate coordinates
    if (row < 0 || row >= 8 || col < 0 || col >= 8)
        return -1;

    // * Only valid for black fields (even sum)
    if ((row + col) % 2 != 0)
        return -1;

    // * Calculate field ID by counting black fields up to this position
    int fieldId = 0;
    for (int r = 0; r < 8; ++r)
    {
        for (int c = 0; c < 8; ++c)
        {
            if ((r + c) % 2 == 0)
            {
                if (r == row && c == col)
                    return fieldId;
                fieldId++;
            }
        }
    }

    return -1;
}

// ! Gets simple diagonal moves (non-capturing) from a field
std::vector<int> Game::getSimpleMoves(int fieldId) const
{
    std::vector<int> moves;
    
    if (fieldId < 0 || fieldId >= 32)
        return moves;

    auto [row, col] = fieldIdToBoardCoords(fieldId);
    if (row < 0 || col < 0)
        return moves;

    int player = blackFields[fieldId].piece;
    if (player == -1)
        return moves;

    // * Regular pieces may only move forward
    int directions[2][2];
    if (player == 1)
    {
        directions[0][0] = -1; directions[0][1] = -1;
        directions[1][0] = -1; directions[1][1] =  1;
    }
    else
    {
        directions[0][0] =  1; directions[0][1] = -1;
        directions[1][0] =  1; directions[1][1] =  1;
    }

    for (int d = 0; d < 2; ++d)
    {
        int newRow = row + directions[d][0];
        int newCol = col + directions[d][1];

        if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8)
        {
            int newFieldId = boardCoordsToFieldId(newRow, newCol);
            if (newFieldId >= 0 && blackFields[newFieldId].piece == -1)
            {
                moves.push_back(newFieldId);
            }
        }
    }

    return moves;
}

// ! Processes state transition when a field is clicked while a piece is selected
void Game::processMoveTransition(int clickedFieldId)
{
    // * Same field clicked: deselect
    if (clickedFieldId == selectedPieceFieldId)
    {
        selectedFieldId = -1;
        selectedPieceFieldId = -1;
        currentState = GameState::WAITING;
        std::cout << "Piece deselected" << std::endl;
        return;
    }

    // * Validate the move
    if (validateMove(selectedPieceFieldId, clickedFieldId))
    {
        // determine if this move is a capture
        int capturedField = -1;
        auto caps = getCaptureMoves(selectedPieceFieldId);
        for (auto &p : caps)
        {
            if (p.first == clickedFieldId)
            {
                capturedField = p.second;
                break;
            }
        }

        std::cout << "Valid move! Executing..." << std::endl;
        executeMove(selectedPieceFieldId, clickedFieldId, capturedField);
        // TODO: implement multi-jump chaining - for now end turn after single capture/move
        currentState = GameState::WAITING;
    }
    else
    {
        // * Invalid move: check if trying to select a different piece
        const BoardField& clickedField = blackFields[clickedFieldId];
        int currentPlayer = (currentTurn == PlayerTurn::WHITE) ? 1 : 2;

        if (clickedField.piece == currentPlayer)
        {
            // * Switch selection to new piece
            selectedPieceFieldId = clickedFieldId;
            selectedFieldId = clickedFieldId;
            std::cout << "Selection changed to field " << clickedFieldId << std::endl;
        }
        else
        {
            // Invalid move: stay in selected state so player can try again
            std::cerr << "Invalid move from field " << selectedPieceFieldId 
                      << " to field " << clickedFieldId;

            if (hasAnyCaptureForCurrentPlayer())
            {
                std::cerr << " — a capture is available and must be taken.";
            }

            std::cerr << std::endl;
        }
    }
}

// ! Switches the turn to the other player
void Game::switchTurn()
{
    if (currentTurn == PlayerTurn::WHITE)
    {
        currentTurn = PlayerTurn::BLACK;
        std::cout << "Turn: BLACK" << std::endl;
    }
    else
    {
        currentTurn = PlayerTurn::WHITE;
        std::cout << "Turn: WHITE" << std::endl;
    }
}
