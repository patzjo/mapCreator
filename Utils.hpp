#pragma once
#include <SFML/Graphics.hpp>

inline bool inRect(sf::Vector2i position, sf::IntRect area)
{
    if ( position.x > area.left && position.x < area.left + area.width &&
         position.y > area.top && position.y < area.top + area.height)
         return true;

    return false;
}
