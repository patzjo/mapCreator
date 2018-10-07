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

void UI::sendEvent(class Event *event)
{
    for(auto& i : components)
        i->processEvent(event);
}

Component *UI::createComponent(int type, sf::IntRect area, std::string name, class Resources *res, bool canBeSelected)
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

        case EDIT_BOX:
        {
            EditBox *editBoxComponent = new EditBox();
            component = (Component *)editBoxComponent;
            component->setLayerPosition(1);
        } break;

        default: break;
    }

    if ( component != nullptr )
    {
        component->setType(type);
        component->setArea(area);
        component->setName(name);
        component->ID = UI::getNextID();
        component->uiParent = this;
        component->canBeSelected = canBeSelected;
        component->init(res);

        addComponent(component);
    }

    return component;
}

void UI::addComponent(Component *component)
{
    components.emplace_back(component);
    drawOrder[component->getLayerPosition()].emplace_back(component);
}

bool Component::processEvent(class Event *event)
{
    if (event->receiver != nullptr && event->receiver != this)
        return false;


    switch(event->type)
    {
    case RESERVED:
    break;

    case MOUSE_CLICK_EVENT:
    {
        MouseClickEventData *data = (MouseClickEventData *)event->data;
        
        if ( !data->pressed && inRect(data->position, getArea()) )
        {
            if ( getCanBeSelected() )
            {
                event->ui->setFocusedComponent(this->getID());
            }
            
        }
    } break;

    default: break;
    }

    return true;
}

void Component::update()
{

}

BlockSelectList::BlockSelectList()
{
}

void BlockSelectList::init(class Resources *res)
{
    float arrowControlWidth = (float)getArea().width*0.05f;
    this->res = res;

    componentBackground.setFillColor(sf::Color::Transparent);
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
    textureViewEndPosition.y   = getPosition().y + getArea().height;

    textureViewWidth = textureViewEndPosition.x - textureViewStartPosition.x;
    textureViewHeight = getArea().height;

    int textureViewWindowWidthTemp = textureViewWidth/textureViewCount;
    //int textureViewWindowWidth = textureViewWidth/textureViewCount;

    //textureViewMargin = (textureViewWidth - textureViewWindowWidth * textureViewCount) / 2.0f;
    textureViewMargin = (textureViewWidth - textureViewWindowWidthTemp * textureViewCount) / 2.0f;
    blockWidthInView = (textureViewWidth-2*textureViewMargin)/textureViewCount - outlineSize;

    blockHeightInView = textureViewHeight;

    // Outline of selectable blocks in view
//    blockOutline.setSize({(float)blockWidthInView, (float)textureViewHeight-1.0f});
    blockOutline.setSize({(float)blockWidthInView-outlineSize, (float)textureViewHeight-outlineSize-1.0f});
    blockOutline.setFillColor(sf::Color::Transparent);
    blockOutline.setOutlineThickness(outlineSize);


    // Text
    text.setFont(*res->getFont(0));
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::Green);

    //textureSprite.setTextureRect({0, 0, textureViewWindowWidth, textureViewHeight});

    leftArrowArea.left      = getPosition().x;
    leftArrowArea.top       = getPosition().y;
    leftArrowArea.width     = arrowControlWidth;
    leftArrowArea.height    = getArea().height;

    rightArrowArea.left     = getPosition().x + getArea().width - arrowControlWidth;    
    rightArrowArea.top      = getPosition().y;
    rightArrowArea.width    = arrowControlWidth;
    rightArrowArea.height   = getArea().height;

    textures.resize(textureViewCount);

    firstTextureToShow = res->getMinTextureKey();
    setSelectedBlock(firstTextureToShow);
    updateTextures();    
}

void BlockSelectList::draw(class Resources *res, sf::RenderWindow& window, bool selected)
{
    window.draw(componentBackground);

    // Left arrow
    arrowBackground.setPosition((sf::Vector2f)getPosition());
    window.draw(arrowBackground);

    arrow.setPosition({(float)getPosition().x+45, (float)getPosition().y+50});
    arrow.setRotation(-90.0f);
    window.draw(arrow);

    // Right arrow
    arrowBackground.setPosition({(float)(getPosition().x+getArea().width-arrowBackground.getLocalBounds().width), 
                                 (float)getPosition().y});
    window.draw(arrowBackground);
    
    arrow.setPosition({(float)(getPosition().x+getArea().width-arrow.getLocalBounds().width+15),
                       (float)getPosition().y+50});
    arrow.setRotation(90.0f);
    window.draw(arrow);

    float selectedBlockX = -1.0f;

    float x = textureViewStartPosition.x + textureViewMargin;
    for(unsigned int c = 0; c < textureViewCount; c++ )
    {
        sf::Sprite textureSprite = {};
        
        sf::Texture *texture = textures[c].texture;
        if(!texture)
            break;

        float xScale = blockWidthInView/texture->getSize().x;
        float yScale = blockHeightInView/texture->getSize().y;

        textureSprite.setTexture(*texture);
        textureSprite.setScale(xScale, yScale);
        textureSprite.setPosition(x, textureViewStartPosition.y+1);
        window.draw(textureSprite);

        if ( textures[c].ID ==  selectedBlock )
        {
            selectedBlockX = x;
        }
        else
        {
            blockOutline.setPosition(x+outlineSize, textureViewStartPosition.y+outlineSize);
            window.draw(blockOutline);
        }


        text.setPosition(x + blockWidthInView/2 - (text.getLocalBounds().width/2),  
                        textureViewEndPosition.y - text.getLocalBounds().height*2 );
        text.setString(std::to_string(textures[c].ID));

        window.draw(text);

        x += + blockWidthInView + outlineSize;
    }
    float selectedBlockOutlineSize = 2.0f*outlineSize;
    if ( selectedBlockX >= 0.0f )
    {
        blockOutline.setSize({(float)blockWidthInView-selectedBlockOutlineSize, (float)textureViewHeight-selectedBlockOutlineSize-2});

        blockOutline.setFillColor(sf::Color(255,0,255,155));
        blockOutline.setOutlineColor(sf::Color::Red);
        blockOutline.setOutlineThickness(selectedBlockOutlineSize);
        blockOutline.setPosition(selectedBlockX+selectedBlockOutlineSize, textureViewStartPosition.y+selectedBlockOutlineSize);
        window.draw(blockOutline);

        blockOutline.setSize({(float)blockWidthInView-outlineSize, (float)textureViewHeight-outlineSize-2});
        blockOutline.setOutlineThickness(outlineSize);
        blockOutline.setFillColor(sf::Color::Transparent);
        blockOutline.setOutlineColor(sf::Color::White);
    }



}

bool BlockSelectList::processEvent(class Event *event)
{
    if (!Component::processEvent(event) )
        return false;

    switch(event->type)
    {
    case RESERVED:
    break;

    case MOUSE_CLICK_EVENT:
    {
        MouseClickEventData *data = (MouseClickEventData *)event->data;
        
        if ( !data->pressed && inRect(data->position, getArea()) )
        {
            if ( inRect(data->position, leftArrowArea) )
            {
                int prevID = event->res->getPrevKeyFromTexture(firstTextureToShow);
                if ( prevID != -1 )
                    firstTextureToShow = prevID;
                updateTextures();
            }
            else if ( inRect(data->position, rightArrowArea) )
            {
                int nextID = event->res->getNextKeyFromTexture(firstTextureToShow, 9);
                if ( nextID != -1 )
                    firstTextureToShow = nextID;
                updateTextures();
            }
            else 
            {
                int posX = data->position.x - textureViewStartPosition.x;
                int index = posX / blockWidthInView;

                if ( index < textureViewCount )
                    if ( textures[index].texture )
                    {
                        event->ui->setBlockID(textures[index].ID);
                        setSelectedBlock(textures[index].ID);
                    }
                     
           }
        }
    } break;

    default: break;
    }

    return true;
}

void BlockSelectList::updateTextures()
{
    int curTexture = firstTextureToShow;
    for(unsigned int c = 0; c < textureViewCount; c++ )
    {
        sf::Texture *texture = res->getTexture(curTexture);
        
        textures[c].ID = curTexture;
        textures[c].texture = texture;

        if(!texture)
            break;

        curTexture = res->getNextKeyFromTexture(curTexture);
        if ( curTexture == -1)
            break;
    }
}

void EditBox::init(class Resources *res)
{
    componentBackground.setPosition({(float)getArea().left, (float)getArea().top});
    componentBackground.setFillColor({20,20,20});
    componentBackground.setOutlineThickness(1.0f);
    componentBackground.setSize({(float)getArea().width, (float)getArea().height});

    text.setFont(*res->getFont(0));
    text.setCharacterSize(getArea().height*0.80f);
    text.setFillColor(sf::Color::White);
    text.setPosition({(float)getArea().left+4.0f, (float)getArea().top-4});
}

void EditBox::draw(class Resources *res, sf::RenderWindow& window, bool focused)
{
    if(focused)
        componentBackground.setOutlineColor(sf::Color::Red);
    else
        componentBackground.setOutlineColor(sf::Color::Green);
    
    window.draw(componentBackground);
    

    if ( !buffer.empty() )
        window.draw(text);

}


bool EditBox::processEvent(class Event *event)
{
    if ( !Component::processEvent(event) )
        return false;

    switch(event->type)
    {
        case TEXT_ENTERED_EVENT:
        {
            if (event->ui->getFocusedComponent() == this->getID())
            {
                int *character = (int *)event->data;
                if ( addCharacter(*character) )
                    event->ui->setFocusedComponent(-1);
            }
        } break;

        default: break;
    }
    return true;
}

// Return true if we want to remove focus from the component
bool EditBox::addCharacter(int character)
{

    if ( isprint(character) )
    {
        std::cout << (char)character;
        buffer.push_back(character);
        setVisibleString(buffer);
    }
    else
    {
        if ( character == 13 || character == 27) // Enter or escape
        {
            return true;
        } else if ( character == 8) // Backspace
        {
            if ( !buffer.empty() )
            {
                buffer.pop_back();
                setVisibleString(buffer);
            }
        }
    }

    return false;
}
// Calculates end of string that does not overlap editbox
void EditBox::setVisibleString(std::string str)
{
    std::string anotherString;
    text.setString("");
    
    while( (text.getLocalBounds().width+10) < getArea().width && !str.empty())
    {
        anotherString.insert(0, 1, str.back());
        str.pop_back();
        text.setString(anotherString);
    }
}
