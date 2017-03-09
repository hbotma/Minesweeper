/// TO DO LIST
/// ADD TIMER AND FLAG COUNT
///
///
///
///

#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctime>
#include <cstdlib>

class Tile : public sf::Drawable
{
public:
    Tile()
    {
        mine = false;
        flagged = false;
        exposed = false;
        cleared = false;
        green = false;
        number = 0;

        tileShade.setPrimitiveType(sf::Lines);
        tileShade.resize(24);
        sf::Color light(235, 235, 235), dark(100, 100, 100);
        for (int i = 0; i < 8; i++)
            tileShade[i].color = light;
        for (int i = 8; i < 24; i++)
            tileShade[i].color = dark;

        tileCover.setPrimitiveType(sf::Quads);
        tileCover.resize(4);
        sf::Color coverColor(200, 200, 200);
        for (int i = 0; i < 4; i++)
            tileCover[i].color = coverColor;
    }

    //////////////////////////////////////////////////

    void reset()
    {
        mine = false;
        flagged = false;
        exposed = false;
        cleared = false;
        green = false;
        number = 0;
    }

    void setPosition(sf::Vector2f const & position)
    {
        hitbox = sf::FloatRect(position + sf::Vector2f(-1,-1), sf::Vector2f(32, 32));
        tileSprite.setPosition(position);

        tileCover[0].position = tileCover[1].position = tileCover[2].position = tileCover[3].position = position;
        tileCover[1].position.x += 30;
        tileCover[3].position.y += 30;
        tileCover[2].position += sf::Vector2f(30, 30);

        for (int i = 0; i < 8; i++)
            tileShade[i].position = position;
        tileShade[1].position.x += 30;
        tileShade[3].position.y += 30;
        tileShade[4].position = tileShade[5].position = tileShade[6].position = tileShade[7].position += sf::Vector2f(1, 1);
        tileShade[5].position.x += 28;
        tileShade[7].position.y += 28;

        for (int i = 8; i < 24; i++)
            tileShade[i].position = position + sf::Vector2f(30, 30);
        tileShade[9].position.x -= 30;
        tileShade[11].position.y -= 30;
        tileShade[12].position = tileShade[13].position = tileShade[14].position = tileShade[15].position -= sf::Vector2f(1, 1);
        tileShade[13].position.x -= 28;
        tileShade[15].position.y -= 28;
        tileShade[16].position = tileShade[17].position = tileShade[18].position = tileShade[19].position += sf::Vector2f(1, 1);
        tileShade[17].position.x -= 32;
        tileShade[19].position.y -= 32;
        tileShade[20].position = tileShade[21].position = tileShade[22].position = tileShade[23].position += sf::Vector2f(2, 2);
        tileShade[21].position.x -= 34;
        tileShade[23].position.y -= 34;
    }

    void setSprite(sf::Texture texture[])
    {
        if (flagged)
            tileSprite.setTexture(texture[0]);
        else if (mine)
            tileSprite.setTexture(texture[9]);
        else
            tileSprite.setTexture(texture[number]);
    }

    void setMine()
    {
        mine = true;
    }

    void reverseFlag()
    {
        if (!exposed)
            flagged = !flagged;
    }

    void setGreen()
    {
        green = true;
    }

    bool isSafe()
    {
        if (flagged)
            return true;
        exposed = true;
        if (mine)
            return false;
        return true;
    }

    void clear()
    {
        cleared = true;
    }

    void incrNumber()
    {
        number++;
    }

    bool isMine() const
    {
        return mine;
    }

    bool isFlagged() const
    {
        return flagged;
    }

    bool isExposed() const
    {
        return exposed;
    }

    bool isGreen() const
    {
        return green;
    }

    bool needsClear() const
    {
        if (exposed && number == 0 && !cleared && !mine)
            return true;
        return false;
    }

    bool contains(sf::Vector2f const & point) const
    {
        if (hitbox.contains(point))
            return true;
        return false;
    }

    //////////////////////////////////////////////////

    void draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        if (exposed)
        {
            if (mine || number != 0)
                target.draw(tileSprite);
        }
        else
        {
            target.draw(tileCover);
            target.draw(tileShade);
            if (flagged)
                target.draw(tileSprite);
        }
    }

private:
    bool mine, flagged, exposed, cleared, green;
    int number;
    sf::FloatRect hitbox;
    sf::Sprite tileSprite;
    sf::VertexArray tileShade, tileCover;
};

void clearAround(Tile tile[], int i)
{
    if (i > 9)
        tile[i-10].isSafe();
    if (i < 90)
        tile[i+10].isSafe();
    if (i % 10 != 0)
    {
        if (i > 10)
            tile[i-11].isSafe();
        if (i > 0)
            tile[i-1].isSafe();
        if (i < 90)
            tile[i+9].isSafe();
    }
    if (i % 10 != 9)
    {
        if (i > 9)
            tile[i-9].isSafe();
        if (i < 99)
            tile[i+1].isSafe();
        if (i < 89)
            tile[i+11].isSafe();
    }
    tile[i].clear();
}

bool exposeAround(Tile tile[], int i)
{
    int gg = 0;
    if (i > 9)
        gg += !tile[i-10].isSafe();
    if (i < 90)
        gg += !tile[i+10].isSafe();
    if (i % 10 != 0)
    {
        if (i > 10)
            gg += !tile[i-11].isSafe();
        if (i > 0)
            gg += !tile[i-1].isSafe();
        if (i < 90)
            gg += !tile[i+9].isSafe();
    }
    if (i % 10 != 9)
    {
        if (i > 9)
            gg += !tile[i-9].isSafe();
        if (i < 99)
            gg += !tile[i+1].isSafe();
        if (i < 89)
            gg += !tile[i+11].isSafe();
    }
    if (gg > 0)
        return true;
    return false;
}

//////////////////////////////////////////////////
//////////////////////////////////////////////////
//////////////////////////////////////////////////

int main()
{
    sf::RenderWindow window(sf::VideoMode(320, 320), "Minesweeper", sf::Style::Close);
    sf::RectangleShape background(sf::Vector2f(320, 320)), overlay(sf::Vector2f(320,320)), redMine(sf::Vector2f(31, 31)),
                       greenMine(sf::Vector2f(31, 31));
    background.setFillColor(sf::Color(175, 175, 175));
    overlay.setFillColor(sf::Color(150, 150, 150, 150));
    redMine.setFillColor(sf::Color::Red);
    greenMine.setFillColor(sf::Color(10, 150, 30));
    sf::Texture tileTexture[10];
    sf::VertexArray grid(sf::Lines, 44);
    sf::Font font;
    sf::Text ggText;
    Tile tile[100];
    bool gameOver = false, firstClick = true;
    int firstTile = -1;
    srand(time(NULL));

    if (!tileTexture[0].loadFromFile("images/flag.png") ||
        !tileTexture[1].loadFromFile("images/1.png") ||
        !tileTexture[2].loadFromFile("images/2.png") ||
        !tileTexture[3].loadFromFile("images/3.png") ||
        !tileTexture[4].loadFromFile("images/4.png") ||
        !tileTexture[5].loadFromFile("images/5.png") ||
        !tileTexture[6].loadFromFile("images/6.png") ||
        !tileTexture[7].loadFromFile("images/7.png") ||
        !tileTexture[8].loadFromFile("images/8.png") ||
        !tileTexture[9].loadFromFile("images/mine.png") ||
        !font.loadFromFile("fonts/comic.ttf"))
    {
        std::cout << "Failed to load resource(s).\n\n";
        return 0;
    }

    ggText.setFont(font);
    ggText.setCharacterSize(40);
    ggText.setPosition(40, 130);
    ggText.setColor(sf::Color::Black);

    sf::Color gridColor = sf::Color(100, 100, 100);
    for (int i = 0; i < 11; i ++)
    {
        grid[2*i].position = sf::Vector2f(32*i, 0);
        grid[2*i+1].position = sf::Vector2f(32*i, 320);
        grid[2*i + 22].position = sf::Vector2f(0, 32*i);
        grid[2*i+1 + 22].position = sf::Vector2f(320, 32*i);
        grid[2*i+1 + 22].color = grid[2*i + 22].color = grid[2*i+1].color = grid[2*i].color = gridColor;
    }

    for (int i = 0; i < 100; i++)
        tile[i].setPosition(sf::Vector2f((i % 10) * 32 + 1, (i / 10) * 32 + 1));

    //////////////////////////////////////////////////
    //////////////////////////////////////////////////
    //////////////////////////////////////////////////

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (!gameOver)
            {
                if (event.type == sf::Event::MouseButtonPressed)
                {
                    if (event.mouseButton.button == sf::Mouse::Left)
                        for (int i = 0; i < 100; i++)
                        {
                            if (tile[i].contains(sf::Vector2f(event.mouseButton.x, event.mouseButton.y)))
                            {
                                if (firstClick)
                                {
                                    firstClick = false;
                                    firstTile = i;
                                }
                                if (!tile[i].isSafe())
                                {
                                    gameOver = true;
                                    redMine.setPosition(sf::Vector2f((i % 10) * 32 + 1, (i / 10) * 32 + 1));
                                }

                                break;
                            }
                        }
                    else if (event.mouseButton.button == sf::Mouse::Right)
                    {
                        for (int i = 0; i < 100; i++)
                            if (tile[i].contains(sf::Vector2f(event.mouseButton.x, event.mouseButton.y)))
                            {
                                tile[i].reverseFlag();
                                break;
                            }
                    }
                    else if (event.mouseButton.button == sf::Mouse::Middle)
                    {
                        for (int i = 0; i < 100; i++)
                            if (tile[i].contains(sf::Vector2f(event.mouseButton.x, event.mouseButton.y)))
                            {
                                if (tile[i].isExposed())
                                {
                                    gameOver = exposeAround(tile, i);
                                    redMine.setPosition(sf::Vector2f((i % 10) * 32 + 1, (i / 10) * 32 + 1));
                                }
                                break;
                            }
                    }
                }
            }
            else if (event.type == sf::Event::MouseButtonPressed &&
                     background.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
            {
                gameOver = false;
                firstClick = true;
                for (int i = 0; i < 100; i++)
                    tile[i].reset();
            }
        }

        if (firstTile >= 0)
        {
            for (int i = 0; i < 13; i++)
            {
                int mineIndex = 0;
                do
                {
                    mineIndex = rand() % 100;
                } while (tile[mineIndex].isMine() || mineIndex == firstTile);
                tile[mineIndex].setMine();
            }

            firstTile = -1;

            for (int i = 0; i < 100; i++)
            {
                if (!tile[i].isMine())
                {
                    if (i > 9 && tile[i-10].isMine())
                        tile[i].incrNumber();
                    if (i < 90 && tile[i+10].isMine())
                        tile[i].incrNumber();
                    if (i % 10 != 0)
                    {
                        if (i > 10 && tile[i-11].isMine())
                            tile[i].incrNumber();
                        if (i > 0 && tile[i-1].isMine())
                            tile[i].incrNumber();
                        if (i < 90 && tile[i+9].isMine())
                            tile[i].incrNumber();
                    }
                    if (i % 10 != 9)
                    {
                        if (i > 9 && tile[i-9].isMine())
                            tile[i].incrNumber();
                        if (i < 99 && tile[i+1].isMine())
                            tile[i].incrNumber();
                        if (i < 89 && tile[i+11].isMine())
                            tile[i].incrNumber();
                    }
                }
            }
        }

        bool allClear = false;
        while (!allClear)
        {
            allClear = true;
            for (int i = 0; i < 99; i++)
                if (tile[i].needsClear())
                {
                    clearAround(tile, i);
                    allClear = false;
                }
        }

        bool win = true;
        for (int i = 0; i < 100; i++)
            if (!tile[i].isExposed() && !tile[i].isMine())
                win = false;

        if (win)
        {
            gameOver = true;
            ggText.setString("You Win :D");
        }
        else
            ggText.setString("Game Over :(");

        window.clear();
        window.draw(background);
        window.draw(grid);
        if (gameOver && !win)
        {
            window.draw(redMine);
            for (int i = 0; i < 100; i++)
                if (tile[i].isMine())
                {
                    if (tile[i].isFlagged())
                    {
                        tile[i].reverseFlag();
                        tile[i].setGreen();
                    }
                    tile[i].isSafe();
                }
        }
        for (int i = 0; i < 100; i++)
        {
            if (tile[i].isGreen())
            {
                greenMine.setPosition(sf::Vector2f((i % 10) * 32 + 1, (i / 10) * 32 + 1));
                window.draw(greenMine);
            }
            tile[i].setSprite(tileTexture);
            window.draw(tile[i]);
        }
        if (gameOver)
        {
            window.draw(overlay);
            window.draw(ggText);
        }
        window.display();
    }

    return 0;
}
