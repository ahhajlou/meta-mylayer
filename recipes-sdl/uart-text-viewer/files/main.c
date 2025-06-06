#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        // printf("SDL could not initialize! SDL_Error: %d\n", SDL_GetError());
        return 1;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        // printf("SDL_ttf could not initialize! TTF_Error: %d\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    // Create window
    SDL_Window* window = SDL_CreateWindow(
        "Simple SDL3 Text",
        640, 480,
        0 // Using 0 for default window flags
    );

    if (window == NULL) {
        // printf("Window could not be created! SDL_Error: %d\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    if (renderer == NULL) {
        // printf("Renderer could not be created! SDL_Error: %d\n", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Load font
    const char* fontPath = "/usr/share/fonts/ttf/LiberationMono-Regular.ttf"; // Adjusted path, verify this
    // Common alternative paths:
    // const char* fontPath = "/usr/share/fonts/TTF/LiberationMono-Regular.ttf";
    // const char* fontPath = "LiberationMono-Regular.ttf"; // If in working directory
    TTF_Font* font = TTF_OpenFont(fontPath, 24); // Increased font size for visibility
    if (font == NULL) {
        // printf("Failed to load font! TTF_Error: %d\n", TTF_GetError());
        printf("Please ensure the font exists at: %s\n", fontPath);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Create text texture (do this once for static text)
    SDL_Color textColor = {255, 255, 255, 255}; // White text
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, "Hello, SDL3_ttf!", strlen("Hello, SDL3_ttf!"), textColor);
    if (textSurface == NULL) {
        // printf("Unable to render text surface! TTF_Error: %d\n", TTF_GetError());
        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (textTexture == NULL) {
        // printf("Unable to create texture from rendered text! SDL_Error: %d\n", SDL_GetError());
        SDL_DestroySurface(textSurface);
        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Get text dimensions for positioning
    SDL_FRect textRect;
    textRect.x = 50.0f; // Position X
    textRect.y = 50.0f; // Position Y
    textRect.w = (float)textSurface->w;
    textRect.h = (float)textSurface->h;

    SDL_DestroySurface(textSurface); // We no longer need the surface

    printf("SDL3 Application started. Press ESC or close window to quit.\n");

    // Main loop
    bool quit = false;
    SDL_Event e;

    while (!quit) {
        // Handle events
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                quit = true;
            } else if (e.type == SDL_EVENT_KEY_DOWN) {
                if (e.key.key == SDLK_ESCAPE) {
                    printf("ESC key has pressed.\n");
                    quit = true;
                }
                else if (e.key.key == SDLK_A) {
                    printf("KEY A has pressed.\n");
                }
                else if (e.key.key == SDLK_BACKSPACE || e.key.key == SDLK_RETURN) {
                    printf("Backspace or Return has pressed\n");
                    const char* newMessage = "New text";
                    SDL_Surface* newText = TTF_RenderText_Blended(font, newMessage, strlen(newMessage), textColor);
                    if (newText == NULL) {
                        // printf("Failed to render new text: %s\n", TTF_GetError());
                        continue;
                    }

                    SDL_Texture* newTexture = SDL_CreateTextureFromSurface(renderer, newText);
                    SDL_DestroySurface(newText); // Valid in SDL3

                    if (newTexture == NULL) {
                        // printf("Failed to create new texture: %s\n", SDL_GetError());
                        continue;
                    }

                    // Clear screen to black
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    SDL_RenderClear(renderer);

                    // Render the text texture
                    SDL_RenderTexture(renderer, newTexture, NULL, &textRect);

                    // Present the back buffer
                    SDL_RenderPresent(renderer);

                    // Small delay
                    SDL_Delay(40); // ~60 FPS                    

                    // Optional: clean up the old texture if needed
                    SDL_DestroyTexture(textTexture);
                    // textTexture = newTexture;

                    // Update textRect dimensions if needed
                    // textRect.w = (float)newText->w;
                    // textRect.h = (float)newText->h;

                }
            }

            fflush(stdout);
        }

        // Clear screen to black
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Render the text texture
        SDL_RenderTexture(renderer, textTexture, NULL, &textRect);

        // Present the back buffer
        SDL_RenderPresent(renderer);

        // Small delay
        SDL_Delay(16); // ~60 FPS
    }

    // Cleanup
    SDL_DestroyTexture(textTexture);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    printf("Application terminated successfully.\n");
    return 0;
}