#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "raylib.h"

#include "timer.h"

#define screenWidth (800)
#define screenHeight (600)
#define tileCountX (7)
#define tileCountY (5)
#define xMargin (10)
#define yMargin (10)
#define levelYOffset (20)
#define ballRadius (5)
#define tileWidth ((screenWidth - xMargin*2) / tileCountX)
#define tileHeight ((screenHeight / 2 - yMargin - levelYOffset) / tileCountY)
static const Color tileColor[5] = { RED, GREEN, BLUE };

typedef struct Game
{
    Rectangle paddle;
    Vector2 ball;
    Vector2 ballVelocity;
    Timer movementTimer;
    int tiles[tileCountX][tileCountY];
    bool started;
    int score;
    bool gameOver;
} Game;

bool CheckBallCollisionRec(Vector2 center, float radius, Rectangle rec,
                           Vector2 *velocity)
{
    if (CheckCollisionCircleRec(center, radius, rec))
    {
        if (rec.x >= center.x)
        {
            velocity->x = -velocity->x;
        }
        else if (rec.x+rec.width <= center.x)
        {
            velocity->x = -velocity->x;
        }
        else if (rec.y >= center.y)
        {
            velocity->y = -velocity->y;
        }
        else if (rec.y+rec.height <= center.y)
        {
            velocity->y = -velocity->y;
        }

        return true;
    }

    return false;
}

void GameInitialize(Game *game)
{
    memset(game, 0, sizeof(*game));

    game->paddle.width = 50;
    game->paddle.height = 10;
    game->paddle.x = screenWidth / 2 - game->paddle.width / 2;
    game->paddle.y = screenHeight - yMargin - game->paddle.height;

    game->ball.x = screenWidth / 2;
    game->ball.y = screenHeight - screenHeight / 4;

    game->ballVelocity.x = 100;
    game->ballVelocity.y = -100;

    TimerStart(&game->movementTimer);

    for (int i = 0; i < tileCountX; ++i)
    {
        for (int j = 0; j < tileCountY; ++j)
        {
            game->tiles[i][j] = GetRandomValue(1, 3);
        }
    }
}

Rectangle GetTileRect(int tx, int ty)
{
    Rectangle tile;

    tile.x = xMargin + tx * tileWidth;
    tile.y = yMargin + levelYOffset + ty * tileHeight;
    tile.width = tileWidth;
    tile.height = tileHeight;

    return tile;
}

void GameHandleKeys(Game *game, int64_t elapsed)
{
    if (!game->gameOver)
    {
        if (IsKeyDown(KEY_RIGHT))
        {
            game->paddle.x += 160 * elapsed / 1000 / 1000;
            if (!game->started)
                game->started = true;
        }
        else if (IsKeyDown(KEY_LEFT))
        {
            game->paddle.x -= 160 * elapsed / 1000 / 1000;
            if (!game->started)
                game->started = true;
        }

        if (game->paddle.x < 0)
            game->paddle.x = 0;
        else if (game->paddle.x > screenWidth - game->paddle.width)
            game->paddle.x = screenWidth - game->paddle.width;
    }
    else
    {
        if (IsKeyPressed(KEY_SPACE))
        {
            GameInitialize(game);
        }
    }
}

void GameHandleBallTileCollisions(Game *game)
{
    for (int i = 0; i < tileCountX; ++i)
    {
        for (int j = 0; j < tileCountY; ++j)
        {
            Rectangle tileRect = GetTileRect(i, j);

            if (!game->tiles[i][j])
                continue;

            if (CheckBallCollisionRec(game->ball, 5, tileRect,
                &game->ballVelocity))
            {
                game->tiles[i][j] -= 1;
                game->score++;
                if (game->tiles[i][j] == 0)
                    game->score++;
                return;
            }
        }
    }
}

void GameHandleBallPaddleCollision(Game *game)
{
    CheckBallCollisionRec(game->ball, 5, game->paddle, &game->ballVelocity);
}

void GameHandleBallScreenCollision(Game *game)
{
    if (game->ball.x+5 >= screenWidth)
    {
        game->ball.x = screenWidth-5;
        game->ballVelocity.x = -game->ballVelocity.x;
    }
    else if (game->ball.x-5 <= 0)
    {
        game->ball.x = 5;
        game->ballVelocity.x = -game->ballVelocity.x;
    }

    if (game->ball.y-5 <= 0)
    {
        game->ball.y = 5;
        game->ballVelocity.y = -game->ballVelocity.y;
    }
    else if (game->ball.y+5 >= screenHeight)
    {
        game->gameOver = true;
    }
}

void GameUpdate(Game *game)
{
    int64_t elapsed = TimerElapsedMicro(&game->movementTimer);

    GameHandleKeys(game, elapsed);

    if (!game->gameOver)
    {
        if (game->started)
        {
            game->ball.x += game->ballVelocity.x * elapsed / 1000 / 1000;
            game->ball.y += game->ballVelocity.y * elapsed / 1000 / 1000;

            GameHandleBallScreenCollision(game);
            GameHandleBallPaddleCollision(game);
            GameHandleBallTileCollisions(game);
        }
    }

    TimerStart(&game->movementTimer);
}

void DrawTextCentered(const char *text, int width, int y, int size, Color color)
{
    int textWidth = MeasureText(text, size);
    DrawText(text, width/2 - textWidth/2, y, size, color);
}

void GameDraw(Game *game)
{
    BeginDrawing();

    ClearBackground(RAYWHITE);

    DrawRectangleRec(game->paddle, BLACK);

    DrawCircle(game->ball.x, game->ball.y, 5, BLACK);

    for (int i = 0; i < tileCountX; ++i)
    {
        for (int j = 0; j < tileCountY; ++j)
        {
            Rectangle tileRect = GetTileRect(i, j);
            Color color = tileColor[game->tiles[i][j]-1];
            DrawRectangleRec(tileRect, color);
        }
    }

    DrawText(FormatText("SCORE: %i", game->score), xMargin, yMargin, 20,
        MAROON);

    if (!game->started)
    {
        DrawTextCentered("START!", screenWidth, screenHeight/2, 40, BLACK);
    }

    if (game->gameOver)
    {
        DrawTextCentered("GAME OVER!", screenWidth, screenHeight/2, 40, BLACK);
        DrawTextCentered("Press SPACE to try again.", screenWidth,
            screenHeight/2 + 40, 20, BLACK);
    }

    EndDrawing();
}

int main(int argc, char **argv)
{
    Game game;

    srand(time(0));

    TimerInit();

    GameInitialize(&game);

    InitWindow(screenWidth, screenHeight, "Breakout");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        GameUpdate(&game);
        GameDraw(&game);
    }

    CloseWindow();

    return 0;
}
