#pragma once

#include <d2d1_3.h>
#include <dwrite_3.h>
#include <cmath>
#include <numbers>
#include <wincodec.h>
#include <combaseapi.h>
#include <vector>

#define BOARD_SIZE 8

enum class PieceType
{
	PAWN,
	KING,
	QUEEN,
	ROOK,
	BISHOP,
	KNIGHT
};

enum class GameState
{
	SAME_TURN,
	NEXT_TURN,
	DRAW,
	CHECKMATE
};

struct Position {
	INT x, y;

	Position(INT x, INT y) : x(x), y(y) {}

	BOOL operator==(Position p) {
		return x == p.x && y == p.y;
	}
	BOOL operator!=(Position p) {
		return x != p.x || y != p.y;
	}
};

struct Piece {
	PieceType type;
	BOOL is_white;
	BOOL is_dead;
	BOOL is_first_move;
	Position pos;
	BOOL is_disabled;

	Piece(PieceType _type, BOOL _is_white, INT _x, INT _y) : type(_type), is_white(_is_white), is_dead(FALSE), is_first_move(TRUE), pos(Position(_x, _y)), is_disabled(FALSE) {
	}
};

struct Tile {
	Position pos;
	INT piece;

	Tile(Position pos, INT piece) : pos(pos), piece(piece) {}
	Tile(Position pos) : pos(pos), piece(-1) {}
};

class Game
{
private:
	// Zmienne dzia³ania programu
	INT board_size = BOARD_SIZE;

	Position wrong_pos = Position(-1, -1);
	Position picked_pos = Position(-1, -1);
	std::vector<Tile> tiles;

	BOOL is_white_turn = TRUE;

	BOOL is_piece_picked = FALSE;
	INT picked_piece = -1; // idx in pieces or -1 if none

	std::vector<Piece> pieces;

	std::vector<INT> who_checks;

	INT count_dead_white = 0;
	INT count_dead_black = 0;

	BOOL is_check = FALSE;

	void InitPieces();

	INT PieceOnTile(Position p);

	void DisablePieceAt(Position pos);

	void EnablePieceAt(Position pos);

	void AddAttackedTiles(INT piece, BOOL is_kings_move = FALSE);

	void AddLegalMoves();

	BOOL AddMove(Position pos, BOOL can_uncover_king = FALSE, BOOL is_attack_possible = TRUE, BOOL is_not_attack_possible = TRUE);

	void AddDiagonals(Position pos, BOOL can_uncover_king = FALSE);

	void AddLines(Position pos, BOOL can_uncover_king = FALSE);

	void AddKingsMoves(Position pos);

	void KillPiece(INT piece);

	INT IsLegalMove(Position pos);

	void CheckWhoChecks(BOOL is_king_white);

	BOOL IsTileAttacked(Position pos, BOOL by_white, BOOL is_kings_move = FALSE);

	BOOL IsKingChecked(BOOL is_white);

	void FilterNotAttackedTiles(BOOL by_white, size_t count_moves);

	BOOL DoesMoveUncoverKing(Position end_pos);

	INT FindKing(BOOL is_white);

	BOOL IsMate(BOOL is_white);
public:
	Game();

	Position GetWrongPos() {
		return wrong_pos;
	}

	Position GetPickedPos() {
		return picked_pos;
	}

	BOOL IsCheck() {
		return is_check;
	}

	BOOL IsWhiteTurn() {
		return is_white_turn;
	}

	void SetWrongPos(Position p) {
		wrong_pos = p;
	}

	void SetPickedPos(Position p) {
		picked_pos = p;
	}

	void SetPickedPiece(INT i) {
		picked_piece = i;
	}

	std::vector<Tile> GetLegalTiles() {
		return tiles;
	}

	void ClearLegalTiles() {
		tiles.clear();
	}

	std::vector<Piece> GetPieces() {
		return pieces;
	}

	GameState HandleClick(Position pos);

};

