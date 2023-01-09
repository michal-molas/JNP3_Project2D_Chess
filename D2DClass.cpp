#include "D2DClass.h"

void D2DClass::InitDirect2D(HWND hwnd) {
	// Utworzenie fabryki Direct2D
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
		&d2d_factory);

	GetClientRect(hwnd, &rc);
	// Utworzenie celu renderowania w oknie Windows
	d2d_factory->CreateHwndRenderTarget(
		RenderTargetProperties(),
		HwndRenderTargetProperties(hwnd,
			SizeU(static_cast<UINT32>(rc.right) -
				static_cast<UINT32>(rc.left),
				static_cast<UINT32>(rc.bottom) -
				static_cast<UINT32>(rc.top))),
		&d2d_render_target);

	// Utworzenie fabryki DirectWrite
	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&write_factory)
	);

	// Utworzenie formatu napisu DirectWrite
	write_factory->CreateTextFormat(
		L"Times New Roman",
		nullptr,
		DWRITE_FONT_WEIGHT_BOLD,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		50.0f,
		L"en-us",
		&text_format
	);

	text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);


	// Utworzenie bitmap
	CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

	CoCreateInstance(
		CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		__uuidof(IWICImagingFactory),
		reinterpret_cast<LPVOID*>(&wic_factory)
	);

	LoadBmp(knight_white_filename, &bmp_white_knight);
	LoadBmp(knight_black_filename, &bmp_black_knight);
	LoadBmp(bishop_white_filename, &bmp_white_bishop);
	LoadBmp(bishop_black_filename, &bmp_black_bishop);
	LoadBmp(rook_white_filename, &bmp_white_rook);
	LoadBmp(rook_black_filename, &bmp_black_rook);
	LoadBmp(queen_white_filename, &bmp_white_queen);
	LoadBmp(queen_black_filename, &bmp_black_queen);
	LoadBmp(king_white_filename, &bmp_white_king);
	LoadBmp(king_black_filename, &bmp_black_king);


	// Utworzenie pêdzla
	d2d_render_target->CreateSolidColorBrush(contour_color, &brush);

	// Gradienty
	pawn_white_gradient_stops[0].color = D2D1::ColorF(D2D1::ColorF::Aquamarine, 1);
	pawn_white_gradient_stops[0].position = 0.0f;
	pawn_white_gradient_stops[1].color = { .r = 1.0f, .g = 204.f / 255.f, .b = 153.f / 255.f, .a = 1.0f };
	pawn_white_gradient_stops[1].position = 1.0f;

	d2d_render_target->CreateGradientStopCollection(
		pawn_white_gradient_stops,
		2,
		D2D1_GAMMA_2_2,
		D2D1_EXTEND_MODE_CLAMP,
		&pawn_white_gradient_stops_colection
	);

	d2d_render_target->CreateRadialGradientBrush(
		D2D1::RadialGradientBrushProperties(
			D2D1::Point2F(350, 250),
			D2D1::Point2F(0, 0),
			300,
			300),
		pawn_white_gradient_stops_colection,
		&pawn_white_rad_gradient_brush
	);

	pawn_black_gradient_stops[0].color = D2D1::ColorF(D2D1::ColorF::Yellow, 1);
	pawn_black_gradient_stops[0].position = 0.0f;
	pawn_black_gradient_stops[1].color = { .r = 33.f / 255.f, .g = 8.f / 255.f, .b = 10.f / 255.f, .a = 1.0f };
	pawn_black_gradient_stops[1].position = 1.0f;

	d2d_render_target->CreateGradientStopCollection(
		pawn_black_gradient_stops,
		2,
		D2D1_GAMMA_2_2,
		D2D1_EXTEND_MODE_CLAMP,
		&pawn_black_gradient_stops_colection
	);

	d2d_render_target->CreateRadialGradientBrush(
		D2D1::RadialGradientBrushProperties(
			D2D1::Point2F(350, 250),
			D2D1::Point2F(0, 0),
			200,
			200),
		pawn_black_gradient_stops_colection,
		&pawn_black_rad_gradient_brush
	);

	GeneratePawn();

	exit_left = rc.right - 2 * exit_size;
	exit_top = exit_size + exit_size / 2;
	GenerateExitButtton();

	// Parametry planszy
	INT min_size = min(rc.right, rc.bottom);
	tile_size = min_size / (BOARD_SIZE + 2);
	board_left = rc.right / 2 - (BOARD_SIZE / 2) * tile_size;
	board_top = tile_size;
}

void D2DClass::DestroyDirect2D() {
	if (brush) brush->Release();
	brush = nullptr;
	if (pawn_black_rad_gradient_brush) pawn_black_rad_gradient_brush->Release();
	pawn_black_rad_gradient_brush = nullptr;
	if (pawn_white_rad_gradient_brush) pawn_white_rad_gradient_brush->Release();
	pawn_white_rad_gradient_brush = nullptr;

	if (d2d_render_target) d2d_render_target->Release();
	if (d2d_factory) d2d_factory->Release();
}

void D2DClass::LoadBmp(LPCWSTR filename, ID2D1Bitmap** bmp) {
	wic_factory->CreateDecoderFromFilename(
		filename,
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&wic_decoder
	);

	// Create the initial frame.
	wic_decoder->GetFrame(0, &bmp_src);

	wic_factory->CreateFormatConverter(&wic_converter);

	wic_converter->Initialize(
		bmp_src,
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone,
		NULL,
		0.f,
		WICBitmapPaletteTypeMedianCut
	);

	d2d_render_target->CreateBitmapFromWicBitmap(wic_converter, NULL, bmp);
}

void D2DClass::GenerateExitButtton() {
	d2d_factory->CreatePathGeometry(&exit_path);
	exit_path->Open(&exit_path_sink);
	exit_path_sink->BeginFigure(Point2F(exit_left, exit_top), D2D1_FIGURE_BEGIN_FILLED);

	exit_path_sink->AddQuadraticBezier(QuadraticBezierSegment(
		Point2F(exit_left, exit_top - exit_size / 2), Point2F(exit_left + exit_size / 2, exit_top - exit_size / 2)));
	exit_path_sink->AddQuadraticBezier(QuadraticBezierSegment(
		Point2F(exit_left + exit_size, exit_top - exit_size / 2), Point2F(exit_left + exit_size, exit_top)));
	exit_path_sink->AddQuadraticBezier(QuadraticBezierSegment(
		Point2F(exit_left + exit_size, exit_top + exit_size / 2), Point2F(exit_left + exit_size / 2, exit_top + exit_size / 2)));
	exit_path_sink->AddQuadraticBezier(QuadraticBezierSegment(
		Point2F(exit_left, exit_top + exit_size / 2), Point2F(exit_left, exit_top)));
	exit_path_sink->EndFigure(D2D1_FIGURE_END_OPEN);
	exit_path_sink->Close();
}

void D2DClass::GeneratePawn() {
	d2d_factory->CreatePathGeometry(&pawn_path);
	pawn_path->Open(&pawn_path_sink);
	pawn_path_sink->BeginFigure(Point2F(300, 300), D2D1_FIGURE_BEGIN_FILLED);
	pawn_path_sink->AddBezier(BezierSegment(
		Point2F(275, 275), Point2F(275, 225), Point2F(300, 200)));
	pawn_path_sink->AddBezier(BezierSegment(
		Point2F(325, 175), Point2F(375, 175), Point2F(400, 200)));
	pawn_path_sink->AddBezier(BezierSegment(
		Point2F(425, 225), Point2F(425, 275), Point2F(400, 300)));
	pawn_path_sink->AddBezier(BezierSegment(
		Point2F(375, 325), Point2F(360, 350), Point2F(375, 375)));
	pawn_path_sink->AddBezier(BezierSegment(
		Point2F(400, 380), Point2F(425, 390), Point2F(400, 400)));
	pawn_path_sink->AddBezier(BezierSegment(
		Point2F(375, 410), Point2F(375, 440), Point2F(380, 450)));
	pawn_path_sink->AddBezier(BezierSegment(
		Point2F(400, 475), Point2F(430, 495), Point2F(420, 510)));
	pawn_path_sink->AddBezier(BezierSegment(
		Point2F(430, 510), Point2F(435, 510), Point2F(440, 510)));
	pawn_path_sink->AddBezier(BezierSegment(
		Point2F(440, 515), Point2F(440, 520), Point2F(440, 545)));
	pawn_path_sink->AddBezier(BezierSegment(
		Point2F(430, 545), Point2F(420, 545), Point2F(260, 545)));
	pawn_path_sink->AddBezier(BezierSegment(
		Point2F(260, 535), Point2F(260, 530), Point2F(260, 510)));
	pawn_path_sink->AddBezier(BezierSegment(
		Point2F(265, 510), Point2F(270, 510), Point2F(280, 510)));
	pawn_path_sink->AddBezier(BezierSegment(
		Point2F(270, 495), Point2F(300, 475), Point2F(320, 450)));
	pawn_path_sink->AddBezier(BezierSegment(
		Point2F(325, 440), Point2F(325, 410), Point2F(300, 400)));
	pawn_path_sink->AddBezier(BezierSegment(
		Point2F(275, 390), Point2F(300, 380), Point2F(325, 375)));
	pawn_path_sink->AddBezier(BezierSegment(
		Point2F(340, 350), Point2F(325, 325), Point2F(300, 300)));
	pawn_path_sink->EndFigure(D2D1_FIGURE_END_OPEN);
	pawn_path_sink->Close();
}

void D2DClass::DrawBoard() {
	transformation = Matrix3x2F::Rotation(board_rotation, Point2F(rc.right / 2, rc.bottom / 2));
	d2d_render_target->SetTransform(transformation);

	for (size_t i = 0; i < BOARD_SIZE; i++) {
		for (size_t j = 0; j < BOARD_SIZE; j++) {
			INT left_pos = board_left + j * tile_size;
			INT top_pos = board_top + i * tile_size;
			D2D1_RECT_F rect = D2D1::RectF(left_pos, top_pos, left_pos + tile_size, top_pos + tile_size);
			if ((i + j) % 2 == 0) {
				brush->SetColor(white_tile_color);
			}
			else {
				brush->SetColor(black_tile_color);
			}
			d2d_render_target->FillRectangle(rect, brush);
			brush->SetColor(contour_color);
			d2d_render_target->DrawRectangle(rect, brush);
		}
	}
	Position picked_pos = game.GetPickedPos();
	if (picked_pos != Position(-1, -1)) {
		INT left_pos = board_left + picked_pos.x * tile_size;
		INT top_pos = board_top + picked_pos.y * tile_size;
		D2D1_RECT_F rect = D2D1::RectF(left_pos + contour_width, top_pos + contour_width, left_pos + tile_size - contour_width, top_pos + tile_size - contour_width);
		brush->SetColor(picked_color);
		d2d_render_target->DrawRectangle(rect, brush, contour_width);
	}
	Position wrong_pos = game.GetWrongPos();
	if (wrong_pos != Position(-1, -1)) {
		INT left_pos = board_left + wrong_pos.x * tile_size;
		INT top_pos = board_top + wrong_pos.y * tile_size;
		D2D1_RECT_F rect = D2D1::RectF(left_pos + contour_width, top_pos + contour_width, left_pos + tile_size - contour_width, top_pos + tile_size - contour_width);
		brush->SetColor(wrong_color);
		d2d_render_target->DrawRectangle(rect, brush, contour_width);
	}
	std::vector<Tile> legal_tiles = game.GetLegalTiles();
	for (size_t i = 0; i < legal_tiles.size(); i++) {
		INT left_pos = board_left + legal_tiles[i].pos.x * tile_size;
		INT top_pos = board_top + legal_tiles[i].pos.y * tile_size;
		D2D1_RECT_F rect = D2D1::RectF(left_pos + contour_width, top_pos + contour_width, left_pos + tile_size - contour_width, top_pos + tile_size - contour_width);
		brush->SetColor(legal_move_color);
		d2d_render_target->DrawRectangle(rect, brush, contour_width);
	}
}

void D2DClass::DrawPieceBmp(INT x, INT y, ID2D1Bitmap* bmp, BOOL is_picked) {
	INT left_pos = board_left + x * tile_size;
	INT top_pos = board_top + y * tile_size;

	auto center = Point2F(left_pos + tile_size / 2, top_pos + tile_size / 2);

	const auto board_rotation_matrix = Matrix3x2F::Rotation(board_rotation, Point2F(rc.right / 2, rc.bottom / 2));
	const auto piece_rotation_matrix = Matrix3x2F::Rotation(board_rotation, center);

	transformation = piece_rotation_matrix * board_rotation_matrix;
	if (is_picked) {
		const auto pulse_matrix = Matrix3x2F::Scale(picked_pulse_scale, picked_pulse_scale, center);
		transformation = pulse_matrix * transformation;
	}

	d2d_render_target->SetTransform(transformation);

	D2D1_RECT_F rect = D2D1::RectF(left_pos, top_pos, left_pos + tile_size, top_pos + tile_size);
	d2d_render_target->DrawBitmap(bmp, &rect);
}

void D2DClass::DrawPawn(D2D1_POINT_2F center, BOOL is_white, BOOL is_picked) {
	const auto translation_matrix = Matrix3x2F::Translation(center.x - 300.0f - 50.0f, center.y - 300.0f - 60.0f);
	FLOAT scale_factor = static_cast<FLOAT>(tile_size) / 370.0f;
	const auto scale_matrix = Matrix3x2F::Scale(scale_factor, scale_factor, center);
	const auto board_rotation_matrix = Matrix3x2F::Rotation(board_rotation, Point2F(rc.right / 2, rc.bottom / 2));
	const auto piece_rotation_matrix = Matrix3x2F::Rotation(board_rotation, center);

	if (is_picked) {
		const auto pulse_matrix = Matrix3x2F::Scale(picked_pulse_scale, picked_pulse_scale, center);
		transformation = translation_matrix * scale_matrix * pulse_matrix * piece_rotation_matrix * board_rotation_matrix;
	}
	else {
		transformation = translation_matrix * scale_matrix * piece_rotation_matrix * board_rotation_matrix;
	}
	d2d_render_target->SetTransform(transformation);

	if (is_white) {
		d2d_render_target->FillGeometry(pawn_path, pawn_white_rad_gradient_brush);
	}
	else {
		d2d_render_target->FillGeometry(pawn_path, pawn_black_rad_gradient_brush);
	}

	brush->SetColor(contour_color);
	d2d_render_target->DrawGeometry(pawn_path, brush, contour_width);
}

void D2DClass::DrawPieces() {
	transformation = Matrix3x2F::Identity();
	d2d_render_target->SetTransform(transformation);
	std::vector<Piece> pieces = game.GetPieces();
	for (auto& piece : pieces) {
		INT x, y;
		x = piece.pos.x;
		y = piece.pos.y;

		BOOL is_picked = piece.pos == game.GetPickedPos();

		switch (piece.type) {
		case PieceType::PAWN:
			DrawPawn(Point2F(board_left + x * tile_size + tile_size / 2, board_top + y * tile_size + tile_size / 2), piece.is_white, is_picked);
			break;
		case PieceType::KNIGHT:
			DrawPieceBmp(x, y, piece.is_white ? bmp_white_knight : bmp_black_knight, is_picked);
			break;
		case PieceType::BISHOP:
			DrawPieceBmp(x, y, piece.is_white ? bmp_white_bishop : bmp_black_bishop, is_picked);
			break;
		case PieceType::ROOK:
			DrawPieceBmp(x, y, piece.is_white ? bmp_white_rook : bmp_black_rook, is_picked);
			break;
		case PieceType::QUEEN:
			DrawPieceBmp(x, y, piece.is_white ? bmp_white_queen : bmp_black_queen, is_picked);
			break;
		case PieceType::KING:
			DrawPieceBmp(x, y, piece.is_white ? bmp_white_king : bmp_black_king, is_picked);
			break;
		default:
			break;
		}
	}
}

void D2DClass::DrawExitButton() {
	transformation = Matrix3x2F::Identity();
	d2d_render_target->SetTransform(transformation);

	brush->SetColor(exit_color);
	d2d_render_target->FillGeometry(exit_path, brush);
	brush->SetColor(contour_color);
	d2d_render_target->DrawGeometry(exit_path, brush);

	D2D1_RECT_F rect = D2D1::RectF(
		exit_left + exit_size / 2 - exit_size / 16,
		exit_top - exit_size / 2 + exit_size / 16,
		exit_left + exit_size / 2 + exit_size / 16,
		exit_top + exit_size / 2 - exit_size / 16
	);

	transformation = Matrix3x2F::Rotation(45.0f, Point2F(exit_left + exit_size / 2, exit_top));
	d2d_render_target->SetTransform(transformation);
	d2d_render_target->FillRectangle(&rect, brush);
	transformation = Matrix3x2F::Rotation(135.0f, Point2F(exit_left + exit_size / 2, exit_top));
	d2d_render_target->SetTransform(transformation);
	d2d_render_target->FillRectangle(&rect, brush);
}

void D2DClass::HandleMouseClick(HWND hwnd, FLOAT mouse_x, FLOAT mouse_y) {
	if (mouse_x >= exit_left && mouse_x <= exit_left + exit_size && mouse_y >= exit_top - exit_size / 2 && mouse_y <= exit_top + exit_size / 2) {
		LANGID lang_id = GetUserDefaultUILanguage();
		auto text = lang_id == MAKELANGID(LANG_POLISH, SUBLANG_POLISH_POLAND) ? TEXT("Czy chcesz opuœciæ grê?") : TEXT("Do you want to exit?");
		if (MessageBox(hwnd, text,
			TEXT(""), MB_ICONQUESTION | MB_YESNO) == IDYES) {
			exit(0);
		}
	}

	if (!is_draw && !is_checkmate && (first_move || time > rotation_start_time + rotation_duration)) {
		game.SetWrongPos(Position(-1, -1));
		game.SetPickedPos(Position(-1, -1));

		if (mouse_x > board_left && mouse_y > board_top) {
			INT row = (static_cast<INT>(mouse_y) - board_top) / tile_size;
			INT col = (static_cast<INT>(mouse_x) - board_left) / tile_size;
			if (switched) {
				row = BOARD_SIZE - 1 - row;
				col = BOARD_SIZE - 1 - col;
			}


			if (row < BOARD_SIZE && col < BOARD_SIZE) {
				GameState state = game.HandleClick(Position(col, row));
				if (state == GameState::NEXT_TURN) {
					first_move = FALSE;
					switched = !switched;
					rotation_start_time = time;
					count_rotations++;
				}
				else if (state == GameState::DRAW) {
					is_draw = TRUE;
				}
				else if (state == GameState::CHECKMATE) {
					is_checkmate = TRUE;
				}
			}
		}
		else {
			game.SetPickedPos(Position(-1, -1));
			game.SetPickedPiece(-1);
			game.ClearLegalTiles();
		}
	}
}

void D2DClass::DrawTexts() {
	if (is_draw || is_checkmate) {
		INT left = 0;
		INT right = rc.right;
		INT top = 0;
		INT bottom = rc.bottom;

		transformation = Matrix3x2F::Identity();
		d2d_render_target->SetTransform(transformation);
		if (is_draw) {
			brush->SetColor(text_color_draw);
			d2d_render_target->DrawText(
				text_draw,
				sizeof(text_draw) / sizeof(text_draw[0]),
				text_format,
				RectF(
					left,
					top,
					right,
					bottom
				),
				brush
			);
		}
		else {
			brush->SetColor(text_color_draw);
			d2d_render_target->DrawText(
				text_checkmate,
				sizeof(text_checkmate) / sizeof(text_checkmate[0]),
				text_format,
				RectF(
					left,
					top,
					right,
					bottom
				),
				brush
			);
		}
	}
	else if (game.IsCheck()) {
		INT left = board_left;
		INT right = board_left + BOARD_SIZE * tile_size;
		INT top = 0;
		INT bottom = board_top;

		brush->SetColor(text_color_check);

		transformation = Matrix3x2F::Identity();
		d2d_render_target->SetTransform(transformation);
		d2d_render_target->DrawText(
			text_check,
			sizeof(text_check) / sizeof(text_check[0]),
			text_format,
			RectF(
				left,
				top,
				right,
				bottom
			),
			brush
		);
	}
}

void D2DClass::OnPaint(HWND hwnd) {
	d2d_render_target->BeginDraw();
	d2d_render_target->Clear(clear_color);

	time += 0.1f;

	picked_pulse_scale = 0.05f * sin(time) + 0.975f;

	if (!first_move && time < rotation_start_time + rotation_duration) {
		board_rotation = (count_rotations - 1) * 180.0f + 20.0f * (time - rotation_start_time);
	}
	else {
		board_rotation = count_rotations * 180.0f;
	}

	DrawBoard();
	DrawPieces();
	DrawTexts();
	DrawExitButton();


	if (d2d_render_target->EndDraw() == D2DERR_RECREATE_TARGET) {
		DestroyDirect2D();
		InitDirect2D(hwnd);
	}
}