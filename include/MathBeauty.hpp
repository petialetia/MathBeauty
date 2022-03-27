#include <immintrin.h>
#include <time.h>
#include <SDL2/SDL.h>

#include <assert.h>

const int SCREEN_LENGTH = 1920;
const int SCREEN_WIDTH  = 1080;

const int WINDOW_LENGTH = 768;
const int WINDOW_WIDTH  = 512;

const float ZERO_POINT_X = 2 * WINDOW_LENGTH / 3;
const float ZERO_POINT_Y = WINDOW_WIDTH / 2;

const float START_SCALE = WINDOW_WIDTH / 2;

const float SCALE_INCREASING = 1.1;
const int VERTICAL_MOVEMENT_OFFSET = 10;
const int HORIZONTAL_MOVEMENT_OFFSET = 10;

const int MAX_COUNTER = 512;
const __m256 MAX_R_VECTOR = _mm256_set1_ps (1000.0);

const int BYTE_SIZE = 8;
const int TITLE_SIZE = 100;
const int VECTOR_SIZE = 8;

struct sdl_window_info
{
    SDL_Window*  window   = nullptr;
    SDL_Surface* surface  = nullptr;
};

struct screen_info
{
    float center_pixel_x = 0;
    float center_pixel_y = 0;
    float scale          = 0;
};

void OpenWindowWithSurface (sdl_window_info* win_info);


void DrawFrame (sdl_window_info* win_info, screen_info* scr_info);

void CalculateMandelbrot (sdl_window_info* win_info, screen_info* scr_info);

inline void SetPixel (sdl_window_info* win_info, int x, int y, uint32_t color);

inline uint32_t Color (unsigned char red, unsigned char green, unsigned char blue, unsigned char a = 255);

void WriteFPS (sdl_window_info* win_info, int fps);


void CheckEvent (sdl_window_info* win_info, screen_info* scr_info, SDL_Event* event, bool* is_programm_ended);

void ProcessEndEvent (sdl_window_info* win_info, bool* is_programm_ended);

void MoveSet (SDL_Event* event, screen_info* scr_info); 
