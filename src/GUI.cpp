#include "GUI.h"

#include <array>
#include <sstream>

namespace {
sf::Color pieceColor(char cell)
{
    if (cell == Board::RED) {
        return sf::Color(220, 45, 45);
    }
    if (cell == Board::YELLOW) {
        return sf::Color(235, 215, 55);
    }
    return sf::Color(25, 35, 60);
}

std::string playerName(char player)
{
    return player == Board::RED ? "Red" : "Yellow";
}
}

GUI::GUI()
    : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Connect Eight"),
      fontLoaded(false),
      game(),
      ai(Board::YELLOW, 2),
      aiThinking(false),
      statusText("Red to move. Left click a column to drop; right click a yellow piece to remove."),
      newGameBtn(BOARD_OFFSET_X, BOARD_OFFSET_Y + BOARD_PIXELS + 20.0f, 130.0f, 42.0f),
      undoBtn(BOARD_OFFSET_X + 150.0f, BOARD_OFFSET_Y + BOARD_PIXELS + 20.0f, 90.0f, 42.0f)
{
    window.setFramerateLimit(60);

    const std::array<const char*, 5> fontPaths = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation2/LiberationSans-Regular.ttf",
        "/usr/share/fonts/truetype/freefont/FreeSans.ttf",
        "/Library/Fonts/Arial.ttf",
        "C:/Windows/Fonts/arial.ttf",
    };

    for (const char* path : fontPaths) {
        if (font.loadFromFile(path)) {
            fontLoaded = true;
            break;
        }
    }
}

void GUI::run()
{
    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    handleLeftClick(event.mouseButton.x, event.mouseButton.y);
                } else if (event.mouseButton.button == sf::Mouse::Right) {
                    handleRightClick(event.mouseButton.x, event.mouseButton.y);
                }
            } else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::N) {
                    newGame();
                } else if (event.key.code == sf::Keyboard::U) {
                    if (game.undoLast()) {
                        if (game.currentPlayer() == Board::YELLOW) {
                            game.undoLast();
                        }
                        statusText = "Undid the last turn.";
                    }
                } else if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
            }
        }

        draw();
    }
}

void GUI::draw()
{
    window.clear(sf::Color(18, 22, 32));
    drawBoard();
    drawWinHighlight();
    drawHUD();
    window.display();
}

void GUI::drawBoard()
{
    sf::RectangleShape boardBg(sf::Vector2f(BOARD_PIXELS, BOARD_PIXELS));
    boardBg.setPosition(BOARD_OFFSET_X, BOARD_OFFSET_Y);
    boardBg.setFillColor(sf::Color(35, 80, 160));
    window.draw(boardBg);

    const Board& board = game.getBoard();
    for (int row = 0; row < Board::ROWS; ++row) {
        for (int col = 0; col < Board::COLS; ++col) {
            sf::CircleShape piece((CELL_SIZE - 3.0f) / 2.0f);
            piece.setPosition(
                BOARD_OFFSET_X + col * CELL_SIZE + 1.5f,
                BOARD_OFFSET_Y + row * CELL_SIZE + 1.5f);
            piece.setFillColor(pieceColor(board.cell(row, col)));
            piece.setOutlineThickness(1.0f);
            piece.setOutlineColor(sf::Color(8, 15, 35));
            window.draw(piece);
        }
    }
}

void GUI::drawHUD()
{
    drawButton(newGameBtn, "New Game");
    drawButton(undoBtn, "Undo");

    if (!fontLoaded) {
        return;
    }

    std::ostringstream info;
    if (game.winner() != Board::EMPTY) {
        info << playerName(game.winner()) << " wins!";
    } else if (game.getBoard().isFull()) {
        info << "Draw: board is full.";
    } else {
        info << statusText;
    }

    info << "  |  Red removals: " << game.removalsLeft(Board::RED)
         << "  Yellow removals: " << game.removalsLeft(Board::YELLOW)
         << "  |  N: new, U: undo, Esc: quit";

    sf::Text text;
    text.setFont(font);
    text.setString(info.str());
    text.setCharacterSize(15);
    text.setFillColor(sf::Color::White);
    text.setPosition(BOARD_OFFSET_X + 260.0f, BOARD_OFFSET_Y + BOARD_PIXELS + 28.0f);
    window.draw(text);
}

void GUI::drawWinHighlight()
{
    const auto cells = game.getBoard().winningCells();
    for (const auto& [row, col] : cells) {
        sf::RectangleShape highlight(sf::Vector2f(CELL_SIZE - 2.0f, CELL_SIZE - 2.0f));
        highlight.setPosition(BOARD_OFFSET_X + col * CELL_SIZE + 1.0f,
                              BOARD_OFFSET_Y + row * CELL_SIZE + 1.0f);
        highlight.setFillColor(sf::Color::Transparent);
        highlight.setOutlineThickness(2.0f);
        highlight.setOutlineColor(sf::Color::White);
        window.draw(highlight);
    }
}

void GUI::drawButton(const sf::FloatRect& r, const std::string& label)
{
    sf::RectangleShape button(sf::Vector2f(r.width, r.height));
    button.setPosition(r.left, r.top);
    button.setFillColor(sf::Color(65, 85, 115));
    button.setOutlineColor(sf::Color(150, 170, 205));
    button.setOutlineThickness(1.0f);
    window.draw(button);

    if (!fontLoaded) {
        return;
    }

    sf::Text text;
    text.setFont(font);
    text.setString(label);
    text.setCharacterSize(16);
    text.setFillColor(sf::Color::White);
    const sf::FloatRect bounds = text.getLocalBounds();
    text.setPosition(r.left + (r.width - bounds.width) / 2.0f - bounds.left,
                     r.top + (r.height - bounds.height) / 2.0f - bounds.top - 2.0f);
    window.draw(text);
}

bool GUI::pixelToCell(int x, int y, int& row, int& col) const
{
    if (x < BOARD_OFFSET_X || y < BOARD_OFFSET_Y ||
        x >= BOARD_OFFSET_X + BOARD_PIXELS || y >= BOARD_OFFSET_Y + BOARD_PIXELS) {
        return false;
    }

    col = (x - BOARD_OFFSET_X) / CELL_SIZE;
    row = (y - BOARD_OFFSET_Y) / CELL_SIZE;
    return row >= 0 && row < Board::ROWS && col >= 0 && col < Board::COLS;
}

int GUI::pixelToColumn(int x) const
{
    if (x < BOARD_OFFSET_X || x >= BOARD_OFFSET_X + BOARD_PIXELS) {
        return -1;
    }
    return (x - BOARD_OFFSET_X) / CELL_SIZE;
}

void GUI::handleLeftClick(int x, int y)
{
    if (newGameBtn.contains(static_cast<float>(x), static_cast<float>(y))) {
        newGame();
        return;
    }
    if (undoBtn.contains(static_cast<float>(x), static_cast<float>(y))) {
        if (game.undoLast()) {
            if (game.currentPlayer() == Board::YELLOW) {
                game.undoLast();
            }
            statusText = "Undid the last turn.";
        }
        return;
    }

    if (game.isOver() || game.currentPlayer() != Board::RED) {
        return;
    }

    const int col = pixelToColumn(x);
    if (col < 0) {
        return;
    }

    if (!game.playDrop(col)) {
        statusText = "That column is full.";
        return;
    }

    statusText = "AI is thinking...";
    draw();
    doAIMove();
}

void GUI::handleRightClick(int x, int y)
{
    if (game.isOver() || game.currentPlayer() != Board::RED) {
        return;
    }

    int row = -1;
    int col = -1;
    if (!pixelToCell(x, y, row, col)) {
        return;
    }

    if (!game.playRemove(row, col)) {
        statusText = "Right click an opponent piece while removals remain.";
        return;
    }

    statusText = "AI is thinking...";
    draw();
    doAIMove();
}

void GUI::doAIMove()
{
    if (game.isOver() || game.currentPlayer() != Board::YELLOW) {
        return;
    }

    aiThinking = true;
    const Move move = ai.chooseMove(game);
    aiThinking = false;

    if (move.col < 0) {
        statusText = "AI has no legal move.";
        return;
    }

    if (move.isRemoval) {
        game.playRemove(move.row, move.col);
        statusText = "AI removed a red threat.";
    } else {
        game.playDrop(move.col);
        statusText = "Red to move.";
    }
}

void GUI::newGame()
{
    game.reset();
    statusText = "Red to move. Left click a column to drop; right click a yellow piece to remove.";
}
