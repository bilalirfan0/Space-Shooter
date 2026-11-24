#include <SFML/Graphics.hpp>  // Part 1 - Bilal Irfan. Asset loading, window, and player setup.
#include <iostream>
#include <string>
#include <vector>

using namespace sf;
using namespace std;

// These are the only two helpers; the game logic is all in main.
Text makeText(const Font& font, const string& value, unsigned size,
              float x, float y, bool centered = true)
{
    Text text(font, value, size);
    text.setPosition({x, y});
    if (centered)
    {
        const auto bounds = text.getGlobalBounds();
        text.setPosition({x + 100.f - bounds.size.x / 2.f,
                          y + 35.f - bounds.size.y / 1.5f});
    }
    return text;
}

bool collision(const Sprite& first, const Sprite& second)
{
    return first.getGlobalBounds().findIntersection(second.getGlobalBounds()).has_value();
}

int main()
{
    // Run from assets/ (run.cmd does this). Only artwork and fonts are loaded.
    Texture backgroundTexture, menuShipTexture, bulletTexture, bombTexture;
    Texture playerTextures[3], enemyTextures[3][3];
    Font menuFont, gameFont;
    if (!backgroundTexture.loadFromFile("img/abdullah.jpg") ||
        !menuShipTexture.loadFromFile("img/PNG/playerShip1_red.png") ||
        !bulletTexture.loadFromFile("img/enemy_laser.png") ||
        !bombTexture.loadFromFile("img/PNG/Lasers/laserBlue10.png") ||
        !menuFont.openFromFile("font2.otf") || !gameFont.openFromFile("font.otf"))
    {
        cerr << "Could not load artwork or fonts. Run run.cmd.\n";
        return 1;
    }

    const char* playerFiles[] = {"img/level1c.png", "img/level2c.png", "img/level3c.png"};
    const char* enemyFiles[3][3] = {
        {"img/PNG/Enemies/enemyGreen1.png", "img/PNG/Enemies/enemyBlue1.png", nullptr},
        {"img/PNG/Enemies/enemyBlack1.png", "img/PNG/Enemies/enemyBlue1.png",
         "img/PNG/Enemies/enemyGreen1.png"},
        {"img/enemy_3.png", "img/enemy_2.png", "img/enemy_1.png"}
    };
    for (int level = 0; level < 3; ++level)
    {
        if (!playerTextures[level].loadFromFile(playerFiles[level]))
            return 1;
        for (int row = 0; row < (level == 0 ? 2 : 3); ++row)
            if (!enemyTextures[level][row].loadFromFile(enemyFiles[level][row]))
                return 1;
    }

    Sprite background(backgroundTexture), menuShip(menuShipTexture), player(playerTextures[0]);
    background.setScale({0.75f, 1.2f});
    menuShip.setScale({0.8f, 0.8f});
    RenderWindow window(VideoMode({780, 780}), " MENU !!");
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);

    const auto setPlayerPosition = [&player, &window](Vector2f position)
    {
        const auto limit = window.getView().getSize() - player.getGlobalBounds().size;
        if (position.x < 0.f)
            position.x = 0.f;
        else if (position.x > limit.x)
            position.x = limit.x;
        if (position.y < 0.f)
            position.y = 0.f;
        else if (position.y > limit.y)
            position.y = limit.y;
        player.setPosition(position);
    };

    // Part 2 - Humail Tofeeq. Sprite containers and level settings. One enemy owns its bombs, so destroying it removes those bombs as before.
    struct Enemy
    {
        Sprite sprite;
        int row;
        bool alive;
        vector<Sprite> bombs;
    };
    vector<Enemy> enemies;
    vector<Sprite> bullets;
    enum Screen { Menu, Playing, Paused, Instructions, Leaderboard, Result };
    Screen screen = Menu, returnScreen = Menu, lastScreen = Result;
    const char* screenTitles[] = {
        " MENU !!", "", " GAME PAUSED !!!!", " INSTRUCTIONS!!", " LEADERBOARD !!", ""
    };
    const char* gameTitles[] = {
        "GAME STARTED !", "GAME STARTED : LEVEL 2 !", "GAME STARTED : LEVEL 3 !"
    };
    const int startingScores[] = {0, 150, 330}, targetScores[] = {150, 330, 630};
    const int rowScores[] = {30, 20, 10};
    const float bombIntervals[] = {2.f, 3.f, 5.f};
    const float firstX[3][3] = {{200.f, 60.f, 0.f}, {50.f, 160.f, 230.f}, {50.f, 60.f, 90.f}};
    const float spacing[3][3] = {{145.f, 300.f, 0.f}, {300.f, 200.f, 130.f}, {150.f, 150.f, 130.f}};
    int currentLevel = 1, score = 0, lives = 3, previousScore = -1; 
     // Part 3 - Abdullah Bilal. Game and firing state, timing, and event handling.
    bool newLevel = false, canFire = true, won = false;
    float bombTimers[3] = {};
    Clock clock;

    while (window.isOpen())
    {
        const float elapsed = clock.restart().asSeconds();
        while (const auto event = window.pollEvent())
        {
            if (event->is<Event::Closed>())
            {
                window.close();
                break;
            }
            const auto* released = event->getIf<Event::KeyReleased>();
            if (released && released->code == Keyboard::Key::Space)
                canFire = true;
            const auto* pressed = event->getIf<Event::KeyPressed>();
            if (!pressed)
                continue;
            const auto key = pressed->code;

            if (screen == Menu)
            {
                if (key == Keyboard::Key::S || key == Keyboard::Key::Num2 || key == Keyboard::Key::Num3)
                {
                    currentLevel = key == Keyboard::Key::S ? 1 : (key == Keyboard::Key::Num2 ? 2 : 3);
                    newLevel = true;
                    screen = Playing;
                }
                else if (key == Keyboard::Key::I)
                    screen = Instructions;
                else if (key == Keyboard::Key::L)
                {
                    returnScreen = Menu;
                    screen = Leaderboard;
                }
                else if (key == Keyboard::Key::Q)
                    window.close();
            }
            else if (screen == Playing && key == Keyboard::Key::Escape)
                screen = Paused;
            else if (screen == Paused && (key == Keyboard::Key::P || key == Keyboard::Key::Escape))
            {
                screen = Playing;
                canFire = !Keyboard::isKeyPressed(Keyboard::Key::Space);
            }
            else if (screen == Instructions && key == Keyboard::Key::Escape)
                screen = Menu;
            else if (screen == Leaderboard && key == Keyboard::Key::Escape)
                screen = returnScreen;
            else if (screen == Result)
            {
                if (key == Keyboard::Key::Q)
                    screen = Menu;
                else if (key == Keyboard::Key::L)
                {
                    returnScreen = Result;
                    screen = Leaderboard;
                }
            }
        }
        if (!window.isOpen())
            break;

        // Part 4 - Bilal Irfan. Player reset, movement, and progression. This block handles both a fresh game and advancing to the next level.
        if (newLevel)
        {
            score = startingScores[currentLevel - 1];
            lives = 3;
            canFire = true;
            bullets.clear();
            enemies.clear();
            player.setTexture(playerTextures[currentLevel - 1], true);
            const float scale = currentLevel == 1 ? 0.2f : 0.3f;
            player.setScale({scale, scale});
            setPlayerPosition({340.f, 700.f});
            for (float& timer : bombTimers)
                timer = 0.f;
            const int rows = currentLevel == 1 ? 2 : 3, columns = currentLevel == 3 ? 5 : 3; 
             // Part 2 continued. Build the enemy formation. HOUUUUUUUUUUUUUUUUUUUUUUUUUUUMAIL
            for (int row = 0; row < rows; ++row)
                for (int column = 0; column < columns; ++column)
                {
                    Sprite enemy(enemyTextures[currentLevel - 1][row]);
                    enemy.setScale({0.6f, 0.6f});
                    enemy.setPosition({firstX[currentLevel - 1][row] + spacing[currentLevel - 1][row] * column,
                                       70.f + 100.f * row});
                    enemies.push_back({enemy, row, true, {}});
                }
            window.setTitle(gameTitles[currentLevel - 1]); 
             // Part 4 continued. Finish the level reset and handle arrow-key movement. BILAL
            newLevel = false;
            clock.restart();
            continue;
        }

        if (screen == Playing)
        {
            // Keep the original movement speeds and one shot per Space press.
            if (Keyboard::isKeyPressed(Keyboard::Key::Left))  player.move({-0.3f, 0.f});
            if (Keyboard::isKeyPressed(Keyboard::Key::Right)) player.move({0.3f, 0.f});
            if (Keyboard::isKeyPressed(Keyboard::Key::Up))    player.move({0.f, -0.3f});
            if (Keyboard::isKeyPressed(Keyboard::Key::Down))  player.move({0.f, 0.3f});
            setPlayerPosition(player.getPosition());
            if (Keyboard::isKeyPressed(Keyboard::Key::Space) && canFire)  
            // Part 5 - Abdullah Bilal. Firing, hit detection, and projectile movement.
            {
                Sprite bullet(bulletTexture);
                const auto playerBounds = player.getGlobalBounds();
                bullet.setPosition({playerBounds.getCenter().x - bullet.getLocalBounds().getCenter().x,
                                    playerBounds.position.y});
                bullets.push_back(bullet);
                canFire = false;
            }
            for (int row = 0; row < 3; ++row)  // Part 2 continued. Spawn enemy bombs by row. huuumail
            {
                bombTimers[row] += elapsed;
                if (bombTimers[row] < bombIntervals[row])
                    continue;
                for (auto& enemy : enemies)
                    if (enemy.alive && enemy.row == row)
                    {
                        Sprite bomb(bombTexture);
                        bomb.setPosition(enemy.sprite.getPosition());
                        enemy.bombs.push_back(bomb);
                    }
                bombTimers[row] = 0.f;
            }
            for (const auto& enemy : enemies)  // Part 5 continued. Check collisions with the player and bullets. abd
                if (lives > 0 && enemy.alive && collision(enemy.sprite, player))
                {
                    --lives;
                    setPlayerPosition({340.f, 700.f});
                }
            for (auto& enemy : enemies)
                for (size_t i = 0; enemy.alive && lives > 0 && i < bullets.size(); ++i)
                    if (collision(bullets[i], enemy.sprite))
                    {
                        bullets.erase(bullets.begin() + i);
                        enemy.alive = false;
                        enemy.bombs.clear();
                        score += rowScores[enemy.row];
                        break;
                    }

            if (lives > 0 && score >= targetScores[currentLevel - 1] && currentLevel < 3)  // Part 4 continued. Advance levels, update lives, and record the result. bilal
            {
                ++currentLevel;
                newLevel = true;
                continue;
            }
            if (lives > 0 && score < targetScores[currentLevel - 1])
                for (auto& enemy : enemies)
                    for (size_t i = 0; i < enemy.bombs.size() && lives > 0;)
                    {
                        if (collision(enemy.bombs[i], player))
                        {
                            enemy.bombs.erase(enemy.bombs.begin() + i);
                            --lives;
                            setPlayerPosition({340.f, 700.f});
                        }
                        else
                            ++i;
                    }
            if (lives == 0 || (currentLevel == 3 && score >= targetScores[2]))
            {
                won = lives > 0;
                previousScore = score; // Session only: no score files are read or written.
                screen = Result;
            }
            else
            {
                for (size_t i = 0; i < bullets.size();)  // Part 5 continued. Move projectiles and remove them off-screen. abd
                {
                    bullets[i].move({0.f, -0.75f});
                    if (bullets[i].getPosition().y < 0.f)
                        bullets.erase(bullets.begin() + i);
                    else
                        ++i;
                }
                for (auto& enemy : enemies)
                    for (size_t i = 0; i < enemy.bombs.size();)
                    {
                        enemy.bombs[i].move({0.f, 0.1f});
                        if (enemy.bombs[i].getPosition().y > 780.f)
                            enemy.bombs.erase(enemy.bombs.begin() + i);
                        else
                            ++i;
                    }
            }
        }

        // Part 6 - Humail Tofeeq. Draw the scene, HUD, and menu screens. All screens share one window and background instead of separate loops.
        if (screen != lastScreen)
        {
            window.setTitle(screen == Playing ? gameTitles[currentLevel - 1] :
                            screen == Result ? (won ? " YOU WON !!!!" : " GAME OVER !!!!") : screenTitles[screen]);
            window.setFramerateLimit(screen == Playing ? 0 : 60);
            lastScreen = screen;
        }
        window.clear(Color::Black);
        window.draw(background);
        if (screen == Playing)
        {
            window.draw(player);
            window.draw(makeText(gameFont, "Score: " + to_string(score), 24, 10.f, 10.f, false));
            window.draw(makeText(gameFont, "Lives: " + to_string(lives), 24, 650.f, 10.f, false));
            for (const auto& enemy : enemies)
                for (const auto& bomb : enemy.bombs)
                    window.draw(bomb);
            for (const auto& bullet : bullets)
                window.draw(bullet);
            for (const auto& enemy : enemies)
                if (enemy.alive)
                    window.draw(enemy.sprite);
        }
        else
        {
            menuShip.setPosition({50.f, screen == Paused || screen == Result ? 300.f : 70.f});
            window.draw(menuShip);
            if (screen == Menu || screen == Instructions || screen == Leaderboard)
                window.draw(makeText(menuFont, "SPACE SHOOTER ", 80, 300.f, 50.f));
            if (screen == Menu)
            {
                const char* items[] = {"Start Game", "Leaderboard", "Quit Game", "Level 2", "Level 3", "Instructions"};
                for (int i = 0; i < 6; ++i)
                    window.draw(makeText(menuFont, items[i], 30, 300.f, 180.f + 100.f * i));
            }
            else if (screen == Instructions)
            {
                window.draw(makeText(menuFont,
                    "Instructions : \n 1. Press Spacebar to fire.\n2. dodge enemy bombs.\n"
                    "3.There are three levels, beat all three to win.\n"
                    "4.Upon Collision , you will lose one life .\n"
                    "5. if you decrease to 0 lives , you will lose .\nEnjoy :)", 30, 300.f, 350.f));
                window.draw(makeText(menuFont, "Arrow keys: move    Escape: pause / return", 22, 300.f, 680.f));
            }
            else if (screen == Leaderboard)
            {
                window.draw(makeText(menuFont, previousScore < 0 ? "Previous Score : No scores yet" :
                                     "Previous Score : " + to_string(previousScore), 30, 300.f, 350.f));
                window.draw(makeText(menuFont, "Escape: return", 22, 300.f, 680.f));
            }
            else if (screen == Paused)
            {
                window.draw(makeText(menuFont, "GAME PAUSED !!! ", 80, 300.f, 280.f));
                window.draw(makeText(menuFont, "P: resume", 22, 300.f, 680.f));
            }
            else if (screen == Result)
            {
                window.draw(makeText(menuFont, won ? "YOU WON !!! " : "GAME OVER !!! ", 80, 300.f, 280.f));
                window.draw(makeText(menuFont, "L: leaderboard    Q: return to menu", 22, 300.f, 680.f));
            }
        }
        window.display();
    }
    return 0;
}
