#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <cstring>

#ifdef linux
#include <filesystem>
namespace fs = std::filesystem;
#endif

#if defined(_WIN32) || defined(_WIN64)
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

struct TextureData
{
    sf::Texture *texture;
    std::string name;
};

// Keeps all usefull data on one place.
class Resources
{
public:
    Resources() {}
    ~Resources();

    void loadBlocks(std::string directory);

    sf::Texture *getTexture(int id);
    int getNextKeyFromTexture(int id, int howManyKeysNeedToBeAfter = 0);
    int getPrevKeyFromTexture(int id);

    void setWindowWidth(int width) { windowWidth = width; }
    void setWindowHeight(int height) { windowHeight = height; }

    int getWindowWidth() { return windowWidth; }
    int getWindowHeight() { return windowHeight; }

    int getMinTextureKey() { 
        if ( blockTextures.empty() )
            return -1;

        return blockTextures.begin()->first;
    }
    int getMaxTextureKey() { 
        if ( blockTextures.empty() )
            return -1;

        return blockTextures.rbegin()->first;
    }

    sf::Font *getFont(unsigned int id)
    {
        if ( id < fonts.size() )
            return fonts[id];
        else
            return nullptr;
    }

    bool loadFont(std::string filename);

    void setConsole(class Console *activeConsole) { console = activeConsole; }
    class Console *getConsole() { return console; }

    void setMap(class Map *mapp) { map = mapp; }
    class Map *getMap() { return map; }


    void setBlockAngle(float angle) { if ( blockAngle >= 0.0f && blockAngle < 360.0f) blockAngle = angle; }
    float getBlockAngle() { return blockAngle; }
    void changeBlockAngle(float amount) 
    {
        if ( blockAngle + amount >= 360.0f )
            blockAngle = amount - 360.0f; 
        else
            blockAngle += amount;
    }

private:
    std::map <int, sf::Texture *> blockTextures;
    std::vector <sf::Font *> fonts;
    
    int windowWidth = 0;
    int windowHeight = 0;

    class Console *console;
    class Map *map;

    float blockAngle = 0.0f;
};
