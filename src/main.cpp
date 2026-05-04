#include "cli/CLI.hpp"

#ifdef HAVE_RAYLIB
#include "gui/GuiApp.hpp"
#endif

#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    bool useGui = true;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--cli" || arg == "-c") useGui = false;
    }

#ifdef HAVE_RAYLIB
    if (useGui) {
        GuiApp app;
        if (app.init()) {
            app.run();
            app.shutdown();
            return 0;
        }
        std::cerr << "GUI init failed, falling back to CLI.\n";
    }
#endif

    CLI app;
    app.run();
    return 0;
}
