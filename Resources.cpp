#include "Resources.hpp"

Resources::~Resources()
{
    for(auto& texture : blockTextures)
    {
        if ( texture.second )
            delete texture.second;
    }
}

sf::Texture *Resources::getTexture(int id)
{
    if ( blockTextures.find(id) != blockTextures.end() )
        return blockTextures[id];
    
    return nullptr;
}

int Resources::getNextKeyFromTexture(int id, int howManyKeysNeedToBeAfter)
{
    auto it = blockTextures.find(id);
    auto nextOne = std::next(it);

    if ( howManyKeysNeedToBeAfter == 0 )
    {
        if ( nextOne != blockTextures.end() )
            return nextOne->first;
        else
            return -1;
    }
    else
    {
        bool enoughtKeys = true;
        int key = getNextKeyFromTexture(id);
        int nextKey = key;
        for ( unsigned int c = 0; c <= howManyKeysNeedToBeAfter && enoughtKeys; c++ )
        {
            if ( key == -1 )
                enoughtKeys = false;
            else
                key = getNextKeyFromTexture(key);
        }
        
        if ( enoughtKeys )
            return nextKey;
        else
            return -1;
    }
}

int Resources::getPrevKeyFromTexture(int id)
{
    auto it = blockTextures.find(id);
    auto prevOne = std::prev(it);

    if ( prevOne != blockTextures.end() )
        return prevOne->first;
    else
        return -1;
}

void Resources::loadBlocks(std::string directory)
{
     std::cout << "Loading blocks..." << std::endl;
    for(auto& p : fs::directory_iterator(directory) )
    {
        sf::Texture *texture = nullptr;
        fs::path path;
        std::string pathAndFilename;
        std::string name;
        std::string filename;
        std::string id;
        size_t idPositionEnd = 0;
        path = p;
        pathAndFilename = path.string();
       
        filename = pathAndFilename.substr(pathAndFilename.find_last_of("/\\")+1);
        idPositionEnd = filename.find_first_of('_');
        id = filename.substr(0, idPositionEnd);
        name = filename.substr(idPositionEnd+1, filename.find_last_of('.')-(idPositionEnd+1));

        std::cout << "\t" << id << "\t= " << pathAndFilename << std::endl;
        texture = new sf::Texture();
        texture->loadFromFile(pathAndFilename);
        blockTextures.emplace(stoi(id), texture);
    }
}

bool Resources::loadFont(std::string filename)
{
    sf::Font *font = new sf::Font;

    if ( !font->loadFromFile(filename) )
    {
        std::cout << "Error loading font!" << std::endl;
        delete font;
    }

    fonts.emplace_back(font);
    
    return font!=nullptr;
}
