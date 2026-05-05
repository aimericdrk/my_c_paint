#include "../include/paint.h"

int main(void)
{
    printf("\n");
    printf("╔════════════════════════════════════════╗\n");
    printf("║     🎨 PAINT - CSFML Edition 🎨        ║\n");
    printf("╚════════════════════════════════════════╝\n\n");
    printf("Initializing application...\n");

    app_t *app = init_app();

    if (!app)
    {
        printf("Failed to initialize application!\n");
        return 84;
    }

    printf("Application initialized successfully!\n");
    printf("Starting main loop...\n\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("CONTROLS:\n");
    printf("  • Left Click: Draw\n");
    printf("  • Right Click: Pick color from canvas\n");
    printf("  • Ctrl+Z: Undo\n");
    printf("  • Ctrl+Y: Redo\n");
    printf("  • Ctrl+S: Save\n");
    printf("  • Ctrl+O: Open\n");
    printf("  • Ctrl+N: New/Clear\n");
    printf("  • F1: Toggle debug info\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");

    run_app(app);

    printf("\nShutting down application...\n");
    cleanup_app(app);
    printf("Cleanup complete. Goodbye!\n\n");

    return 0;
}
