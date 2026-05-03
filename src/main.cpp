#include "GUI.h"

#include <exception>
#include <iostream>

int main()
{
    try {
        GUI gui;
        gui.run();
    } catch (const std::exception& ex) {
        std::cerr << "Fatal error: " << ex.what() << '\n';
        return 1;
    }

    return 0;
}
