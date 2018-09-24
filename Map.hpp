#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>

/*
    Map file format [ MaP! ] = 0x2150614D = 558915917
    
    int [FileFormat Descriptor] = MaP!
    int [width]
    int [height]
    char [Name size]
    char [Author size]
    int [Block count]

    [ block data
        - int x
        - int y
        - float angle
        - int id
    ] * Block count

 */

const int defaultGridSize = 500;


struct Block
{
    int x, y;
    float angle;

    int id;
};

struct MapFile
{
    int width, height;
    std::string name;
    std::string author;

    std::string filename;
};

class Map
{
public:
    ~Map();
    bool saveMap(std::string filename);
    bool loadMap(std::string filename);

    void draw(sf::RenderWindow& window, sf::View& camera, class Resources *res);
    void addBlock(int blockX, int blockY, float blockAngle, int blockID);

    int getGridNumber(int x, int y);
    inline int getGridX(int x);
    inline int getGridY(int y);

    void createNew(std::string filename, int width, int height, std::string name, std::string author);

    std::vector <Block *> getBlocksOnCamera(sf::View& camera);

    bool getSaveStatus() { return saved; }

    int getWidth() {  return info.width; }
    int getHeight() { return info.height; }


private:
    void clear();

    MapFile info;
    std::vector <Block *> blocks;
    std::map <int, std::vector<Block *>> blockGrid;

    int gridSize = defaultGridSize;
    bool mapReady = false;

    bool saved = false;
};