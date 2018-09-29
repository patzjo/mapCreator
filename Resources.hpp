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

class Resources
{
public:
    Resources() {}
    ~Resources();

    void loadBlocks(std::string directory);

    sf::Texture *getTexture(int id);
    int getNextKeyFromTexture(int id);
    int getPrevKeyFromTexture(int id);

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

private:
    std::map <int, sf::Texture *> blockTextures;
    std::vector <sf::Font *> fonts;
    
};
