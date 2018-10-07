#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <filesystem>
#include <string>
#include <cstring>

namespace fs = std::filesystem;

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

private:
    std::map <int, sf::Texture *> blockTextures;
    std::vector <sf::Font *> fonts;
    
    int windowWidth = 0;
    int windowHeight = 0;

    class Console *console;
};
