// GitHub: https://github.com/baibhav3211/Tetris-Game-Arduino
// Code for the game Tetris on an Arduino with a 109 LED matrix
// The game is controlled by 4 buttons, 2 for movement, 1 for rotation and 1 for speeding up the piece
// The game is played on a 10x14 grid, with the top 4 rows hidden from the player
// Coded by: Robotics Society NIT Hamirpur


#include <Adafruit_NeoPixel.h>
#include <avr/eeprom.h>
#include <EEPROM.h>

int ledPin = 12;
int numPixels = 109;
const int arenaX = 10;
const int arenaY = 14;
int arena[arenaY][arenaX] = {0};
int downTime = 500;
int rightButton = 6;
int leftButton = 5;
int downButton = 4;
int rotButton = 7;
int moveTime = 200;
int score = 0;
int animationTime = 100;
int piece = 0;
int color = 1;
int level = 1;
int highscore=0;
Adafruit_NeoPixel pixels(numPixels, ledPin, NEO_GRB + NEO_KHZ800);

//////////////////////////////////// random shit



/*==============================================================================
  Call reseedRandom once in setup to start random on a new sequence.  Uses
  four bytes of EEPROM.
==============================================================================*/

void reseedRandom(uint32_t *address)
{
    static const uint32_t HappyPrime = 127807;
    uint32_t raw;
    unsigned long seed;

    // Read the previous raw value from EEPROM
    raw = eeprom_read_dword(address);

    // Loop until a seed within the valid range is found
    do
    {
        // Incrementing by a prime (except 2) every possible raw value is visited
        raw += HappyPrime;

        // Park-Miller is only 31 bits so ignore the most significant bit
        seed = raw & 0x7FFFFFFF;
    } while ((seed < 1) || (seed > 2147483646));

    // Seed the random number generator with the next value in the sequence
    srandom(seed);

    // Save the new raw value for next time
    eeprom_write_dword(address, raw);
}

inline void reseedRandom(unsigned short address)
{
    reseedRandom((uint32_t *)(address));
}

/*==============================================================================
  So the reseedRandom raw value can be initialized allowing different
  applications or instances to have different random sequences.

  Generate initial raw values...

  https://www.random.org/cgi-bin/randbyte?nbytes=4&format=h
  https://www.fourmilab.ch/cgi-bin/Hotbits?nbytes=4&fmt=c&npass=1&lpass=8&pwtype=3

==============================================================================*/

void reseedRandomInit(uint32_t *address, uint32_t value)
{
    eeprom_write_dword(address, value);
}

inline void reseedRandomInit(unsigned short address, uint32_t value)
{
    reseedRandomInit((uint32_t *)(address), value);
}

uint32_t reseedRandomSeed EEMEM = 0xFFFFFFFF;

//////////////////////////////////

void colorSetter(int pixel, int colorCode)
{
    switch (colorCode)
    {
    case 0:
        pixels.setPixelColor(pixel, pixels.Color(0, 0, 0));
        break;
    case 1:
        pixels.setPixelColor(pixel, pixels.Color(0, 255, 159));
        break;
    case 2:
        pixels.setPixelColor(pixel, pixels.Color(0, 184, 255));
        break;
    case 3:
        pixels.setPixelColor(pixel, pixels.Color(0, 30, 255));
        break;
    case 4:
        pixels.setPixelColor(pixel, pixels.Color(189, 0, 255));
        break;
    case 5:
        pixels.setPixelColor(pixel, pixels.Color(214, 0, 255));
        break;
    }
}

void printScreen()
{

    for (int i = 4; i < arenaY; ++i)
    {
        int offset = i - 4;
        for (int j = 0; j < arenaX; ++j)
        {
            int pixel;
            if (i % 2 == 0)
                pixel = offset * arenaX + j + offset;
            else
                pixel = offset * arenaX + (arenaX - j - 1) + offset;
            
            if(arena[i][j] == 0)
                colorSetter(pixel, 0);
            else
                colorSetter(pixel, color);
                
            
        }
    }
    pixels.show();
}

///////////////////// Point

class Point
{
public:
    int x = 0;
    int y = 0;
    Point addPoint(int x_add, int y_add);
    void setPoint(int x_new, int y_new);
    bool checkPoint();
};

Point Point::addPoint(int x_add, int y_add)
{
    Point temp;
    temp.x += x + x_add;
    temp.y += y + y_add;
    return temp;
}
void Point::setPoint(int x_new, int y_new)
{
    x = x_new;
    y = y_new;
}

bool Point::checkPoint()
{
    if (x >= 0 && x < arenaX && y >= 0 && y < arenaY && arena[y][x] == 0)
        return true;
    return false;
}
/////////////////////////////// square

class Square
{
private:
    Point blocks[4];
    int color = 1;
    void setSquare();

public:
    Point pivot;
    int state;
    void spawn();
    void remove();
    bool canGoDown();
    void goDown();
    void goLeft();
    void goRight();
    // 01
    // 23
};

void Square::setSquare()
{
    blocks[0] = pivot;
    blocks[1] = pivot.addPoint(1, 0);
    blocks[2] = pivot.addPoint(0, 1);
    blocks[3] = pivot.addPoint(1, 1);
}

void Square::spawn()
{
    setSquare();
    for (int i = 0; i < 4; ++i)
        arena[blocks[i].y][blocks[i].x] = 1;
}
void Square::remove()
{
    setSquare();
    for (int i = 0; i < 4; ++i)
        arena[blocks[i].y][blocks[i].x] = 0;
}

bool Square::canGoDown()
{
    bool checkBlock[4] = {false, false, true, true};
    for (int i = 0; i < 4; ++i)
    {
        if (checkBlock[i])
        {
            Point temp = blocks[i].addPoint(0, 1);
            if (!temp.checkPoint())
                return false;
        }
    }
    return true;
}

void Square::goDown()
{
    remove();
    pivot = pivot.addPoint(0, 1);
    spawn();
}

void Square::goLeft()
{
    bool possible = true;
    bool checkBlock[4] = {true, false, true, false};
    for (int i = 0; i < 4; ++i)
    {
        if (checkBlock[i])
        {
            Point temp = blocks[i].addPoint(-1, 0);
            if (!temp.checkPoint())
                possible = false;
        }
    }
    if (possible)
    {
        remove();
        pivot = pivot.addPoint(-1, 0);
        spawn();
    }
}
void Square::goRight()
{
    bool possible = true;
    bool checkBlock[4] = {false, true, false, true};
    for (int i = 0; i < 4; ++i)
    {
        if (checkBlock[i])
        {
            Point temp = blocks[i].addPoint(1, 0);
            if (!temp.checkPoint())
                possible = false;
        }
    }
    if (possible)
    {
        remove();
        pivot = pivot.addPoint(1, 0);
        spawn();
    }
}

////////////////////////////////////////////////// IBlock
class IBlock
{
private:
    Point blocks[4];
    int color = 2;
    void setIBlock();

public:
    Point pivot;
    int state;
    void spawn();
    void remove();
    bool canGoDown();
    void goDown();
    void goLeft();
    void goRight();
    void rotate();
    // state 0
    // 0123
    // state 1
    // 0
    // 1
    // 2
    // 3
};
void IBlock::setIBlock()
{
    switch (state % 2)
    {
    case 0:
        blocks[0] = pivot;
        blocks[1] = pivot.addPoint(1, 0);
        blocks[2] = pivot.addPoint(2, 0);
        blocks[3] = pivot.addPoint(3, 0);
        break;
    case 1:
        blocks[0] = pivot;
        blocks[1] = pivot.addPoint(0, 1);
        blocks[2] = pivot.addPoint(0, 2);
        blocks[3] = pivot.addPoint(0, 3);
        break;
    default:
        break;
    }
}
void IBlock::spawn()
{
    setIBlock();
    for (int i = 0; i < 4; ++i)
        arena[blocks[i].y][blocks[i].x] = 1;
}
void IBlock::remove()
{
    setIBlock();
    for (int i = 0; i < 4; ++i)
        arena[blocks[i].y][blocks[i].x] = 0;
}
bool IBlock::canGoDown()
{
    bool checkBlock[4];
    switch (state % 2)
    {
    case 0:
        checkBlock[0] = true;
        checkBlock[1] = true;
        checkBlock[2] = true;
        checkBlock[3] = true;
        break;
    case 1:
        checkBlock[0] = false;
        checkBlock[1] = false;
        checkBlock[2] = false;
        checkBlock[3] = true;
        break;
    default:
        break;
    }
    for (int i = 0; i < 4; ++i)
    {
        if (checkBlock[i])
        {
            Point temp = blocks[i].addPoint(0, 1);
            if (!temp.checkPoint())
                return false;
        }
    }
    return true;
}
void IBlock::goDown()
{
    remove();
    pivot = pivot.addPoint(0, 1);
    spawn();
}
void IBlock::goLeft()
{
    bool possible = true;
    bool checkBlock[4];
    switch (state % 2)
    {
    case 0:
        checkBlock[0] = true;
        checkBlock[1] = false;
        checkBlock[2] = false;
        checkBlock[3] = false;
        break;
    case 1:
        checkBlock[0] = true;
        checkBlock[1] = true;
        checkBlock[2] = true;
        checkBlock[3] = true;
        break;
    default:
        break;
    }
    for (int i = 0; i < 4; ++i)
    {
        if (checkBlock[i])
        {
            Point temp = blocks[i].addPoint(-1, 0);
            if (!temp.checkPoint())
                possible = false;
        }
    }
    if (possible)
    {
        remove();
        pivot = pivot.addPoint(-1, 0);
        spawn();
    }
}
void IBlock::goRight()
{
    bool possible = true;
    bool checkBlock[4];
    switch (state % 2)
    {
    case 0:
        checkBlock[0] = false;
        checkBlock[1] = false;
        checkBlock[2] = false;
        checkBlock[3] = true;
        break;
    case 1:
        checkBlock[0] = true;
        checkBlock[1] = true;
        checkBlock[2] = true;
        checkBlock[3] = true;
        break;
    default:
        break;
    }
    for (int i = 0; i < 4; ++i)
    {
        if (checkBlock[i])
        {
            Point temp = blocks[i].addPoint(1, 0);
            if (!temp.checkPoint())
                possible = false;
        }
    }
    if (possible)
    {
        remove();
        pivot = pivot.addPoint(1, 0);
        spawn();
    }
}
void IBlock::rotate()
{
    bool possible = true;
    switch (state % 2)
    {
    case 0:
        for (int i = 1; i <= 3; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                Point temp = pivot.addPoint(j, i);
                if (!temp.checkPoint())
                    possible = false;
            }
        }
        if (possible)
        {
            remove();
            state += 1;
            spawn();
        }
        break;
    case 1:
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 1; j <= 3; ++j)
            {
                Point temp = pivot.addPoint(j, i);
                if (!temp.checkPoint())
                    possible = false;
            }
        }
        if (possible)
        {
            remove();
            state += 1;
            spawn();
        }
        break;
    default:
        break;
    }
}
////////////////////////////////////////////////// ZBlock
class ZBlock
{
private:
    Point blocks[4];
    int color = 3;
    void setZBlock();

public:
    Point pivot;
    int state;
    void spawn();
    void remove();
    bool canGoDown();
    void goDown();
    void goLeft();
    void goRight();
    void rotate();
    // state 0
    // 32
    //  01
    // state 1
    //  3
    // 02
    // 1
};
void ZBlock::setZBlock()
{
    switch (state % 2)
    {
    case 0:
        blocks[0] = pivot;
        blocks[1] = pivot.addPoint(1, 0);
        blocks[2] = pivot.addPoint(0, -1);
        blocks[3] = pivot.addPoint(-1, -1);
        break;
    case 1:
        blocks[0] = pivot;
        blocks[1] = pivot.addPoint(0, 1);
        blocks[2] = pivot.addPoint(1, 0);
        blocks[3] = pivot.addPoint(1, -1);
        break;
    default:
        break;
    }
}
void ZBlock::spawn()
{
    setZBlock();
    for (int i = 0; i < 4; ++i)
        arena[blocks[i].y][blocks[i].x] = 1;
}
void ZBlock::remove()
{
    setZBlock();
    for (int i = 0; i < 4; ++i)
        arena[blocks[i].y][blocks[i].x] = 0;
}
bool ZBlock::canGoDown()
{
    bool checkBlock[4];
    switch (state % 2)
    {
    case 0:
        checkBlock[0] = true;
        checkBlock[1] = true;
        checkBlock[2] = false;
        checkBlock[3] = true;
        break;
    case 1:
        checkBlock[0] = false;
        checkBlock[1] = true;
        checkBlock[2] = true;
        checkBlock[3] = false;
        break;
    default:
        break;
    }
    for (int i = 0; i < 4; ++i)
    {
        if (checkBlock[i])
        {
            Point temp = blocks[i].addPoint(0, 1);
            if (!temp.checkPoint())
                return false;
        }
    }
    return true;
}
void ZBlock::goDown()
{
    remove();
    pivot = pivot.addPoint(0, 1);
    spawn();
}
void ZBlock::goLeft()
{
    bool possible = true;
    bool checkBlock[4];
    switch (state % 2)
    {
    case 0:
        checkBlock[0] = true;
        checkBlock[1] = false;
        checkBlock[2] = false;
        checkBlock[3] = true;
        break;
    case 1:
        checkBlock[0] = true;
        checkBlock[1] = true;
        checkBlock[2] = false;
        checkBlock[3] = true;
        break;
    default:
        break;
    }
    for (int i = 0; i < 4; ++i)
    {
        if (checkBlock[i])
        {
            Point temp = blocks[i].addPoint(-1, 0);
            if (!temp.checkPoint())
                possible = false;
        }
    }
    if (possible)
    {
        remove();
        pivot = pivot.addPoint(-1, 0);
        spawn();
    }
}
void ZBlock::goRight()
{
    bool possible = true;
    bool checkBlock[4];
    switch (state % 2)
    {
    case 0:
        checkBlock[0] = false;
        checkBlock[1] = true;
        checkBlock[2] = true;
        checkBlock[3] = false;
        break;
    case 1:
        checkBlock[0] = false;
        checkBlock[1] = true;
        checkBlock[2] = true;
        checkBlock[3] = true;
        break;
    default:
        break;
    }
    for (int i = 0; i < 4; ++i)
    {
        if (checkBlock[i])
        {
            Point temp = blocks[i].addPoint(1, 0);
            if (!temp.checkPoint())
                possible = false;
        }
    }
    if (possible)
    {
        remove();
        pivot = pivot.addPoint(1, 0);
        spawn();
    }
}
void ZBlock::rotate()
{
    bool possible = true;
    Point temp;
    switch (state % 2)
    {
    case 0:

        for (int i = -1; i <= 1; ++i)
        {
            temp = pivot.addPoint(i, -2);
            if (!temp.checkPoint())
                possible = false;
        }

        temp = pivot.addPoint(1, -1);
        if (!temp.checkPoint())
            possible = false;

        temp = pivot.addPoint(0, 1);
        if (!temp.checkPoint())
            possible = false;

        temp = pivot.addPoint(1, 1);
        if (!temp.checkPoint())
            possible = false;

        if (possible)
        {
            remove();
            state += 1;
            spawn();
        }
        break;
    case 1:
        for (int i = -1; i <= 1; ++i)
        {
            temp = pivot.addPoint(-1, i);
            if (!temp.checkPoint())
                possible = false;
        }

        temp = pivot.addPoint(0, -1);
        if (!temp.checkPoint())
            possible = false;

        temp = pivot.addPoint(2, 0);
        if (!temp.checkPoint())
            possible = false;

        temp = pivot.addPoint(2, -1);
        if (!temp.checkPoint())
            possible = false;

        if (possible)
        {
            remove();
            state += 1;
            spawn();
        }

        break;
    default:
        break;
    }
}

////////////////////////////////////// T Block
class TBlock
{
private:
    Point blocks[4];
    int color = 4;
    void setTBlock();

public:
    Point pivot;
    int state;
    void spawn();
    void remove();
    bool canGoDown();
    void goDown();
    void goLeft();
    void goRight();
    void rotate();
    // state 0
    //   3
    //  102
    // state 1
    // 1
    // 0 3
    // 2
    // state 2
    // 2 0 1
    //   3
    // state 3
    //     2
    //  3  0
    //     1
};

void TBlock::setTBlock()
{
    switch (state % 4)
    {
    case 0:
        blocks[0] = pivot;
        blocks[1] = pivot.addPoint(-1, 0);
        blocks[2] = pivot.addPoint(1, 0);
        blocks[3] = pivot.addPoint(0, -1);
        break;
    case 1:
        blocks[0] = pivot;
        blocks[1] = pivot.addPoint(0, -1);
        blocks[2] = pivot.addPoint(0, 1);
        blocks[3] = pivot.addPoint(1, 0);
        break;
    case 2:
        blocks[0] = pivot;
        blocks[1] = pivot.addPoint(1, 0);
        blocks[2] = pivot.addPoint(-1, 0);
        blocks[3] = pivot.addPoint(0, 1);
        break;
    case 3:
        blocks[0] = pivot;
        blocks[1] = pivot.addPoint(0, 1);
        blocks[2] = pivot.addPoint(0, -1);
        blocks[3] = pivot.addPoint(-1, 0);
        break;

    default:
        break;
    }
}

void TBlock::spawn()
{
    setTBlock();
    for (int i = 0; i < 4; ++i)
        arena[blocks[i].y][blocks[i].x] = 1;
}
void TBlock::remove()
{
    setTBlock();
    for (int i = 0; i < 4; ++i)
        arena[blocks[i].y][blocks[i].x] = 0;
}

bool TBlock::canGoDown()
{
    bool checkBlock[4];
    switch (state % 4)
    {
    case 0:
        checkBlock[0] = true;
        checkBlock[1] = true;
        checkBlock[2] = true;
        checkBlock[3] = false;
        break;
    case 1:
        checkBlock[0] = false;
        checkBlock[1] = false;
        checkBlock[2] = true;
        checkBlock[3] = true;
        break;
    case 2:
        checkBlock[0] = false;
        checkBlock[1] = true;
        checkBlock[2] = true;
        checkBlock[3] = true;
        break;
    case 3:
        checkBlock[0] = false;
        checkBlock[1] = true;
        checkBlock[2] = false;
        checkBlock[3] = true;
        break;

    default:
        break;
    }
    for (int i = 0; i < 4; ++i)
    {
        if (checkBlock[i])
        {
            Point temp = blocks[i].addPoint(0, 1);
            if (!temp.checkPoint())
                return false;
        }
    }
    return true;
}
void TBlock::goDown()
{
    remove();
    pivot = pivot.addPoint(0, 1);
    spawn();
}

void TBlock::goLeft()
{
    bool possible = true;
    bool checkBlock[4];
    switch (state % 4)
    {
    case 0:
        checkBlock[0] = false;
        checkBlock[1] = true;
        checkBlock[2] = false;
        checkBlock[3] = true;
        break;
    case 1:
        checkBlock[0] = true;
        checkBlock[1] = true;
        checkBlock[2] = true;
        checkBlock[3] = false;
        break;
    case 2:
        checkBlock[0] = false;
        checkBlock[1] = false;
        checkBlock[2] = true;
        checkBlock[3] = true;
        break;

    case 3:
        checkBlock[0] = false;
        checkBlock[1] = true;
        checkBlock[2] = true;
        checkBlock[3] = true;
        break;

    default:
        break;
    }
    for (int i = 0; i < 4; ++i)
    {
        if (checkBlock[i])
        {
            Point temp = blocks[i].addPoint(-1, 0);
            if (!temp.checkPoint())
                possible = false;
        }
    }
    if (possible)
    {
        remove();
        pivot = pivot.addPoint(-1, 0);
        spawn();
    }
}
void TBlock::goRight()
{
    bool possible = true;
    bool checkBlock[4];
    switch (state % 4)
    {
    case 0:
        checkBlock[0] = false;
        checkBlock[1] = false;
        checkBlock[2] = true;
        checkBlock[3] = true;
        break;
    case 1:
        checkBlock[0] = false;
        checkBlock[1] = true;
        checkBlock[2] = true;
        checkBlock[3] = true;
        break;
    case 2:
        checkBlock[0] = false;
        checkBlock[1] = true;
        checkBlock[2] = false;
        checkBlock[3] = true;
        break;

    case 3:
        checkBlock[0] = true;
        checkBlock[1] = true;
        checkBlock[2] = true;
        checkBlock[3] = false;
        break;

    default:
        break;
    }
    for (int i = 0; i < 4; ++i)
    {
        if (checkBlock[i])
        {
            Point temp = blocks[i].addPoint(1, 0);
            if (!temp.checkPoint())
                possible = false;
        }
    }
    if (possible)
    {
        remove();
        pivot = pivot.addPoint(1, 0);
        spawn();
    }
}
void TBlock::rotate()
{
    bool possible = true;
    Point temp;
    switch (state % 4)
    {
    case 0:
        temp = pivot.addPoint(-1, -1);
        if (!temp.checkPoint())
            possible = false;
        temp = pivot.addPoint(1, -1);
        if (!temp.checkPoint())
            possible = false;
        temp = pivot.addPoint(1, 1);
        if (!temp.checkPoint())
            possible = false;
        temp = pivot.addPoint(0, 1);
        if (!temp.checkPoint())
            possible = false;
        if (possible)
        {
            remove();
            state += 1;
            spawn();
        }
        break;
    case 1:
        temp = pivot.addPoint(1, -1);
        if (!temp.checkPoint())
            possible = false;
        temp = pivot.addPoint(1, 1);
        if (!temp.checkPoint())
            possible = false;
        temp = pivot.addPoint(-1, 1);
        if (!temp.checkPoint())
            possible = false;
        temp = pivot.addPoint(-1, 0);
        if (!temp.checkPoint())
            possible = false;
        if (possible)
        {
            remove();
            state += 1;
            spawn();
        }
        break;
    case 2:
        temp = pivot.addPoint(1, -1);
        if (!temp.checkPoint())
            possible = false;
        temp = pivot.addPoint(-1, -1);
        if (!temp.checkPoint())
            possible = false;
        temp = pivot.addPoint(-1, 1);
        if (!temp.checkPoint())
            possible = false;
        temp = pivot.addPoint(0, -1);
        if (!temp.checkPoint())
            possible = false;
        if (possible)
        {
            remove();
            state += 1;
            spawn();
        }
        break;
    case 3:
        temp = pivot.addPoint(1, -1);
        if (!temp.checkPoint())
            possible = false;
        temp = pivot.addPoint(-1, -1);
        if (!temp.checkPoint())
            possible = false;
        temp = pivot.addPoint(-1, 1);
        if (!temp.checkPoint())
            possible = false;
        temp = pivot.addPoint(1, 0);
        if (!temp.checkPoint())
            possible = false;
        if (possible)
        {
            remove();
            state += 1;
            spawn();
        }
        break;
    default:
        break;
    }
}
////////////////////////////////////// S Block
class SBlock
{
private:
    Point blocks[4];
    int color = 5;
    void setSBlock();

public:
    Point pivot;
    int state;
    void spawn();
    void remove();
    bool canGoDown();
    void goDown();
    void goLeft();
    void goRight();
    void rotate();
    // state 0
    //   0 1
    // 3 2
    // state 1
    // 3
    // 2 0
    //   1
};

void SBlock::setSBlock()
{
    switch (state % 2)
    {
    case 0:
        blocks[0] = pivot;
        blocks[1] = pivot.addPoint(1, 0);
        blocks[2] = pivot.addPoint(0, 1);
        blocks[3] = pivot.addPoint(-1, 1);
        break;
    case 1:
        blocks[0] = pivot;
        blocks[1] = pivot.addPoint(0, 1);
        blocks[2] = pivot.addPoint(-1, 0);
        blocks[3] = pivot.addPoint(-1, -1);
        break;

    default:
        break;
    }
}

void SBlock::spawn()
{
    setSBlock();
    for (int i = 0; i < 4; ++i)
        arena[blocks[i].y][blocks[i].x] = 1;
}
void SBlock::remove()
{
    setSBlock();
    for (int i = 0; i < 4; ++i)
        arena[blocks[i].y][blocks[i].x] = 0;
}
bool SBlock::canGoDown()
{
    bool checkBlock[4];
    switch (state % 2)
    {
    case 0:
        checkBlock[0] = false;
        checkBlock[1] = true;
        checkBlock[2] = true;
        checkBlock[3] = true;
        break;
    case 1:
        checkBlock[0] = false;
        checkBlock[1] = true;
        checkBlock[2] = true;
        checkBlock[3] = false;
        break;

    default:
        break;
    }
    for (int i = 0; i < 4; ++i)
    {
        if (checkBlock[i])
        {
            Point temp = blocks[i].addPoint(0, 1);
            if (!temp.checkPoint())
                return false;
        }
    }
    return true;
}
void SBlock::goDown()
{
    remove();
    pivot = pivot.addPoint(0, 1);
    spawn();
}
void SBlock::goLeft()
{
    bool possible = true;
    bool checkBlock[4];
    switch (state % 2)
    {
    case 0:
        checkBlock[0] = true;
        checkBlock[1] = false;
        checkBlock[2] = false;
        checkBlock[3] = true;
        break;
    case 1:
        checkBlock[0] = false;
        checkBlock[1] = true;
        checkBlock[2] = true;
        checkBlock[3] = true;
        break;

    default:
        break;
    }
    for (int i = 0; i < 4; ++i)
    {
        if (checkBlock[i])
        {
            Point temp = blocks[i].addPoint(-1, 0);
            if (!temp.checkPoint())
                possible = false;
        }
    }
    if (possible)
    {
        remove();
        pivot = pivot.addPoint(-1, 0);
        spawn();
    }
}
void SBlock::goRight()
{
    bool possible = true;
    bool checkBlock[4];
    switch (state % 2)
    {
    case 0:
        checkBlock[0] = false;
        checkBlock[1] = true;
        checkBlock[2] = true;
        checkBlock[3] = false;
        break;
    case 1:
        checkBlock[0] = true;
        checkBlock[1] = true;
        checkBlock[2] = false;
        checkBlock[3] = false;
        break;

    default:
        break;
    }
    for (int i = 0; i < 4; ++i)
    {
        if (checkBlock[i])
        {
            Point temp = blocks[i].addPoint(1, 0);
            if (!temp.checkPoint())
                possible = false;
        }
    }
    if (possible)
    {
        remove();
        pivot = pivot.addPoint(1, 0);
        spawn();
    }
}
void SBlock::rotate()
{
    bool possible = true;
    Point temp;
    switch (state % 2)
    {
    case 0:
        temp = pivot.addPoint(-1, -1);
        if (!temp.checkPoint())
            possible = false;
        temp = pivot.addPoint(-1, 0);
        if (!temp.checkPoint())
            possible = false;
        temp = pivot.addPoint(1, 1);
        if (!temp.checkPoint())
            possible = false;
        if (possible)
        {
            remove();
            state += 1;
            spawn();
        }
        break;
    case 1:
        temp = pivot.addPoint(-1, 1);
        if (!temp.checkPoint())
            possible = false;
        temp = pivot.addPoint(0, -1);
        if (!temp.checkPoint())
            possible = false;
        temp = pivot.addPoint(1, -1);
        if (!temp.checkPoint())
            possible = false;
        if (possible)
        {
            remove();
            state += 1;
            spawn();
        }
        break;

    default:
        break;
    }
}

////////////////////////////////////// J Block
class JBlock
{
private:
    Point blocks[4];
    int color = 6;
    void setJBlock();

public:
    Point pivot;
    int state;
    void spawn();
    void remove();
    bool canGoDown();
    void goDown();
    void goLeft();
    void goRight();
    void rotate();
    // state 0
    // 1
    // 0 2 3
    // state 1
    // 0 1
    // 2
    // 3
    // state 2
    // 3 2 0
    //     1
    // state 3
    //     3
    //     2
    //   1 0
};

void JBlock::setJBlock()
{
    switch (state % 4)
    {
    case 0:
        blocks[0] = pivot;
        blocks[1] = pivot.addPoint(0, -1);
        blocks[2] = pivot.addPoint(1, 0);
        blocks[3] = pivot.addPoint(2, 0);
        break;
    case 1:
        blocks[0] = pivot;
        blocks[1] = pivot.addPoint(1, 0);
        blocks[2] = pivot.addPoint(0, 1);
        blocks[3] = pivot.addPoint(0, 2);
        break;
    case 2:
        blocks[0] = pivot;
        blocks[1] = pivot.addPoint(0, 1);
        blocks[2] = pivot.addPoint(-1, 0);
        blocks[3] = pivot.addPoint(-2, 0);
        break;
    case 3:
        blocks[0] = pivot;
        blocks[1] = pivot.addPoint(-1, 0);
        blocks[2] = pivot.addPoint(0, -1);
        blocks[3] = pivot.addPoint(0, -2);
        break;

    default:
        break;
    }
}

void JBlock::spawn()
{
    setJBlock();
    for (int i = 0; i < 4; ++i)
        arena[blocks[i].y][blocks[i].x] = 1;
}
void JBlock::remove()
{
    setJBlock();
    for (int i = 0; i < 4; ++i)
        arena[blocks[i].y][blocks[i].x] = 0;
}
bool JBlock::canGoDown()
{
    bool checkBlock[4];
    switch (state % 4)
    {
    case 0:
        checkBlock[0] = true;
        checkBlock[1] = false;
        checkBlock[2] = true;
        checkBlock[3] = true;
        break;
    case 1:
        checkBlock[0] = false;
        checkBlock[1] = true;
        checkBlock[2] = false;
        checkBlock[3] = true;
        break;
    case 2:
        checkBlock[0] = false;
        checkBlock[1] = true;
        checkBlock[2] = true;
        checkBlock[3] = true;
        break;
    case 3:
        checkBlock[0] = true;
        checkBlock[1] = true;
        checkBlock[2] = false;
        checkBlock[3] = false;
        break;

    default:
        break;
    }
    for (int i = 0; i < 4; ++i)
    {
        if (checkBlock[i])
        {
            Point temp = blocks[i].addPoint(0, 1);
            if (!temp.checkPoint())
                return false;
        }
    }
    return true;
}
void JBlock::goDown()
{
    remove();
    pivot = pivot.addPoint(0, 1);
    spawn();
}
void JBlock::goLeft()
{
    bool possible = true;
    bool checkBlock[4];
    switch (state % 4)
    {
    case 0:
        checkBlock[0] = true;
        checkBlock[1] = true;
        checkBlock[2] = false;
        checkBlock[3] = false;
        break;
    case 1:
        checkBlock[0] = true;
        checkBlock[1] = false;
        checkBlock[2] = true;
        checkBlock[3] = true;
        break;
    case 2:
        checkBlock[0] = false;
        checkBlock[1] = true;
        checkBlock[2] = false;
        checkBlock[3] = true;
        break;
    case 3:
        checkBlock[0] = false;
        checkBlock[1] = true;
        checkBlock[2] = true;
        checkBlock[3] = true;
        break;

    default:
        break;
    }
    for (int i = 0; i < 4; ++i)
    {
        if (checkBlock[i])
        {
            Point temp = blocks[i].addPoint(-1, 0);
            if (!temp.checkPoint())
                possible = false;
        }
    }
    if (possible)
    {
        remove();
        pivot = pivot.addPoint(-1, 0);
        spawn();
    }
}
void JBlock::goRight()
{
    bool possible = true;
    bool checkBlock[4];
    switch (state % 4)
    {
    case 0:
        checkBlock[0] = false;
        checkBlock[1] = true;
        checkBlock[2] = false;
        checkBlock[3] = true;
        break;
    case 1:
        checkBlock[0] = false;
        checkBlock[1] = true;
        checkBlock[2] = true;
        checkBlock[3] = true;
        break;
    case 2:
        checkBlock[0] = true;
        checkBlock[1] = true;
        checkBlock[2] = false;
        checkBlock[3] = false;
        break;
    case 3:
        checkBlock[0] = true;
        checkBlock[1] = false;
        checkBlock[2] = true;
        checkBlock[3] = true;
        break;

    default:
        break;
    }
    for (int i = 0; i < 4; ++i)
    {
        if (checkBlock[i])
        {
            Point temp = blocks[i].addPoint(1, 0);
            if (!temp.checkPoint())
                possible = false;
        }
    }
    if (possible)
    {
        remove();
        pivot = pivot.addPoint(1, 0);
        spawn();
    }
}
void JBlock::rotate()
{
    bool possible = true;
    Point temp;
    switch (state % 4)
    {
    case 0:
        for (int i = 1; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                temp = pivot.addPoint(j, i);
                if (!temp.checkPoint())
                    possible = false;
            }
        }
        temp = pivot.addPoint(1, -1);
        if (!temp.checkPoint())
            possible = false;
        if (possible)
        {
            remove();
            state += 1;
            spawn();
        }
        break;
    case 1:
        for (int i = 1; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                temp = pivot.addPoint(-i, j);
                if (!temp.checkPoint())
                    possible = false;
            }
        }
        temp = pivot.addPoint(1, 1);
        if (!temp.checkPoint())
            possible = false;
        if (possible)
        {
            remove();
            state += 1;
            spawn();
        }
        break;
    case 2:
        for (int i = 1; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                temp = pivot.addPoint(-j, -i);
                if (!temp.checkPoint())
                    possible = false;
            }
        }
        temp = pivot.addPoint(-1, 1);
        if (!temp.checkPoint())
            possible = false;
        if (possible)
        {
            remove();
            state += 1;
            spawn();
        }
        break;
    case 3:
        for (int i = 1; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                temp = pivot.addPoint(i, -j);
                if (!temp.checkPoint())
                    possible = false;
            }
        }
        temp = pivot.addPoint(-1, -1);
        if (!temp.checkPoint())
            possible = false;
        if (possible)
        {
            remove();
            state += 1;
            spawn();
        }
        break;
    default:
        break;
    }
}

////////////////////////////////////// L Block
class LBlock
{
private:
    Point blocks[4];
    int color = 7;
    void setLBlock();

public:
    Point pivot;
    int state;
    void spawn();
    void remove();
    bool canGoDown();
    void goDown();
    void goLeft();
    void goRight();
    void rotate();
    // state 0
    //     1
    // 3 2 0
    // state 1
    // 3
    // 2
    // 0 1
    // state 2
    // 0 2 3
    // 1
    // state 3
    //  1 0
    //    2
    //    3
};

void LBlock::setLBlock()
{
    switch (state % 4)
    {
    case 0:
        blocks[0] = pivot;
        blocks[1] = pivot.addPoint(0, -1);
        blocks[2] = pivot.addPoint(-1, 0);
        blocks[3] = pivot.addPoint(-2, 0);
        break;
    case 1:
        blocks[0] = pivot;
        blocks[1] = pivot.addPoint(1, 0);
        blocks[2] = pivot.addPoint(0, -1);
        blocks[3] = pivot.addPoint(0, -2);
        break;
    case 2:
        blocks[0] = pivot;
        blocks[1] = pivot.addPoint(0, 1);
        blocks[2] = pivot.addPoint(1, 0);
        blocks[3] = pivot.addPoint(2, 0);
        break;
    case 3:
        blocks[0] = pivot;
        blocks[1] = pivot.addPoint(-1, 0);
        blocks[2] = pivot.addPoint(0, 1);
        blocks[3] = pivot.addPoint(0, 2);
        break;

    default:
        break;
    }
}

void LBlock::spawn()
{
    setLBlock();
    for (int i = 0; i < 4; ++i)
        arena[blocks[i].y][blocks[i].x] = 1;
}
void LBlock::remove()
{
    setLBlock();
    for (int i = 0; i < 4; ++i)
        arena[blocks[i].y][blocks[i].x] = 0;
}
bool LBlock::canGoDown()
{
    bool checkBlock[4];
    switch (state % 4)
    {
    case 0:
        checkBlock[0] = true;
        checkBlock[1] = false;
        checkBlock[2] = true;
        checkBlock[3] = true;
        break;
    case 1:
        checkBlock[0] = true;
        checkBlock[1] = true;
        checkBlock[2] = false;
        checkBlock[3] = false;
        break;
    case 2:
        checkBlock[0] = false;
        checkBlock[1] = true;
        checkBlock[2] = true;
        checkBlock[3] = true;
        break;
    case 3:
        checkBlock[0] = false;
        checkBlock[1] = true;
        checkBlock[2] = false;
        checkBlock[3] = true;
        break;

    default:
        break;
    }
    for (int i = 0; i < 4; ++i)
    {
        if (checkBlock[i])
        {
            Point temp = blocks[i].addPoint(0, 1);
            if (!temp.checkPoint())
                return false;
        }
    }
    return true;
}
void LBlock::goDown()
{
    remove();
    pivot = pivot.addPoint(0, 1);
    spawn();
}
void LBlock::goLeft()
{
    bool possible = true;
    bool checkBlock[4];
    switch (state % 4)
    {
    case 0:
        checkBlock[0] = false;
        checkBlock[1] = true;
        checkBlock[2] = false;
        checkBlock[3] = true;
        break;
    case 1:
        checkBlock[0] = true;
        checkBlock[1] = false;
        checkBlock[2] = true;
        checkBlock[3] = true;
        break;
    case 2:
        checkBlock[0] = true;
        checkBlock[1] = true;
        checkBlock[2] = false;
        checkBlock[3] = false;
        break;
    case 3:
        checkBlock[0] = false;
        checkBlock[1] = true;
        checkBlock[2] = true;
        checkBlock[3] = true;
        break;

    default:
        break;
    }
    for (int i = 0; i < 4; ++i)
    {
        if (checkBlock[i])
        {
            Point temp = blocks[i].addPoint(-1, 0);
            if (!temp.checkPoint())
                possible = false;
        }
    }
    if (possible)
    {
        remove();
        pivot = pivot.addPoint(-1, 0);
        spawn();
    }
}
void LBlock::goRight()
{
    bool possible = true;
    bool checkBlock[4];
    switch (state % 4)
    {
    case 0:
        checkBlock[0] = true;
        checkBlock[1] = true;
        checkBlock[2] = false;
        checkBlock[3] = false;
        break;
    case 1:
        checkBlock[0] = false;
        checkBlock[1] = true;
        checkBlock[2] = true;
        checkBlock[3] = true;
        break;
    case 2:
        checkBlock[0] = false;
        checkBlock[1] = true;
        checkBlock[2] = false;
        checkBlock[3] = true;
        break;
    case 3:
        checkBlock[0] = true;
        checkBlock[1] = false;
        checkBlock[2] = true;
        checkBlock[3] = true;
        break;

    default:
        break;
    }
    for (int i = 0; i < 4; ++i)
    {
        if (checkBlock[i])
        {
            Point temp = blocks[i].addPoint(1, 0);
            if (!temp.checkPoint())
                possible = false;
        }
    }
    if (possible)
    {
        remove();
        pivot = pivot.addPoint(1, 0);
        spawn();
    }
}
void LBlock::rotate()
{
    bool possible = true;
    Point temp;
    switch (state % 4)
    {
    case 0:
        for (int i = 0; i < 3; ++i)
        {
            temp = pivot.addPoint(-i, -2);
            if (!temp.checkPoint())
                possible = false;
        }
        for (int i = 0; i < 3; ++i)
        {
            if (i == 0)
                temp = pivot.addPoint(-i + 1, -1);
            else
                temp = pivot.addPoint(-i, -1);
            if (!temp.checkPoint())
                possible = false;
        }
        temp = pivot.addPoint(1, 0);
        if (!temp.checkPoint())
            possible = false;
        if (possible)
        {
            remove();
            state += 1;
            spawn();
        }
        break;
    case 1:
        for (int i = 0; i < 3; ++i)
        {
            temp = pivot.addPoint(2, -i);
            if (!temp.checkPoint())
                possible = false;
        }
        for (int i = 0; i < 3; ++i)
        {
            if (i == 0)
                temp = pivot.addPoint(1, -i + 1);
            else
                temp = pivot.addPoint(1, -i);
            if (!temp.checkPoint())
                possible = false;
        }
        temp = pivot.addPoint(0, 1);
        if (!temp.checkPoint())
            possible = false;
        if (possible)
        {
            remove();
            state += 1;
            spawn();
        }
        break;
    case 2:
        for (int i = 0; i < 3; ++i)
        {
            temp = pivot.addPoint(i, 2);
            if (!temp.checkPoint())
                possible = false;
        }
        for (int i = 0; i < 3; ++i)
        {
            if (i == 0)
                temp = pivot.addPoint(i - 1, 1);
            else
                temp = pivot.addPoint(i, 1);
            if (!temp.checkPoint())
                possible = false;
        }
        temp = pivot.addPoint(-1, 0);
        if (!temp.checkPoint())
            possible = false;
        if (possible)
        {
            remove();
            state += 1;
            spawn();
        }
        break;
    case 3:
        for (int i = 0; i < 3; ++i)
        {
            temp = pivot.addPoint(-2, i);
            if (!temp.checkPoint())
                possible = false;
        }
        for (int i = 0; i < 3; ++i)
        {
            if (i == 0)
                temp = pivot.addPoint(-1, i - 1);
            else
                temp = pivot.addPoint(-1, i);
            if (!temp.checkPoint())
                possible = false;
        }
        temp = pivot.addPoint(0, -1);
        if (!temp.checkPoint())
            possible = false;
        if (possible)
        {
            remove();
            state += 1;
            spawn();
        }
        break;
    default:
        break;
    }
}

//////////////////////////////////////////////////
bool checkOverflow()
{
    for (int i = 0; i < arenaX; ++i)
    {
        if (arena[3][i] != 0)
            return true;
    }
    return false;
}
int checkButtonPress()
{
    int buttonLeft = digitalRead(leftButton);
    int buttonRight = digitalRead(rightButton);
    int buttonRot = digitalRead(rotButton);
    int buttonDown = digitalRead(downButton);
    if (buttonLeft == LOW)
    {
        // Serial.println("left Button Pressed");
        return 1;
    }

    else if (buttonRight == LOW)
        return 2;
    else if (buttonRot == LOW){
         while(buttonRot == HIGH);
         return 3;
        }
    else if (buttonDown == LOW)
        return 4;
    else
        return 0;
}
void removeLines()
{
    int temp = 0;
    for (int i = 4; i < arenaY; ++i)
    {
        bool line = true;
        for (int j = 0; j < arenaX; ++j)
        {
            if (arena[i][j] == 0)
                line = false;
        }
        if (line)
        {
            for (int j = 0; j < arenaX; ++j)
                arena[i][j] == 0;
            delay(animationTime);
            printScreen();
            for (int j = i; j > 0; --j)
            {
                for (int k = 0; k < arenaX; ++k)
                    arena[j][k] = arena[j - 1][k];
            }
            delay(animationTime);
            printScreen();
            i -= 1;
            temp += 1;
        }
    }
    if (temp > 0)
    {
        score += temp + temp - 1;
        printScore(score);
    }
        
}
/////////////////////////////////////////////////////

unsigned long timeIntial;
unsigned long timeFinal;
Point mainPivot;
Square sqr;
IBlock iBlock;
ZBlock zBlock;
SBlock sBlock;
JBlock jBlock;
LBlock lBlock;
TBlock tBlock;
int intialState = 0;
void setup()
{
    reseedRandom(&reseedRandomSeed);
    pinMode(leftButton, INPUT_PULLUP);
    pinMode(rightButton, INPUT_PULLUP);
    pinMode(downButton, INPUT_PULLUP);
    pinMode(rotButton, INPUT_PULLUP);
    pixels.begin();
    pixels.clear();
    timeIntial = millis();
    timeFinal = millis();
    mainPivot.setPoint(3, 2);
    sqr.pivot = mainPivot;
    sqr.state = intialState;
    iBlock.pivot = mainPivot;
    iBlock.state = intialState;
    zBlock.pivot = mainPivot;
    zBlock.state = intialState;
    tBlock.pivot = mainPivot;
    tBlock.state = intialState;
    sBlock.pivot = mainPivot;
    sBlock.state = intialState;
    jBlock.pivot = mainPivot;
    jBlock.state = intialState;
    lBlock.pivot = mainPivot;
    lBlock.state = intialState;
    tBlock.pivot = mainPivot;
    tBlock.state = intialState;
    Serial.begin(9600);
    printScore(score);
}

void updateHighScore(int score) {
    highscore = EEPROM.get(50, highscore);
    if (score > highscore) {
        highscore = score;
        EEPROM.put(50, score);
    }
}

void printScore(int score) {
    updateHighScore(score);
    Serial.print(F("S: "));
    Serial.print(score);
    Serial.print(F(" | H:"));
    Serial.println(highscore);
}

bool inPlay = false;
bool overflow = false;
void loop()
{ 
    // Serial.println("Loop test");
    if(!overflow)
    {
        timeFinal = millis();
        if (inPlay)
        {
          // Serial.println("Inpla test");
            switch (piece)
            {
            case 0:
                sqr.spawn();
                break;
            case 1:
                iBlock.spawn();
                break;
            case 2:
                zBlock.spawn();
                break;
            case 3:
                tBlock.spawn();
                break;
            case 4:
                sBlock.spawn();
                break;
            case 5:
                jBlock.spawn();
                break;
            case 6:
                lBlock.spawn();
                break;
            }
            // // /////////////////////// down
            if ((timeFinal - timeIntial) > downTime)
            {
                timeFinal = millis();
                timeIntial = timeFinal;
                switch (piece)
                {
                case 0:
                    if (sqr.canGoDown())
                    {
                        sqr.goDown();
                    }
                    else
                    {
                        inPlay = false;
                        overflow = checkOverflow();
                        removeLines();
                    }
                    break;
                case 1:
                    if (iBlock.canGoDown())
                    {
                        iBlock.goDown();
                    }
                    else
                    {
                        inPlay = false;
                        overflow = checkOverflow();
                        removeLines();
                    }
                    break;
                case 2:
                    if (zBlock.canGoDown())
                    {
                        zBlock.goDown();
                    }
                    else
                    {
                        inPlay = false;
                        overflow = checkOverflow();
                        removeLines();
                    }
                    break;
                case 3:
                    if (tBlock.canGoDown())
                    {
                        tBlock.goDown();
                    }
                    else
                    {
                        inPlay = false;
                        overflow = checkOverflow();
                        removeLines();
                    }
                    break;
                case 4:
                    if (sBlock.canGoDown())
                    {
                        sBlock.goDown();
                    }
                    else
                    {
                        inPlay = false;
                        overflow = checkOverflow();
                        removeLines();
                    }
                    break;
                case 5:
                    if (jBlock.canGoDown())
                    {
                        jBlock.goDown();
                    }
                    else
                    {
                        inPlay = false;
                        overflow = checkOverflow();
                        removeLines();
                    }
                    break;
                case 6:
                    if (lBlock.canGoDown())
                    {
                        lBlock.goDown();
                    }
                    else
                    {
                        inPlay = false;
                        overflow = checkOverflow();
                        removeLines();
                    }
                    break;

                default:
                    break;
                }
                
            }
            if (inPlay)
            {
                // // //  //////////////////////// buttonInput
                int input = checkButtonPress();
                if (input != 0)
                {
                    if (input == 1)
                    {
                        switch (piece)
                        {
                        case 0:
                            sqr.goLeft();
                            break;
                        case 1:
                            iBlock.goLeft();
                            break;
                        case 2:
                            zBlock.goLeft();
                            break;
                        case 3:
                            tBlock.goLeft();
                            break;
                        case 4:
                            sBlock.goLeft();
                            break;
                        case 5:
                            jBlock.goLeft();
                            break;
                        case 6:
                            lBlock.goLeft();
                            break;
                        }
                        delay(moveTime);
                    }
                    else if (input == 2)
                    {
                        switch (piece)
                        {
                        case 0:
                            sqr.goRight();
                            break;
                        case 1:
                            iBlock.goRight();
                            break;
                        case 2:
                            zBlock.goRight();
                            break;
                        case 3:
                            tBlock.goRight();
                            break;
                        case 4:
                            sBlock.goRight();
                            break;
                        case 5:
                            jBlock.goRight();
                            break;
                        case 6:
                            lBlock.goRight();
                            break;
                        }
                        delay(moveTime);
                    }
                    else if (input == 3)
                    {
                        switch (piece)
                        {
                        case 0:
                            break;
                        case 1:
                            iBlock.rotate();
                            break;
                        case 2:
                            zBlock.rotate();
                            break;
                        case 3:
                            tBlock.rotate();
                            break;
                        case 4:
                            sBlock.rotate();
                            break;
                        case 5:
                            jBlock.rotate();
                            break;
                        case 6:
                            lBlock.rotate();
                            break;
                        }
                        delay(moveTime);
                    }
                    else if (input == 4)
                    {
                        timeFinal = millis();
                        timeIntial = timeFinal;
                        switch (piece)
                        {
                        case 0:
                            if (sqr.canGoDown())
                            {
                                sqr.goDown();
                            }
                            else
                            {
                                inPlay = false;
                                overflow = checkOverflow();
                                removeLines();
                            }
                            break;
                        case 1:
                            if (iBlock.canGoDown())
                            {
                                iBlock.goDown();
                            }
                            else
                            {
                                inPlay = false;
                                overflow = checkOverflow();
                                removeLines();
                            }
                            break;
                        case 2:
                            if (zBlock.canGoDown())
                            {
                                zBlock.goDown();
                            }
                            else
                            {
                                inPlay = false;
                                overflow = checkOverflow();
                                removeLines();
                            }
                            break;
                        case 3:
                            if (tBlock.canGoDown())
                            {
                                tBlock.goDown();
                            }
                            else
                            {
                                inPlay = false;
                                overflow = checkOverflow();
                                removeLines();
                            }
                            break;
                        case 4:
                            if (sBlock.canGoDown())
                            {
                                sBlock.goDown();
                            }
                            else
                            {
                                inPlay = false;
                                overflow = checkOverflow();
                                removeLines();
                            }
                            break;
                        case 5:
                            if (jBlock.canGoDown())
                            {
                                jBlock.goDown();
                            }
                            else
                            {
                                inPlay = false;
                                overflow = checkOverflow();
                                removeLines();
                            }
                            break;
                        case 6:
                            if (lBlock.canGoDown())
                            {
                                lBlock.goDown();
                            }
                            else
                            {
                                inPlay = false;
                                overflow = checkOverflow();
                                removeLines();
                            }
                            break;
                        }
                        delay(moveTime);
                    }
                }
            }
        }
        else
        {

            piece = random(7);
            switch (piece)
            {
            case 0:
                sqr.pivot = mainPivot;
                sqr.state = intialState;
                break;
            case 1:
                iBlock.pivot = mainPivot;
                iBlock.state = intialState;
                break;
            case 2:
                zBlock.pivot = mainPivot;
                zBlock.state = intialState;
                break;
            case 3:
                tBlock.pivot = mainPivot;
                tBlock.state = intialState;
                break;
            case 4:
                sBlock.pivot = mainPivot;
                sBlock.state = intialState;
                break;
            case 5:
                jBlock.pivot = mainPivot;
                jBlock.state = intialState;
                break;
            case 6:
                lBlock.pivot = mainPivot;
                lBlock.state = intialState;
                break;
            }

            inPlay = true;
            color += 1;
            if(color > 5)
                color = 1;
            if(score >= level*5)
            {
                level += 1;
                if(downTime > 200)
                    downTime -= 100;
            }
        }
        printScreen();
    }
    else
    {
        // Serial.println("restart Game");
        overflow = false;
        level = 1;
        score = 0;
        printScore(score);
        color = 1;
        inPlay = false;
        downTime = 500;
        // timeInitial = 0;
        for(int i = arenaY - 1; i >= 0 ; --i )
        {
            if(i%2 == 0)
            {
                for(int j = 0; j < arenaX; ++j)
                    arena[i][j] = 1;
                printScreen();
                delay(50);
            }
            else
            {
                for(int j = arenaX - 1; j >= 0; --j)
                    arena[i][j] = 1;
                printScreen();
                delay(50);
            }
        }
        for(int i = 0; i < arenaY; ++i)
        {
            for(int j = 0; j < arenaX; ++j)
                arena[i][j] == 0;
        }
        printScreen();
    }
}