#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>

/*
    Map file format [ MaP! ] = 0x2150614D = 558915917
    
    int [FileFormat Descriptor] = MaP! (4 bytes)
    int [width]                        (4 bytes)
    int [height]                       (4 bytes)
    char [Name size]                   (1 byte)
    char [Author size]                 (1 byte)
    char []Name]                       (Name size * bytes)
    char [][Author]                    (Author size * bytes)
    int [Block count]                  (Block count)

    [ block data                       (16 bytes)
        - float x     - 4 bytes
        - float y     - 4 bytes
        - float angle - 4 bytes
        - int id      - 4 bytes                 
    ] * Block count

 */

const int defaultGridSize = 500;


struct Block
{
    float x, y;
    float angle;

    int id;
};

struct MapFile
{
    int width, height;
    std::string name;
    std::string author;
};

class Map
{
public:
    ~Map();
    bool saveMap();
    bool loadMap(std::string filename);

    void draw(sf::RenderWindow& window, sf::View& camera, class Resources *res);
    void addBlock(float blockX, float blockY, float blockAngle, int blockID);

    int getGridNumber(int x, int y);
    inline int getGridX(int x);
    inline int getGridY(int y);

    void createNew(std::string filename, int width, int height, std::string name, std::string author);

    std::vector <Block *> getBlocksOnCamera(sf::View& camera);

    bool isSaved() { return saved; }

    int getWidth() {  return info.width; }
    int getHeight() { return info.height; }

    void setWidthandHeight(int width, int height) { info.width = width; info.height = height;}
    void setFilename(std::string nameOfFile) {filename = nameOfFile + ".map";}
    void setName(std::string nameOfLevel)  {info.name = nameOfLevel;}
    void setAuthor(std::string authorName)   {info.author = authorName;}

private:
    void clear();

    MapFile info;
    std::vector <Block *> blocks;
    std::map <int, std::vector<Block *>> blockGrid;
    std::string filename;

    int gridSize = defaultGridSize;
    bool mapReady = false;

    bool saved = false;
};