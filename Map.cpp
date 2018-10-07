#include "Map.hpp"
#include "Resources.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <cstring>

namespace fs = std::filesystem;

const int mapID = 0x2150614D;

Map::~Map()
{
    clear();
}

bool Map::saveMap()
{
    std::fstream mapFile;
    if(fs::exists(filename))
        fs::copy(filename, filename+".bak");

    mapFile.open(filename, std::ios::out | std::ios::binary);
    if(mapFile.is_open())
    {
        char nameLength = info.name.length();
        char authorLength = info.author.length();
        int blockCount = blocks.size();

        mapFile.write((const char *)&mapID,             4); // MaP!
        mapFile.write((const char *)&info.width,        4); // Width
        mapFile.write((const char *)&info.height,       4); // Height
        mapFile.write(&nameLength,                      1); // Name length
        mapFile.write(&authorLength,                    1); // Author name length

        mapFile.write(info.name.c_str(),                nameLength);    // Map name
        mapFile.write(info.author.c_str(),              authorLength);  // Author name

        mapFile.write((const char *)&blockCount,        4); // Block count

        for( auto* block : blocks )
        {
            mapFile.write((const char *)&block->x,       4);
            mapFile.write((const char *)&block->y,       4);
            mapFile.write((const char *)&block->angle,   4);
            mapFile.write((const char *)&block->id,      4);
        }
        
        mapFile.close();
    }
    else
        return false;
    
    saved = true;
    return true;
}

bool Map::loadMap(std::string filename)
{
    char readBuffer[256] = {};
    std::fstream mapFile;

    mapFile.open(filename, std::ios::in | std::ios::binary);
    if(mapFile.is_open())
    {
        char nameLength = 0;
        char authorLength = 0;
        int blockCount = 0;
        int readedMapID = 0;

        mapFile.read((char *)&readedMapID,              4); // File ID
        if ( readedMapID != mapID )
        {
            mapFile.close();
            return false;
        }

        clear();

        mapFile.read((char *)&info.width,               4); // Width
        mapFile.read((char *)&info.height,              4); // Height
        mapFile.read((char *)&nameLength,               1); // Name length
        mapFile.read((char *)&authorLength,             1); // Author name length
    
        mapFile.read(readBuffer,                nameLength);    // Name
        readBuffer[nameLength] = 0;
        info.name = readBuffer;

        mapFile.read(readBuffer,              authorLength);  // Author name
        readBuffer[authorLength] = 0;
        info.author = readBuffer;

        mapFile.read((char *)&blockCount,        4); // Block count

        for( unsigned int c = 0; c < blockCount; c++ )
        {
            Block *blockPointer = new Block;

            mapFile.read((char *)&blockPointer->x,       4);
            mapFile.read((char *)&blockPointer->y,       4);
            mapFile.read((char *)&blockPointer->angle,   4);
            mapFile.read((char *)&blockPointer->id,      4);

            blocks.push_back(blockPointer);
            blockGrid[getGridNumber(blockPointer->x, blockPointer->y)].emplace_back(blockPointer);

        }
        
        mapFile.close();
    }
    else
        return false;
    
    this->filename = filename.substr(0, filename.find_last_of('.'));
    saved = true;
    mapReady = true;
    return true;
}

void Map::clear()
{
    info.name       = "";
    info.author     = "";
    info.width      = 0;
    info.height     = 0;

    
    for(auto *i : blocks)
        if ( i )
            delete i;
    
    blocks.clear();
    blockGrid.clear();
}

std::vector <Block *> Map::getBlocksOnCamera(sf::View& camera)
{
    std::vector <Block *> result;

    if ( blocks.empty() )
        return result;

    int lastIndex = blockGrid.rbegin()->first;

    int halfSizeX = camera.getSize().x / 2.0f;
    int halfSizeY = camera.getSize().y / 2.0f;

    int xStartPositionIndex = getGridX(camera.getCenter().x - halfSizeX);
    int xEndPositionIndex   = getGridX(camera.getCenter().x + halfSizeX);
    
    int yStartPositionIndex = getGridY(camera.getCenter().y - halfSizeY);
    int yEndPositionIndex   = getGridY(camera.getCenter().y + halfSizeY);

    for ( int y = yStartPositionIndex; y <= yEndPositionIndex; y++ )
    {
        for ( int x = xStartPositionIndex; x <= xEndPositionIndex; x++)
        {
            int index = y*(info.width/gridSize)+x;
            if ( index > lastIndex )
                break;

            if ( blockGrid.find(index) != blockGrid.end() )
                result.insert(std::end(result), std::begin(blockGrid[index]), std::end(blockGrid[index]));
        }
    }
   
    return result;
}


void Map::draw(sf::RenderWindow& window, sf::View& camera)
{
    if ( !mapReady )
        return;

    window.setView(camera);

    for ( auto *block : getBlocksOnCamera(camera) )
    {
        sf::Sprite sprite;
        sprite.setTexture(*res->getTexture(block->id));
        sprite.setOrigin(sprite.getLocalBounds().width/2.0f, sprite.getLocalBounds().height/2.0f);
        sprite.setPosition(block->x, block->y);
        sprite.setRotation(block->angle);
        window.draw(sprite);
    }

    window.setView(window.getDefaultView());
}

int Map::getGridNumber(int x, int y)
{
    return getGridY(y) * (info.width/gridSize) + getGridX(x);
}

int Map::getGridX(int x)
{
    if ( x > info.width )
        x = info.width;
    
    return x/gridSize;
}

int Map::getGridY(int y)
{
    if( y > info.height )
        y = info.height;
    return y/gridSize;
}

void Map::addBlock(float blockX, float blockY, float blockAngle, int blockID)
{
    if ( blockID == -1 )
        return;
    Block *blockPointer = new Block;
    
    blockPointer->x = blockX;
    blockPointer->y = blockY;
    blockPointer->angle = blockAngle;
    blockPointer->id = blockID;

    blocks.push_back(blockPointer);
    blockGrid[getGridNumber(blockX, blockY)].emplace_back(blockPointer);

    saved = false;
}

void Map::createNew(std::string filename, int width, int height, std::string name, std::string author)
{
    clear();

    info.name = name;
    info.author = author;
    info.width = width;
    info.height = height;

    mapReady = true;
}

