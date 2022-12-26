#include "Game.h"

Game::Game() {
    InitPieces();
}

void Game::InitPieces() {
    // Pawns
    for (size_t j = 0; j < board_size; j++) {
        pieces.push_back(Piece(PieceType::PAWN, TRUE, j, board_size - 2));
        pieces.push_back(Piece(PieceType::PAWN, FALSE, j, 1));
    }
    // Knights
    pieces.push_back(Piece(PieceType::KNIGHT, FALSE, 1, 0));
    pieces.push_back(Piece(PieceType::KNIGHT, FALSE, board_size - 2, 0));
    pieces.push_back(Piece(PieceType::KNIGHT, TRUE, 1, board_size - 1));
    pieces.push_back(Piece(PieceType::KNIGHT, TRUE, board_size - 2, board_size - 1));
    // Bishops
    pieces.push_back(Piece(PieceType::BISHOP, FALSE, 2, 0));
    pieces.push_back(Piece(PieceType::BISHOP, FALSE, board_size - 3, 0));
    pieces.push_back(Piece(PieceType::BISHOP, TRUE, 2, board_size - 1));
    pieces.push_back(Piece(PieceType::BISHOP, TRUE, board_size - 3, board_size - 1));
    // Rooks
    pieces.push_back(Piece(PieceType::ROOK, FALSE, 0, 0));
    pieces.push_back(Piece(PieceType::ROOK, FALSE, board_size - 1, 0));
    pieces.push_back(Piece(PieceType::ROOK, TRUE, 0, board_size - 1));
    pieces.push_back(Piece(PieceType::ROOK, TRUE, board_size - 1, board_size - 1));
    // Queens
    pieces.push_back(Piece(PieceType::QUEEN, FALSE, 3, 0));
    pieces.push_back(Piece(PieceType::QUEEN, TRUE, 3, board_size - 1));
    // Kings
    pieces.push_back(Piece(PieceType::KING, FALSE, 4, 0));
    pieces.push_back(Piece(PieceType::KING, TRUE, 4, board_size - 1));
}

INT Game::PieceOnTile(Position p) {
    for (size_t i = 0; i < pieces.size(); i++) {
        if (!pieces[i].is_dead && pieces[i].pos == p) {
            return i;
        }
    }
    return -1;
}

void Game::AddLegalMoves() {
    INT dir = pieces[picked_piece].is_white ? -1 : 1;
    Position pos = pieces[picked_piece].pos;
    BOOL one_step_legal;
    switch (pieces[picked_piece].type)
    {
    case PieceType::PAWN:
        one_step_legal = AddMove(Position(pos.x, pos.y + dir), FALSE, FALSE, TRUE);
        AddMove(Position(pos.x + 1, pos.y + dir), FALSE, TRUE, FALSE);
        AddMove(Position(pos.x - 1, pos.y + dir), FALSE, TRUE, FALSE);
        if (one_step_legal && pieces[picked_piece].is_first_move) {
            AddMove(Position(pos.x, pos.y + 2 * dir), FALSE, FALSE, TRUE);
        }
        break;
    case PieceType::KNIGHT:
        AddMove(Position(pos.x - 1, pos.y - 2));
        AddMove(Position(pos.x - 2, pos.y - 1));
        AddMove(Position(pos.x - 2, pos.y + 1));
        AddMove(Position(pos.x - 1, pos.y + 2));
        AddMove(Position(pos.x + 1, pos.y + 2));
        AddMove(Position(pos.x + 2, pos.y + 1));
        AddMove(Position(pos.x + 2, pos.y - 1));
        AddMove(Position(pos.x + 1, pos.y - 2));
        break;
    case PieceType::BISHOP:
        AddDiagonals(pos);
        break;
    case PieceType::ROOK:
        AddLines(pos);
        break;
    case PieceType::QUEEN:
        AddLines(pos);
        AddDiagonals(pos);
        break;
    case PieceType::KING:
        AddKingsMoves(pos);
        break;
    default:
        break;
    }
}

void Game::AddKingsMoves(Position pos) {
    AddMove(Position(pos.x - 1, pos.y), TRUE);
    AddMove(Position(pos.x - 1, pos.y - 1), TRUE);
    AddMove(Position(pos.x, pos.y - 1), TRUE);
    AddMove(Position(pos.x + 1, pos.y - 1), TRUE);
    AddMove(Position(pos.x + 1, pos.y), TRUE);
    AddMove(Position(pos.x + 1, pos.y + 1), TRUE);
    AddMove(Position(pos.x, pos.y + 1), TRUE);
    AddMove(Position(pos.x - 1, pos.y + 1), TRUE);

    DisablePieceAt(pos);
    is_white_turn = !is_white_turn;

    // tutaj jest is_white turn przez zamianê w poprzedniej linijce
    FilterNotAttackedTiles(is_white_turn);

    is_white_turn = !is_white_turn;
    EnablePieceAt(pos);
}

void Game::AddAttackedTiles(INT piece, BOOL is_kings_move) {
    Position pos = pieces[piece].pos;
    INT dir = pieces[piece].is_white ? -1 : 1;

    switch (pieces[piece].type)
    {
    case PieceType::BISHOP:
        AddDiagonals(pos, TRUE);
        break;
    case PieceType::ROOK:
        AddLines(pos, TRUE);
        break;
    case PieceType::QUEEN:
        AddLines(pos, TRUE);
        AddDiagonals(pos, TRUE);
        break;
    case PieceType::PAWN:
        AddMove(Position(pos.x + 1, pos.y + dir), TRUE);
        AddMove(Position(pos.x - 1, pos.y + dir), TRUE);
        break;
    case PieceType::KNIGHT:
        AddMove(Position(pos.x - 1, pos.y - 2), TRUE);
        AddMove(Position(pos.x - 2, pos.y - 1), TRUE);
        AddMove(Position(pos.x - 2, pos.y + 1), TRUE);
        AddMove(Position(pos.x - 1, pos.y + 2), TRUE);
        AddMove(Position(pos.x + 1, pos.y + 2), TRUE);
        AddMove(Position(pos.x + 2, pos.y + 1), TRUE);
        AddMove(Position(pos.x + 2, pos.y - 1), TRUE);
        AddMove(Position(pos.x + 1, pos.y - 2), TRUE);
        break;
    default:
        break;
    }
    if (is_kings_move) {
        switch (pieces[piece].type)
        {
        case PieceType::KING:
            AddMove(Position(pos.x - 1, pos.y), TRUE);
            AddMove(Position(pos.x - 1, pos.y - 1), TRUE);
            AddMove(Position(pos.x, pos.y - 1), TRUE);
            AddMove(Position(pos.x + 1, pos.y - 1), TRUE);
            AddMove(Position(pos.x + 1, pos.y), TRUE);
            AddMove(Position(pos.x + 1, pos.y + 1), TRUE);
            AddMove(Position(pos.x, pos.y + 1), TRUE);
            AddMove(Position(pos.x - 1, pos.y + 1), TRUE);
            break;
        default:
            break;
        }
    }
}

void Game::FilterNotAttackedTiles(BOOL by_white) {
    BOOL result = FALSE;

    std::vector<Tile> king_tiles = tiles;
    std::vector<Tile> new_tiles;

    BOOL is_attacked = FALSE;
    for (auto& king_tile : king_tiles) {
        tiles.clear();
        DisablePieceAt(king_tile.pos);
        for (size_t i = 0; i < pieces.size(); i++) {
            if (!pieces[i].is_dead && pieces[i].is_white == by_white) {
                AddAttackedTiles(i, TRUE);
            }
        }
        EnablePieceAt(king_tile.pos);
        for (auto& att_tile : tiles) {
            if (att_tile.pos == king_tile.pos) {
                is_attacked = TRUE;
                break;
            }
        }
        if (!is_attacked) {
            new_tiles.push_back(king_tile);
        }
        is_attacked = FALSE;
    }

    tiles = new_tiles;
}

BOOL Game::IsTileAttacked(Position pos, BOOL by_white, BOOL is_kings_move) {
    BOOL result = FALSE;

    std::vector<Tile> temp_tiles = tiles;
    tiles.clear();

    for (size_t i = 0; i < pieces.size(); i++) {
        if (!pieces[i].is_dead && pieces[i].is_white == by_white) {
            AddAttackedTiles(i, is_kings_move);
        }
    }
    for (auto& tile : tiles) {
        if (tile.pos == pos) {
            result = TRUE;
        }
    }

    tiles = temp_tiles;

    return result;
}

void Game::DisablePieceAt(Position pos) {
    for (auto& piece : pieces) {
        if (piece.pos == pos) {
            if (!piece.is_dead) {
                piece.is_dead = TRUE;
                piece.is_disabled = TRUE;
            }
        }
    }
}

void Game::EnablePieceAt(Position pos) {
    for (auto& piece : pieces) {
        if (piece.pos == pos) {
            if (piece.is_disabled) {
                piece.is_dead = FALSE;
                piece.is_disabled = FALSE;
            }
        }
    }
}

// checks if by moving picked_piece to end_pos, we uncover king
BOOL Game::DoesMoveUncoverKing(Position end_pos) {
    INT king = FindKing(is_white_turn);
    Position start_pos = pieces[picked_piece].pos;
    DisablePieceAt(end_pos);
    pieces[picked_piece].pos = end_pos;
    is_white_turn = !is_white_turn;

    // tutaj jest is_white turn przez zamianê w poprzedniej linijce
    BOOL result = IsTileAttacked(pieces[king].pos, is_white_turn);

    is_white_turn = !is_white_turn;
    pieces[picked_piece].pos = start_pos;
    EnablePieceAt(end_pos);
    return result;
}

void Game::CheckWhoChecks(BOOL is_king_white) {
    INT king = FindKing(is_king_white);
    Position king_pos = pieces[king].pos;
    INT dir = !is_king_white ? -1 : 1;
    INT x, y;

    for (size_t i = 0; i < pieces.size(); i++) {
        if (!pieces[i].is_dead && pieces[i].is_white == !is_king_white) {
            x = pieces[i].pos.x;
            y = pieces[i].pos.y;
            if (pieces[i].type == PieceType::PAWN) {
                if (Position(x + 1, y + dir) == king_pos || Position(x - 1, y + dir) == king_pos) {
                    who_checks.push_back(i);
                }
            }
            else if (pieces[i].type == PieceType::KNIGHT) {
                if (Position(x - 1, y - 2) == king_pos 
                    || Position(x + 1, y - 2) == king_pos
                    || Position(x + 2, y - 1) == king_pos
                    || Position(x + 2, y + 1) == king_pos
                    || Position(x + 1, y + 2) == king_pos
                    || Position(x - 1, y + 2) == king_pos
                    || Position(x - 2, y + 1) == king_pos
                    || Position(x - 2, y - 1) == king_pos) {
                    who_checks.push_back(i);
                }
            }
            if (pieces[i].type == PieceType::BISHOP || pieces[i].type == PieceType::QUEEN) {
                BOOL is_obstacle = FALSE;
                INT j = 1;
                if (x - y == king_pos.x - king_pos.y) {
                    if (x < king_pos.x) {
                        while (Position(x + j, y + j) != king_pos) {
                            if (PieceOnTile(Position(x + j, y + j)) != -1) {
                                is_obstacle = TRUE;
                                break;
                            }
                            j++;
                        }
                    }
                    else if (x > king_pos.x){
                        while (Position(x - j, y - j) != king_pos) {
                            if (PieceOnTile(Position(x - j, y - j)) != -1) {
                                is_obstacle = TRUE;
                                break;
                            }
                            j++;
                        }
                    }
                } 
                else if (x + y == king_pos.x + king_pos.y) {
                    if (x < king_pos.x) {
                        while (Position(x + j, y - j) != king_pos) {
                            if (PieceOnTile(Position(x + j, y - j)) != -1) {
                                is_obstacle = TRUE;
                                break;
                            }
                            j++;
                        }
                    }
                    else if (x > king_pos.x) {
                        while (Position(x - j, y + j) != king_pos) {
                            if (PieceOnTile(Position(x - j, y + j)) != -1) {
                                is_obstacle = TRUE;
                                break;
                            }
                            j++;
                        }
                    }
                }
                else {
                    continue;
                }
                if (!is_obstacle) {
                    who_checks.push_back(i);
                }
            }
            if (pieces[i].type == PieceType::ROOK || pieces[i].type == PieceType::QUEEN) {
                BOOL is_obstacle = FALSE;
                INT j = 1;
                if (x == king_pos.x) {
                    if (y < king_pos.y) {
                        while (Position(x, y + j) != king_pos) {
                            if (PieceOnTile(Position(x, y + j)) != -1) {
                                is_obstacle = TRUE;
                                break;
                            }
                            j++;
                        }
                    }
                    else if (y > king_pos.y) {
                        while (Position(x, y - j) != king_pos) {
                            if (PieceOnTile(Position(x, y - j)) != -1) {
                                is_obstacle = TRUE;
                                break;
                            }
                            j++;
                        }
                    }
                }
                else if (y == king_pos.y) {
                    if (x < king_pos.x) {
                        while (Position(x + j, y) != king_pos) {
                            if (PieceOnTile(Position(x + j, y)) != -1) {
                                is_obstacle = TRUE;
                                break;
                            }
                            j++;
                        }
                    }
                    else if (x > king_pos.x) {
                        while (Position(x - j, y) != king_pos) {
                            if (PieceOnTile(Position(x - j, y)) != -1) {
                                is_obstacle = TRUE;
                                break;
                            }
                            j++;
                        }
                    }
                }
                else {
                    continue;
                }
                if (!is_obstacle) {
                    who_checks.push_back(i);
                }
            }
        }
    }
}

INT Game::FindKing(BOOL is_white) {
    for (size_t i = 0; i < pieces.size(); i++) {
        if (pieces[i].type == PieceType::KING && pieces[i].is_white == is_white) {
            return i;
        }
    }
    // nie powinno nigdy to zajœæ
    return -1;
}


BOOL Game::AddMove(Position pos, BOOL can_uncover_king, BOOL is_attack_possible, BOOL is_not_attack_possible) {
    if (pos.x < 0 || pos.x >= board_size || pos.y < 0 || pos.y >= board_size) {
        return FALSE;
    }

    BOOL allowed = TRUE;
    INT attacks = -1;
    for (size_t i = 0; i < pieces.size(); i++) {
        if (!pieces[i].is_dead && pieces[i].pos == pos) {
            if (pieces[i].is_white == is_white_turn) {
                allowed = FALSE;
                break;
            }
            else if (!is_attack_possible) {
                allowed = FALSE;
                break;
            }
            else {
                attacks = i;
                break;
            }
        }
    }

    BOOL simple_allowed = allowed;

    if (attacks == -1 && !is_not_attack_possible) {
        allowed = FALSE;
    }
    if (!can_uncover_king) {
        if (DoesMoveUncoverKing(pos)) {
            allowed = FALSE;
        }
    }

    if (allowed) {
        tiles.push_back(Tile(pos, attacks));
    }

    // czy mo¿na i czy puste
    if (can_uncover_king || !is_check) {
        return allowed && attacks == -1;
    }
    else {
        if (!simple_allowed) {
            return FALSE;
        }

        BOOL kills_attacker = FALSE;
        for (INT who : who_checks) {
            if (who == attacks) {
                kills_attacker = TRUE;
                break;
            }
        }

        return !kills_attacker;
    }
}

void Game::AddDiagonals(Position pos, BOOL can_uncover_king) {
    size_t i = 1;
    // up, right
    while (pos.x + i < board_size && pos.y - i >= 0) {
        if (!AddMove(Position(pos.x + i, pos.y - i), can_uncover_king)) {
            break;
        }
        i++;
    }
    i = 1;
    // down, right
    while (pos.x + i < board_size && pos.y + i < board_size) {
        if (!AddMove(Position(pos.x + i, pos.y + i), can_uncover_king)) {
            break;
        }
        i++;
    }
    i = 1;
    // down, left
    while (pos.x - i >= 0 && pos.y + i < board_size) {
        if (!AddMove(Position(pos.x - i, pos.y + i), can_uncover_king)) {
            break;
        }
        i++;
    }
    i = 1;
    // up, left
    while (pos.x - i >= 0 && pos.y - i >= 0) {
        if (!AddMove(Position(pos.x - i, pos.y - i), can_uncover_king)) {
            break;
        }
        i++;
    }
}

void Game::AddLines(Position pos, BOOL can_uncover_king) {
    size_t i = 1;
    // up
    while (pos.y - i >= 0) {
        if (!AddMove(Position(pos.x, pos.y - i), can_uncover_king)) {
            break;
        }
        i++;
    }
    i = 1;
    // right
    while (pos.x + i < board_size) {
        if (!AddMove(Position(pos.x + i, pos.y), can_uncover_king)) {
            break;
        }
        i++;
    }
    i = 1;
    // down
    while (pos.y + i < board_size) {
        if (!AddMove(Position(pos.x, pos.y + i), can_uncover_king)) {
            break;
        }
        i++;
    }
    i = 1;
    // left
    while (pos.x - i >= 0) {
        if (!AddMove(Position(pos.x - i, pos.y), can_uncover_king)) {
            break;
        }
        i++;
    }
}

INT Game::IsLegalMove(Position pos) {
    for (size_t i = 0; i < tiles.size(); i++) {
        if (tiles[i].pos == pos) {
            return i;
        }
    }
    return -1;
}

void Game::KillPiece(INT piece) {
    pieces[piece].is_dead = TRUE;
    INT count = (pieces[piece].is_white ? count_dead_white : count_dead_black);
    pieces[piece].pos = Position((pieces[piece].is_white ? -1 - (count / board_size) : board_size + (count / board_size)), count % board_size);
    if (pieces[piece].is_white) {
        count_dead_white++;
    }
    else {
        count_dead_black++;
    }
}

BOOL Game::IsKingChecked(BOOL is_white) {
    INT king = FindKing(is_white);
    BOOL result = IsTileAttacked(pieces[king].pos, !is_white, TRUE);
    return result;
}

BOOL Game::IsMate(BOOL is_white) {
    is_white_turn = !is_white_turn;
    tiles.clear();
    for (size_t i = 0; i < pieces.size(); i++) {
        if (!pieces[i].is_dead && pieces[i].is_white == is_white) {
            picked_piece = i;
            AddLegalMoves();
        }
    }
    picked_piece = -1;
    is_white_turn = !is_white_turn;
    return tiles.empty();
}

BOOL Game::HandleClick(Position pos) {
    // pierwsze klikniêcie

    if (picked_piece == -1) {
        picked_piece = PieceOnTile(pos);
        if (picked_piece != -1 && pieces[picked_piece].is_white == is_white_turn) {
            picked_pos = pos;
            AddLegalMoves();
        }
        else {
            wrong_pos = pos;
        }
    }
    // drugie klikniêcie
    else {
        INT move_idx = IsLegalMove(pos);
        // klikniêto w legalne pole
        if (move_idx != -1) {
            pieces[picked_piece].pos = pos;
            pieces[picked_piece].is_first_move = FALSE;

            is_check = FALSE;
            if (tiles[move_idx].piece != -1) {
                KillPiece(tiles[move_idx].piece);
            }
            tiles.clear();
   
            if (IsKingChecked(!is_white_turn)) {
                is_check = TRUE;
                who_checks.clear();
                CheckWhoChecks(!is_white_turn);
                if (IsMate(!is_white_turn)) {
                    exit(0);
                }
            }

            is_white_turn = !is_white_turn;

            picked_piece = -1;
            tiles.clear();

            return TRUE;
        }
        // niepoprawne pole
        else {
            tiles.clear();
            picked_piece = PieceOnTile(pos);
            if (picked_piece != -1 && pieces[picked_piece].is_white == is_white_turn) {
                picked_pos = pos;
                AddLegalMoves();
            }
            else {
                wrong_pos = pos;
                picked_piece = -1;
            }
        }
    }
    return FALSE;
}
