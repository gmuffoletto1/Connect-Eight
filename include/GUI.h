#ifndef GUI_H
#define GUI_H

#include "AI.h"
#include "Game.h"

#include <SFML/Graphics.hpp>
#include <string>

class GUI {
public:
    GUI();
    void run();

private:
    // Layout constants
    static const int CELL_SIZE      = 16;
    static const int BOARD_OFFSET_X = 20;
    static const int BOARD_OFFSET_Y = 20;
    static const int BOARD_PIXELS   = Board::COLS * CELL_SIZE;   // 800
    static const int HUD_HEIGHT     = 90;
    static const int WINDOW_WIDTH   = BOARD_PIXELS + 2 * BOARD_OFFSET_X;
    static const int WINDOW_HEIGHT  = BOARD_PIXELS + 2 * BOARD_OFFSET_Y + HUD_HEIGHT;

    sf::RenderWindow window;
    sf::Font         font;
    bool             fontLoaded;

    Game game;
    AI   ai;

    bool aiThinking;
    std::string statusText;

    // Button rectangles
    sf::FloatRect newGameBtn;
    sf::FloatRect undoBtn;

    void draw();
    void drawBoard();
    void drawHUD();
    void drawWinHighlight();
    void drawButton(const sf::FloatRect& r, const std::string& label);

    // Returns true if (x,y) is inside the board area; sets row, col on success.
    bool pixelToCell(int x, int y, int& row, int& col) const;
    int  pixelToColumn(int x) const;

    void handleLeftClick(int x, int y);
    void handleRightClick(int x, int y);

    // Make the AI play one move (called after the human moves).
    void doAIMove();

    void newGame();
};

#endif
