#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "Map.hpp"

enum ComponentType {SLIDER, BLOCK_SELECT_LIST, EDIT_BOX};
enum EventType {RESERVED, MOUSE_CLICK_EVENT, TEXT_ENTERED_EVENT};

struct MouseClickEventData
{
    sf::Vector2i position;
    int button;
    int pressed;
    MouseClickEventData(sf::Vector2i position, int button, int pressed) {
            this->position = position;
            this->button = button;
            this->pressed = pressed;
    }
    MouseClickEventData() {}
};

struct Event
{
    int type;
    void *data = nullptr;
    class Resources *res = nullptr;
    class Component *receiver = nullptr;
    class Component *sender = nullptr;
    class UI *ui = nullptr;
};

class Component
{
friend class UI;
public:
    virtual void draw(class Resources *res, sf::RenderWindow& window, bool focused) = 0;
    virtual void update();

    virtual bool processEvent(class Event *event);
    virtual void init(class Resources *res) {};

    int getID() { return ID; }
    
    bool getVisible() { return visible; }
    int getType() { return type; }

    void setArea(sf::IntRect area) {this->area = area; }
    void setType(int type) {this->type = type; }
    void setVisible(bool visible) {this->visible = visible; }
    void setName(std::string name) { this->name = name; }

    sf::Vector2i getPosition() { return {area.left, area.top}; }
    sf::IntRect getArea() { return area; }

    void setLayerPosition(int position) { layerPosition = position; }
    int  getLayerPosition() { return layerPosition; }
    bool getCanBeSelected() { return canBeSelected; }
    

protected:
    sf::IntRect  area;

    int layerPosition = 1;

    int type = 0;
    int ID = 0;
    std::string name;
    bool visible = true;
    class UI *uiParent = nullptr;
    class Component *componentParent = nullptr;
    bool canBeSelected = false;
};

class UI
{
public:
    void update() {};
    void draw(class Resources *res, sf::RenderWindow& window);
    void sendEvent(class Event *event);
    class Event& createEvent(class Event& event, int type, void *data, class Resources *res = nullptr, 
                            class Component *receiver = nullptr, class Component *sender = nullptr)
                            {
                                event.type = type;
                                event.data = data;
                                event.res = res;
                                event.receiver = receiver;
                                event.sender = sender;
                                event.ui = this;

                                sendEvent(&event);
                                return event;
                            };

    Component *createComponent( int type, sf::IntRect area, std::string name, class Resources *res, bool canBeSelected = false);
    int getFocusedComponent() { return focusedComponent; }
    void setFocusedComponent(int componentID) { focusedComponent = componentID; }
    void setBlockID( int ID ) { blockID = ID; }
    int getBlockID() { return blockID; }
    bool isComponentFocused() { return focusedComponent!=-1;}

    void setSelectedBlock(int selectedBlockID) { selectedBlock = selectedBlockID; };
    int getSelectedBlock() { return selectedBlock; };

    template <class T>
    T *getComponentByName(std::string name) {
        for ( auto *component : components )
            if ( component->name == name )
                return static_cast<T*>(component);

        return nullptr;
    }

private: 
    void addComponent(Component *component);
    
    std::vector <Component *>   components;
    std::map <int, std::vector <Component *>> drawOrder;
    
    int focusedComponent = -1;

    int getNextID() { return nextID++; }
    int nextID = 1;
    int blockID = -1;
    
    int selectedBlock = 1;
};

class BlockSelectList : public Component
{
    friend class Map;
public:
    BlockSelectList(); 
    ~BlockSelectList() {}
    void draw(class Resources *res, sf::RenderWindow& window, bool focused);
    void init(class Resources *res);
    bool processEvent(class Event *event);

    void updateTextures();

    void setSelectedBlock( int selectedBlockID ) 
    {
        selectedBlock = selectedBlockID;
        
        if ( uiParent )
            uiParent->setSelectedBlock(selectedBlock);
    }

private:
    struct BlockTexture
    {
        int ID = 0;
        sf::Texture *texture = nullptr;
    }; 

    sf::RectangleShape componentBackground; // Background of the component
    sf::RectangleShape arrowBackground;     // Background of the arrow
    sf::CircleShape    arrow;               // Arrow
    sf::IntRect        leftArrowArea;       // Area where to click to move the list backwards
    sf::IntRect        rightArrowArea;      // Area where to click to move the list forward

    sf::Text text;                          // ID Texts

    int selectedBlock = -1;                 // Block which is selected

    sf::Vector2f textureViewStartPosition;  
    sf::Vector2f textureViewEndPosition;
    int textureViewWidth;                   // Width of the area where blocks can be placed
    int textureViewHeight;                  // Height of the area where blocks cen be placed

    const int textureViewCount = 10;        // How many textures in the view by default
    int textureViewMargin = 5;              // View area margin.

    float blockWidthInView;                 // Block width in the view
    float blockHeightInView;                // Block height in the view
    int firstTextureToShow = 1;             // Position where list begins
                                            // This value is overwritten in the init

    std::vector <BlockTexture> textures;    // Keeps all visible blocks
    class Resources *res;

    sf::RectangleShape blockOutline; // Box that contains block
    float outlineSize = 1.0f;        // Size of box outlines

};


class EditBox : public Component
{
public:
    void draw(class Resources *res, sf::RenderWindow& window, bool focused);
    void init(class Resources *res);
    bool processEvent(class Event *event);

    bool addCharacter(int character);
    std::string getBuffer() { return buffer; }
    void setBuffer(std::string str) { buffer = str; setVisibleString( buffer ); }
    void setVisibleString(std::string);

    
protected:
    std::string buffer;
    
    sf::RectangleShape componentBackground;
    sf::Text text;
};

class MessageBox : public Component
{
public:
    void draw(class Resource *res, sf::RenderWindow& window, bool focused);
    void init( class Resources *res );
    bool processEvent( class Event *event );

    enum MESSAGE_BOX_TYPE { MSG_OK, MSG_OK_CANCEL, MSG_CUSTOM };

protected:
    std::string title;

    int type;
};