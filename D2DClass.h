#pragma once

#define WIN32_LEAN_AND_MEAN

#include <d2d1_3.h>
#include <dwrite_3.h>
#include <cmath>
#include <numbers>
#include <wincodec.h>
#include <combaseapi.h>
#include <vector>

#include "Game.h"

using D2D1::RenderTargetProperties;
using D2D1::HwndRenderTargetProperties;
using D2D1::SizeU;
using D2D1::Point2F;
using D2D1::StrokeStyleProperties;
using D2D1::ColorF;
using D2D1::LinearGradientBrushProperties;
using D2D1::RadialGradientBrushProperties;
using D2D1::Matrix3x2F;
using D2D1::RectF;
using D2D1::Ellipse;
using D2D1::BezierSegment;
using D2D1::QuadraticBezierSegment;
using D2D1::BitmapProperties;
using D2D1::PixelFormat;
using std::sin;

class D2DClass
{
private:
    RECT rc;
    // Interfejsy potrzebne do zainicjowania Direct2D
    ID2D1Factory7* d2d_factory = nullptr;
    ID2D1HwndRenderTarget* d2d_render_target = nullptr;

    // Interfejsy DirectWrite
    IDWriteFactory* write_factory = nullptr;
    IDWriteTextFormat* text_format = nullptr;

    // Interfejsy potrzebne do rysowania
    ID2D1SolidColorBrush* brush = nullptr;
    // - Pêdzel - gradient promienisty stworek
    ID2D1RadialGradientBrush* rad_brush = nullptr;
    ID2D1GradientStopCollection* rad_stops = nullptr;
    //UINT const NUM_RAD_STOPS = 3;
    D2D1_GRADIENT_STOP rad_stops_data[3];
    // - Pêdzle - gradient promienisty oczy
    ID2D1RadialGradientBrush* rad_brush2 = nullptr;
    ID2D1RadialGradientBrush* rad_brush3 = nullptr;
    ID2D1GradientStopCollection* rad_stops2 = nullptr;
    UINT const NUM_RAD_STOPS2 = 3;
    D2D1_GRADIENT_STOP rad_stops_data2[3];

    // - Macierz do po³¹czenia transformacji
    Matrix3x2F transformation;

    // Sta³e kolorów
    D2D1_COLOR_F const clear_color =
    { .r = 0.75f, .g = 0.75f, .b = 1.0f, .a = 1.0f };
    D2D1_COLOR_F const brush_color =
    { .r = 0.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f };

    D2D1_COLOR_F const picked_color =
    { .r = 0.0f, .g = 0.0f, .b = 1.0f, .a = 1.0f };
    D2D1_COLOR_F const wrong_color =
    { .r = 1.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f };
    D2D1_COLOR_F const legal_move_color =
    { .r = 0.0f, .g = 1.0f, .b = 0.0f, .a = 1.0f };

    D2D1_COLOR_F const white_piece_color = ColorF(ColorF::Beige, 1.0f);
    D2D1_COLOR_F const black_piece_color = ColorF(ColorF::Chocolate, 1.0f);

    D2D1_COLOR_F const contour_color =
    { .r = 0.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f };

    FLOAT const contour_width = 3.0f;

    // Interfejsy do obs³ugi œcie¿ki
    ID2D1PathGeometry* pawn_path = nullptr;
    ID2D1GeometrySink* pawn_path_sink = nullptr;

    // do wczytywania bitmap
    IWICImagingFactory* wic_factory = NULL;
    IWICBitmapDecoder* wic_decoder = NULL;
    IWICFormatConverter* wic_converter = NULL;
    IWICBitmapFrameDecode* bmp_src = NULL;

    ID2D1Bitmap* bmp_white_tile = NULL;
    ID2D1Bitmap* bmp_black_tile = NULL;
    ID2D1Bitmap* bmp_white_knight = NULL;
    ID2D1Bitmap* bmp_black_knight = NULL;
    ID2D1Bitmap* bmp_white_bishop = NULL;
    ID2D1Bitmap* bmp_black_bishop = NULL;
    ID2D1Bitmap* bmp_white_rook = NULL;
    ID2D1Bitmap* bmp_black_rook = NULL;
    ID2D1Bitmap* bmp_white_queen = NULL;
    ID2D1Bitmap* bmp_black_queen = NULL;
    ID2D1Bitmap* bmp_white_king = NULL;
    ID2D1Bitmap* bmp_black_king = NULL;

    // nazwy plików
    LPCWSTR white_filename = L"check_white.png";
    LPCWSTR black_filename = L"check_black.png";
    LPCWSTR knight_white_filename = L"knight_white.png";
    LPCWSTR knight_black_filename = L"knight_black.png";
    LPCWSTR bishop_white_filename = L"bishop_white.png";
    LPCWSTR bishop_black_filename = L"bishop_black.png";
    LPCWSTR rook_white_filename = L"rook_white.png";
    LPCWSTR rook_black_filename = L"rook_black.png";
    LPCWSTR queen_white_filename = L"queen_white.png";
    LPCWSTR queen_black_filename = L"queen_black.png";
    LPCWSTR king_white_filename = L"king_white.png";
    LPCWSTR king_black_filename = L"king_black.png";

    // Texts
    WCHAR const text_check[6] = L"CHECK";
    D2D1_COLOR_F const text_color_check =
    { .r = 1.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f };

    FLOAT board_rotation = 0.0f;
    FLOAT rotation_start_time = 0.0f;
    FLOAT rotation_duration = 9.0f;
    FLOAT time = 0.0f;

    BOOL first_move = TRUE;
    BOOL switched = FALSE;
    INT count_rotations = 0;

    FLOAT picked_pulse_scale = 1.0f; // 0.8 do 1.2
    

    Game game;

    INT tile_size;
    INT board_left;
    INT board_top;

    

    void LoadBmp(LPCWSTR filename, ID2D1Bitmap** bmp);

    void GeneratePawn();

    void DrawBoard();

    void DrawPieceBmp(INT x, INT y, ID2D1Bitmap* bmp, BOOL is_picked);

    void DrawPawn(D2D1_POINT_2F center, BOOL is_white, BOOL is_picked);

    void DrawPieces();

    void DrawTexts();
public:
    void InitDirect2D(HWND hwnd);

    void DestroyDirect2D();

    void HandleMouseClick(FLOAT mouse_x, FLOAT mouse_y);

    void OnPaint(HWND hwnd);
};

