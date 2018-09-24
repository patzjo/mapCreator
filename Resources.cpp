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

int Resources::getNextKeyFromTexture(int id)
{
    auto it = blockTextures.find(id);
    auto nextOne = std::next(it);

    if ( nextOne != blockTextures.end() )
        return nextOne->first;
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