
#include <iostream>
#include <Windows.h>
#include <chrono>
#include <string>


int main()
{
    int screenWidth = 120;
    int screenHeight = 40;

    float playerX = 14.0f;
    float playerY = 1.0f;
    float playerAngle = 3.14f;

    float FOV = 3.14159f / 4.0f; // 90 degrees

    int mapHeight = 16;
    int mapWidth = 16;
    wchar_t* screen = new wchar_t[screenWidth * screenHeight];
    HANDLE console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(console);
    DWORD bytesWritten = 0;

    std::wstring map;
    
    map += L"################";
    map += L"#              #";
    map += L"#  #############";
    map += L"#       #   #  #";
    map += L"######  # # #  #";
    map += L"#       # # #  #";
    map += L"##### # # # #  #";
    map += L"#   # #     #  #";
    map += L"# # # ##### #  #";
    map += L"# ### #     #  #";
    map += L"#     #  ## #  #";
    map += L"########  # #  #";
    map += L"#         #    #";
    map += L"#  #      #    #";
    map += L"#  #           #";
    map += L"####     #######";

    float anglePerColumn = FOV / static_cast<float>(screenWidth);

    float STEP_SIZE = 0.1f;
    float ROTATION_SPEED = 2.0f;
    float WALK_SPEED = 2.0f;

    auto oldTime = std::chrono::system_clock::now();

    while (true)
    {
        float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - oldTime).count() / 1000000.0f;
        oldTime = std::chrono::system_clock::now();
        // For each column
            // Ray cast out
            // Check distance
            // Depending on distance, fill part of screen

        if (GetAsyncKeyState((unsigned short)'A') && 0x8000)
        {
            playerAngle -= ROTATION_SPEED * deltaTime;
        }

        if (GetAsyncKeyState((unsigned short)'D') && 0x8000)
        {
            playerAngle += ROTATION_SPEED * deltaTime;
        }

        if (GetAsyncKeyState((unsigned short)'R') && 0x8000)
        {
            playerX = 14.0f;
            playerY = 1.0f;
            playerAngle = 3.14f;
        }

        if (GetAsyncKeyState((unsigned short)'W') && 0x8000)
        {
            float oldX = playerX;
            float oldY = playerY;

            playerX += cos(playerAngle) * WALK_SPEED * deltaTime;
            playerY += sin(playerAngle) * WALK_SPEED * deltaTime;

            if (playerX >= 0 && playerY >= 0 && static_cast<int>(playerX) < mapWidth && static_cast<int>(playerY) < mapHeight)
            {
                if (map[static_cast<int>(playerX) + mapWidth * static_cast<int>(playerY)] == '#')
                {
                    playerX = oldX;
                    playerY = oldY;
                }
            }
        }

        if (GetAsyncKeyState((unsigned short)'S') && 0x8000)
        {
            float oldX = playerX;
            float oldY = playerY;

            playerX -= cos(playerAngle) * WALK_SPEED * deltaTime;
            playerY -= sin(playerAngle) * WALK_SPEED * deltaTime;

            if (playerX >= 0 && playerY >= 0 && static_cast<int>(playerX) < mapWidth && static_cast<int>(playerY) < mapHeight)
            {
                if (map[static_cast<int>(playerX) + mapWidth * static_cast<int>(playerY)] == '#')
                {
                    playerX = oldX;
                    playerY = oldY;
                }
            }
        }

        for (int i = 0; i < screenWidth; i++)
        {
            bool hitWall = false;
            bool isFlag = false;
            // Get the distance to the wall for the current column of the console
            float currentAngle = (playerAngle - (0.5f * FOV)) + i / static_cast<float>(screenWidth) * FOV;
            float distance = 0.0f;
            for (; distance < mapWidth; distance += STEP_SIZE)
            {
                int currX = static_cast<int>(cos(currentAngle) * distance + playerX);
                int currY = static_cast<int>(sin(currentAngle) * distance + playerY);

                if (currX < 0 || currY < 0 || currX >= mapWidth || currY >= mapHeight)
                {
                    distance = 16.0f;
                    break;
                }

                if (map[currX + currY * mapWidth] == '#')
                {
                    hitWall = true;
                    break;
                }

                if (map[currX + currY * mapWidth] == 'F')
                {
                    isFlag = true;
                    break;
                }
            }

            // Add to the buffer
            int ceiling = static_cast<int>(0.5 * screenHeight - screenHeight / static_cast<float>(distance));
            int floor = screenHeight - ceiling;

            for (int y = 0; y < screenHeight; y++)
            {
                short shade = ' ';

                if (isFlag)
                {
                    shade = '\u2590';
                }
                // Wall stuff
                else if (y > ceiling && y <= floor) {
                    if (distance <= static_cast<float>(mapWidth) / 6.0f)
                        shade = L'\u2588';
                    else if (distance <= static_cast<float>(mapWidth) / 4.5f)
                        shade = L'\u2593';
                    else if (distance <= static_cast<float>(mapWidth) / 2.0f)
                        shade = L'\u2592';
                    else if (hitWall) 
                    {
                        shade = L'\u2591';
                    }
                    else 
                    {
                        shade = ' ';
                    }
                }
                // Floor stuff
                else if (y > floor)
                {
                    if (y > screenHeight - floor / 4)
                        shade = '+';
                    else if (y > screenHeight - floor / 3)
                        shade = '=';
                    else if (y > screenHeight - floor / 2)
                        shade = '-';
                    else
                    {
                        shade = '.';
                    }
                }
                else 
                {
                    shade = ' ';
                }
                screen[y * screenWidth + i] = shade;
            }
        }

        //// Add minimap
        for (int y = 0; y < mapHeight; y++)
        {
            for (int x = 0; x < mapWidth; x++)
            {
                screen[x + (y + 1)  * screenWidth] = map[x + (y * mapWidth)];
            }
        }

        screen[static_cast<int>(playerX) + static_cast<int>(playerY + 1) * screenWidth] = '@';

        swprintf_s(screen, 40, L"A=%3.2f X=%3.2f Y=%3.2f fr=%3.0f", playerAngle, playerX, playerY, 60.0f / deltaTime);
        

        screen[screenWidth * screenHeight - 1] = '\0';
        WriteConsoleOutputCharacter(console, screen, screenWidth * screenHeight, { 0, 0 }, &bytesWritten);
    }

    return 0;
}

