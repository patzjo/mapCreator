#include <iostream>
#include "UI.hpp"
#include "Resources.hpp"
#include "Utils.hpp"

void UI::draw(class Resources *res, sf::RenderWindow& window)
{
    for(auto& layer : drawOrder)
    {
        for(auto& c : layer.second)
        {
            if ( c->getVisible() ) 
                c->draw(res, window, focusedComponent==c->getID()?true:false);
        }
    }
}

void UI::sendEvent(Event event)
{
    for(auto& i : components)
        i->processEvent(event);
}

void UI::createComponent(int type, sf::Vector2i position, sf::IntRect area, std::string name)
{
    Component *component = nullptr;
    
    switch(type)
    {
        case SLIDER: 
        break;

        case BLOCK_SELECT_LIST:
        {
            BlockSelectList *bslComponent = new BlockSelectList();
            component = (Component *)bslComponent;
            component->setLayerPosition(1);

        } break;

        default: break;
    }

    if ( component != nullptr )
    {
        component->setType(type);
        component->setPosition(position);
        component->setArea(area);
        component->setName(name);
        component->ID = UI::getNextID();
        component->uiParent = this;
        component->init();

        addComponent(component);
    }
}

void UI::addComponent(Component *component)
{
    components.emplace_back(component);
    drawOrder[component->getLayerPosition()].emplace_back(component);
}

void Component::processEvent(class Event event)
{

}

void Component::update()
{

}

BlockSelectList::BlockSelectList()
{
}

void BlockSelectList::init()
{
    
    float arrowControlWidth = (float)getArea().width*0.05f;
    componentBackground.setFillColor(sf::Color::Red);
    componentBackground.setPosition((sf::Vector2f)getPosition());
    componentBackground.setSize({(float)getArea().width, (float)getArea().height});

    arrowBackground.setFillColor({0, 100, 0});
    arrowBackground.setPosition((sf::Vector2f)getPosition());
    arrowBackground.setSize({arrowControlWidth, (float)getArea().height});

    arrow.setPointCount(3);
    arrow.setRadius(getArea().height/3.0f);
    arrow.setFillColor(sf::Color::White);
    arrow.setOrigin(arrow.getRadius(), arrow.getRadius());

    textureViewStartPosition.x = getPosition().x + arrowControlWidth;
    textureViewEndPosition.x   = getPosition().x + getArea().width - arrowControlWidth;
    
    textureViewStartPosition.y = getPosition().y;
    textureViewEndPosition.y   = getPosition().y;

    textureViewWidth = textureViewEndPosition.x - textureViewStartPosition.x;
    textureViewHeight = getArea().height;

    textureViewWindowWidth = textureViewWidth/textureViewCount;

    textureViewMargin = (textureViewWidth - textureViewWindowWidth * textureViewCount) / 2.0f;

    textureSprite.setTextureRect({0, 0, textureViewWindowWidth, textureViewHeight});

    leftArrowArea.left = getPosition().x;
    leftArrowArea.top  = getPosition().y;
    leftArrowArea.width = arrowControlWidth;
    leftArrowArea.height = getArea().height;

    rightArrowArea.left     = getPosition().x + getArea().width - arrowControlWidth;    
    rightArrowArea.top      = getPosition().y;
    rightArrowArea.width    = arrowControlWidth;
    rightArrowArea.height   = getArea().height;
}

void BlockSelectList::draw(class Resources *res, sf::RenderWindow& window, bool selected)
{
    window.draw(componentBackground);
    
    arrowBackground.setPosition((sf::Vector2f)getPosition());
    window.draw(arrowBackground);

    arrowBackground.setPosition({(float)(getPosition().x+getArea().width-arrowBackground.getLocalBounds().width), 
                                 (float)getPosition().y});
    window.draw(arrowBackground);


    arrow.setPosition({(float)getPosition().x+45, (float)getPosition().y+50});
    arrow.setRotation(-90.0f);
    window.draw(arrow);

    arrow.setPosition({(float)(getPosition().x+getArea().width-arrow.getLocalBounds().width+15),
                       (float)getPosition().y+50});
    arrow.setRotation(90.0f);
    window.draw(arrow);


    sf::RectangleShape box; // Block borders
    box.setSize({(float)textureViewWindowWidth, (float)textureViewHeight-1.0f});
    box.setFillColor(sf::Color::Transparent);
    box.setOutlineColor(sf::Color::Yellow);
    box.setOutlineThickness(1.0f);

    int curTexture = firstTextureToShow;

    for(unsigned int c = 0; c < textureViewCount; c++ )
    {
        
        sf::Texture *texture = res->getTexture(curTexture);
        if(!texture)
            break;
        float x = textureViewStartPosition.x + textureViewMargin + (c*textureViewWindowWidth);
        textureSprite.setTexture(*res->getTexture(1));
        textureSprite.setPosition(x, textureViewStartPosition.y+1);
        window.draw(textureSprite);
        curTexture = res->getNextKeyFromTexture(curTexture);
        box.setPosition(x, textureViewStartPosition.y);
        window.draw(box);


        if ( curTexture == -1)
            break;
    }

}

void BlockSelectList::processEvent(class Event event)
{
    if (event.receiver != nullptr && event.receiver != this)
        return;

    switch(event.type)
    {
    case RESERVED:
    break;

    case MOUSE_CLICK_EVENT:
    {
        MouseClickEventData *data = (MouseClickEventData *)event.data;
        
        if ( !data->pressed && inRect(data->position, getArea()) )
        {
            if ( inRect(data->position, leftArrowArea) )
            {
                std::cout << "Left Arrow Pressed!" << std::endl;
                
            }
            else if ( inRect(data->position, rightArrowArea) )
            {
                std::cout << "Right Arrow Pressed!" << std::endl;

            }
            else 
            {
                std::cout << "" << std::endl;
            }


        }
        

                

    } break;

    default: break;
    }

    
}

