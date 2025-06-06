#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define TEXT_BOX_X 50
#define TEXT_BOX_Y 200
#define TEXT_BOX_WIDTH 700
#define TEXT_BOX_HEIGHT 200
#define MAX_TEXT_LENGTH 1000
#define FONT_SIZE 16

// Simple bitmap font rendering (8x16 pixels per character)
void draw_char(SDL_Renderer* renderer, char c, int x, int y) {
    // Simple 8x16 bitmap font patterns for basic ASCII characters
    static const unsigned char font_data[96][16] = {
        // Space (32)
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        // ! (33)
        {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        // " (34)
        {0x6C, 0x6C, 0x6C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        // # (35)
        {0x36, 0x36, 0x7F, 0x36, 0x36, 0x36, 0x7F, 0x36, 0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        // Continue with more characters... (simplified for demo)
    };
    
    // For simplicity, we'll use basic rectangle rendering for most characters
    // This is a simplified approach - in a real app you'd use TTF fonts
    
    if (c >= 32 && c <= 126) {
        // Draw a simple representation based on character
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        
        if (c == ' ') {
            // Space - draw nothing
            return;
        } else if (c >= 'A' && c <= 'Z') {
            // Capital letters - draw filled rectangle with gap
            SDL_FRect rect = {x + 1, y + 2, 6, 10};
            SDL_RenderFillRect(renderer, &rect);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_FRect inner = {x + 2, y + 4, 4, 6};
            SDL_RenderFillRect(renderer, &inner);
        } else if (c >= 'a' && c <= 'z') {
            // Lowercase letters - draw smaller filled rectangle
            SDL_FRect rect = {x + 1, y + 6, 6, 6};
            SDL_RenderFillRect(renderer, &rect);
        } else if (c >= '0' && c <= '9') {
            // Numbers - draw outlined rectangle
            SDL_FRect rect = {x + 1, y + 2, 6, 10};
            SDL_RenderRect(renderer, &rect);
            SDL_FRect inner = {x + 2, y + 3, 4, 8};
            SDL_RenderRect(renderer, &inner);
        } else {
            // Other characters - draw simple dot pattern
            SDL_FRect rect1 = {x + 2, y + 4, 2, 2};
            SDL_FRect rect2 = {x + 4, y + 8, 2, 2};
            SDL_RenderFillRect(renderer, &rect1);
            SDL_RenderFillRect(renderer, &rect2);
        }
    }
}

void draw_text(SDL_Renderer* renderer, const char* text, int x, int y) {
    int char_x = x;
    int char_y = y;
    
    for (int i = 0; text[i] != '\0'; i++) {
        if (text[i] == '\n') {
            char_x = x;
            char_y += FONT_SIZE + 2;
        } else {
            draw_char(renderer, text[i], char_x, char_y);
            char_x += 8; // Character width
            
            // Wrap to next line if needed
            if (char_x > TEXT_BOX_X + TEXT_BOX_WIDTH - 16) {
                char_x = x;
                char_y += FONT_SIZE + 2;
            }
        }
    }
}

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create window
    SDL_Window* window = SDL_CreateWindow(
        "SDL3 Text Input Demo",
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL
    );
    
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Text input variables
    char input_text[MAX_TEXT_LENGTH] = {0};
    int text_length = 0;
    bool text_input_active = true;
    Uint32 cursor_timer = 0;
    bool show_cursor = true;
    
    // Enable text input
    SDL_StartTextInput(window);

    // Main loop
    bool quit = false;
    SDL_Event e;

    printf("SDL3 Text Input Application started.\n");
    printf("Click in the text box and start typing!\n");
    printf("Press ESC to quit, Backspace to delete, Enter for new line.\n");

    while (!quit) {
        Uint32 current_time = SDL_GetTicks();
        
        // Handle events
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_EVENT_KEY_DOWN) {
                if (e.key.key == SDLK_ESCAPE) {
                    quit = true;
                }
                else if (e.key.key == SDLK_BACKSPACE && text_length > 0) {
                    // Handle backspace
                    text_length--;
                    input_text[text_length] = '\0';
                }
                else if (e.key.key == SDLK_RETURN) {
                    // Handle enter/return - add newline
                    if (text_length < MAX_TEXT_LENGTH - 1) {
                        input_text[text_length] = '\n';
                        text_length++;
                        input_text[text_length] = '\0';
                    }
                }
            }
            else if (e.type == SDL_EVENT_TEXT_INPUT) {
                // Handle text input
                int input_len = strlen(e.text.text);
                if (text_length + input_len < MAX_TEXT_LENGTH - 1) {
                    strcat(input_text, e.text.text);
                    text_length += input_len;
                }
            }
            else if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                // Check if clicked in text box
                int mouse_x = e.button.x;
                int mouse_y = e.button.y;
                
                if (mouse_x >= TEXT_BOX_X && mouse_x <= TEXT_BOX_X + TEXT_BOX_WIDTH &&
                    mouse_y >= TEXT_BOX_Y && mouse_y <= TEXT_BOX_Y + TEXT_BOX_HEIGHT) {
                    text_input_active = true;
                    SDL_StartTextInput(window);
                } else {
                    text_input_active = false;
                    SDL_StopTextInput(window);
                }
            }
        }

        // Update cursor blink
        if (current_time - cursor_timer > 500) { // Blink every 500ms
            show_cursor = !show_cursor;
            cursor_timer = current_time;
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 30, 30, 40, 255); // Dark blue background
        SDL_RenderClear(renderer);

        // Draw title
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        draw_text(renderer, "SDL3 Text Input Demo - Type something!", 50, 50);
        draw_text(renderer, "Press ESC to quit, Backspace to delete, Enter for new line", 50, 80);
        
        // Draw text box border
        SDL_SetRenderDrawColor(renderer, text_input_active ? 100 : 60, 
                              text_input_active ? 150 : 60, 
                              text_input_active ? 255 : 100, 255);
        SDL_FRect text_box = {TEXT_BOX_X - 2, TEXT_BOX_Y - 2, TEXT_BOX_WIDTH + 4, TEXT_BOX_HEIGHT + 4};
        SDL_RenderRect(renderer, &text_box);
        
        // Fill text box background
        SDL_SetRenderDrawColor(renderer, 20, 20, 25, 255);
        SDL_FRect text_box_fill = {TEXT_BOX_X, TEXT_BOX_Y, TEXT_BOX_WIDTH, TEXT_BOX_HEIGHT};
        SDL_RenderFillRect(renderer, &text_box_fill);

        // Draw input text
        if (text_length > 0) {
            draw_text(renderer, input_text, TEXT_BOX_X + 10, TEXT_BOX_Y + 10);
        }

        // Draw cursor
        if (text_input_active && show_cursor) {
            // Calculate cursor position
            int cursor_x = TEXT_BOX_X + 10;
            int cursor_y = TEXT_BOX_Y + 10;
            
            // Simple cursor positioning (end of text)
            for (int i = 0; i < text_length; i++) {
                if (input_text[i] == '\n') {
                    cursor_x = TEXT_BOX_X + 10;
                    cursor_y += FONT_SIZE + 2;
                } else {
                    cursor_x += 8;
                    if (cursor_x > TEXT_BOX_X + TEXT_BOX_WIDTH - 16) {
                        cursor_x = TEXT_BOX_X + 10;
                        cursor_y += FONT_SIZE + 2;
                    }
                }
            }
            
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_FRect cursor = {cursor_x, cursor_y, 2, FONT_SIZE};
            SDL_RenderFillRect(renderer, &cursor);
        }

        // Draw status text
        char status[256];
        snprintf(status, sizeof(status), "Characters: %d/%d | Text Input: %s", 
                text_length, MAX_TEXT_LENGTH, text_input_active ? "Active" : "Inactive");
        draw_text(renderer, status, 50, WINDOW_HEIGHT - 50);

        // Present the back buffer
        SDL_RenderPresent(renderer);

        // Small delay
        SDL_Delay(16); // ~60 FPS
    }

    // Stop text input
    SDL_StopTextInput(window);

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    printf("Final text entered:\n%s\n", input_text);
    printf("Application terminated successfully.\n");
    return 0;
}