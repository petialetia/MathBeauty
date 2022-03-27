#include "../include/MathBeauty.hpp"

int main ()
{
    sdl_window_info win_info {};
    OpenWindowWithSurface (&win_info);

    screen_info scr_info = {(WINDOW_LENGTH/2 - ZERO_POINT_X)/START_SCALE, (WINDOW_WIDTH/2 - ZERO_POINT_Y)/START_SCALE, 1/START_SCALE};
    SDL_Event event;

    bool is_programm_ended = false;

    while (!is_programm_ended)
    {
        DrawFrame (&win_info, &scr_info);
        CheckEvent (&win_info, &scr_info, &event, &is_programm_ended);
    }
}

void OpenWindowWithSurface (sdl_window_info* win_info)
{
    assert (win_info != nullptr);

    win_info->window = SDL_CreateWindow ("Mandelbrot", (SCREEN_LENGTH - WINDOW_LENGTH)/2, (SCREEN_WIDTH - WINDOW_WIDTH)/2, WINDOW_LENGTH, WINDOW_WIDTH, SDL_WINDOW_MAXIMIZED);
    assert (win_info->window != nullptr);

    SDL_SetWindowSize (win_info->window, WINDOW_LENGTH, WINDOW_WIDTH);

    win_info->surface = SDL_GetWindowSurface (win_info->window);
    assert (win_info->surface != nullptr); 
}

void DrawFrame (sdl_window_info* win_info, screen_info* scr_info)
{
    assert (win_info != nullptr);
    assert (scr_info != nullptr);

    clock_t start_time = clock ();

    win_info->surface = SDL_GetWindowSurface (win_info->window);
    CalculateMandelbrot (win_info, scr_info);
    SDL_UpdateWindowSurface (win_info->window);

    clock_t end_time = clock ();

    WriteFPS (win_info, CLOCKS_PER_SEC / (end_time - start_time));
}

void CalculateMandelbrot (sdl_window_info* win_info, screen_info* scr_info)
{
    assert (win_info != nullptr);
    assert (scr_info != nullptr);

    float step_x = scr_info->scale,
          step_y = step_x;      

    __m256 offset_x_vector = _mm256_setr_ps (0, step_x, step_x * 2, step_x * 3, step_x * 4, step_x * 5, step_x * 6, step_x * 7);

    float start_y = scr_info->center_pixel_y + WINDOW_WIDTH * scr_info->scale / 2;

    for (int i_y = 0; i_y < WINDOW_WIDTH; i_y++, start_y -= step_y)
    {
        float start_x = scr_info->center_pixel_x - WINDOW_LENGTH * scr_info->scale / 2; 

        for (int i_x = 0; i_x < WINDOW_LENGTH; i_x += VECTOR_SIZE, start_x += VECTOR_SIZE * step_x)
        {
            __m256 start_x_vector = _mm256_set1_ps (start_x);
                   start_x_vector = _mm256_add_ps (start_x_vector, offset_x_vector);

            __m256 start_y_vector = _mm256_set1_ps (start_y);

            __m256 current_x = _mm256_setzero_ps (),
                   current_y = _mm256_setzero_ps ();

            __m256i counters = _mm256_set1_epi32 (0);

            for (int main_counter = 0; main_counter < MAX_COUNTER; main_counter++)
            {
                __m256 x_squared = _mm256_mul_ps (current_x, current_x),
                       y_squared = _mm256_mul_ps (current_y, current_y);

                __m256 cmp = _mm256_cmp_ps (_mm256_add_ps (x_squared, y_squared), MAX_R_VECTOR, _CMP_LT_OS);

                int mask = _mm256_movemask_ps (cmp);

                if (mask == 0) break;

                counters = _mm256_add_epi32 (counters, _mm256_castps_si256 (cmp));

                __m256 xy = _mm256_mul_ps (current_x, current_y);

                current_x = _mm256_add_ps (_mm256_sub_ps (x_squared, y_squared), start_x_vector);
                current_y = _mm256_add_ps (_mm256_add_ps (xy,        xy),        start_y_vector);
            }
            uint32_t* counter = (uint32_t*) &counters;
            for (int i = 0; i < VECTOR_SIZE; i++) SetPixel (win_info, i_x + i, i_y, Color (counter[i]*5, counter[i], counter[i]*5));
        }
    }    
}

inline uint32_t Color (unsigned char red, unsigned char green, unsigned char blue, unsigned char a)
{
    uint32_t color = a;
    color <<= BYTE_SIZE;
    color |= red;
    color <<= BYTE_SIZE;
    color |= green;
    color <<= BYTE_SIZE;
    color |= blue;
    return color;
}

inline void SetPixel (sdl_window_info* win_info, int x, int y, uint32_t color)
{
    assert (win_info != nullptr);

    uint32_t* pixel = (uint32_t*)((uint8_t*)win_info->surface->pixels + y * win_info->surface->pitch +
                                                                        x * win_info->surface->format->BytesPerPixel);

                                                                        // (uint8_t*) потому что win_info->surface->pitch, win_info->surface->format->BytesPerPixel даются в байтах
    *pixel = color;
}

void WriteFPS (sdl_window_info* win_info, int fps)
{
    assert (win_info != nullptr);
    assert (fps >= 0);

    static char title[TITLE_SIZE] = {};

    snprintf (title, TITLE_SIZE, "FPS %d", fps);
    SDL_SetWindowTitle (win_info->window, title);
}

void CheckEvent (sdl_window_info* win_info, screen_info* scr_info, SDL_Event* event, bool* is_programm_ended)
{
    assert (win_info          != nullptr);
    assert (scr_info          != nullptr);
    assert (event             != nullptr);
    assert (is_programm_ended != nullptr);

    while (SDL_PollEvent (event))
    {
        if (event->type == SDL_QUIT) 
        {
            ProcessEndEvent (win_info, is_programm_ended);
        }
        else 
        {
            if (event->type == SDL_KEYDOWN)
            {
                if (event->key.keysym.scancode == SDL_SCANCODE_ESCAPE) ProcessEndEvent (win_info, is_programm_ended);
                else MoveSet (event, scr_info);
            }
        }
    }
}

void ProcessEndEvent (sdl_window_info* win_info, bool* is_programm_ended)
{
    assert (win_info          != nullptr);
    assert (is_programm_ended != nullptr);

    SDL_DestroyWindow (win_info->window);
    SDL_Quit ();
    *is_programm_ended = true;
}

void MoveSet (SDL_Event* event, screen_info* scr_info)
{
    assert (event    != nullptr);
    assert (scr_info != nullptr);

    switch (event->key.keysym.scancode)
    {
        case SDL_SCANCODE_W:
            scr_info->center_pixel_y += HORIZONTAL_MOVEMENT_OFFSET * scr_info->scale;
            break;

        case SDL_SCANCODE_S:
            scr_info->center_pixel_y -= HORIZONTAL_MOVEMENT_OFFSET * scr_info->scale;
            break;

        case SDL_SCANCODE_A:
            scr_info->center_pixel_x -= VERTICAL_MOVEMENT_OFFSET * scr_info->scale;
            break;

        case SDL_SCANCODE_D:
            scr_info->center_pixel_x += VERTICAL_MOVEMENT_OFFSET * scr_info->scale;
            break;

        case SDL_SCANCODE_E:
            scr_info->scale /= SCALE_INCREASING;
            break;

        case SDL_SCANCODE_Q:
            scr_info->scale *= SCALE_INCREASING;
            break;

        default:
            break;
    }
} 
