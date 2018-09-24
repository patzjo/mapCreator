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

    virtual void processEvent(class Event *event);
    virtual void init(class Resources *res) {};

    int getID() { return ID; }
    
    bool getVisible() { return visible; }
    int getType() { return type; }

    void setPosition(sf::Vector2i position) { this->position = position; }
    void setArea(sf::IntRect area) {this->area = area; }
    void setType(int type) {this->type = type; }
    void setVisible(bool visible) {this->visible = visible; }
    void setName(std::string name) { this->name = name; }

    sf::Vector2i getPosition() { return position; }
    sf::IntRect getArea() { return area; }

    void setLayerPosition(int position) { layerPosition = position; }
    int  getLayerPosition() { return layerPosition; }


private:
    sf::Vector2i position;
    sf::IntRect  area;

    int layerPosition = 1;

    int type = 0;
    int ID = 0;
    std::string name;
    bool visible = true;
    class UI *uiParent;
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

    void createComponent( int type, sf::Vector2i position, sf::IntRect area, std::string name, class Resources *res);
    int getFocusedComponent() { return focusedComponent; }
    void setFocusedComponent(int componentID) { focusedComponent = componentID; }
    
private: 
    void addComponent(Component *component);
    
    std::vector <Component *>   components;
    std::map <int, std::vector <Component *>> drawOrder;
    
    int focusedComponent = -1;

    int getNextID() { return nextID++; }
    int nextID = 1;

};

class BlockSelectList : Component
{
    friend class Map;
public:
    BlockSelectList(); 
    ~BlockSelectList() {}
    void draw(class Resources *res, sf::RenderWindow& window, bool focused);
    void init(class Resources *res);
    void processEvent(class Event *event);

private:
    

    sf::RectangleShape componentBackground;
    sf::RectangleShape arrowBackground;
    sf::CircleShape    arrow;
    sf::Sprite         textureSprite;
    sf::IntRect        leftArrowArea;
    sf::IntRect        rightArrowArea;    

    int selected = 1;

    sf::Vector2f textureViewStartPosition;
    sf::Vector2f textureViewEndPosition;
    int textureViewWidth;
    int textureViewHeight;

    const int textureViewCount = 10;
    int textureViewMargin = 5;

    int textureViewWindowWidth;
    int firstTextureToShow = 1;
};


class EditBox : Component
{
public:
    void draw(class Resources *res, sf::RenderWindow& window, bool focused);
    void init(class Resources *res);
    void processEvent(class Event *event);

private:
    std::string buffer;
};

