#include <iostream>
#include <SFML/Graphics.hpp>
#include <fstream>

#include "Resources.hpp"
#include "Map.hpp"
#include "UI.hpp"
#include "Utils.hpp"

const int screenW = 1920;
const int screenH = 1080;

const float toolAreaHeight = 0.1f;
const float toolAreaWidth  = 0.9f;

const std::string defaultFilename = "Map name";
const std::string defaultAuthor   = "Author";
const std::string defaultDescription = "Description here.";

void init(sf::RectangleShape& viewOutlines, sf::IntRect& viewArea, UI& ui, sf::View& camera, class Resources *res)
{
    viewOutlines.setFillColor(sf::Color::Transparent);
    viewOutlines.setOutlineColor(sf::Color::Blue);
    viewOutlines.setOutlineThickness(2.0f);
    viewOutlines.setSize({(float)screenW-4.0f, (float)screenH*(1.0f-toolAreaHeight) - 2.0f});
    viewOutlines.setPosition(2.0f, 2.0f);

    viewArea.left = 0;
    viewArea.top = 0;
    viewArea.height = screenH*(1.0f - toolAreaHeight);
    viewArea.width = screenW-4.0f;

    camera.setViewport({0,0,1, 1.0f-toolAreaHeight});

    ui.createComponent(BLOCK_SELECT_LIST,
                        {0, (int)(screenH*(1.0f-toolAreaHeight)+4), 
                        (int)(screenW*0.8f), (int)(screenH*toolAreaHeight)-4},
                        "Tool", res);

    Component *component = nullptr;

    int editBoxHeight = (int)(screenH*(toolAreaHeight/3))-5;
    int editBoxWidth = (int)(screenW*0.2f)-3;

    component = ui.createComponent(EDIT_BOX, {(int)(screenW*0.8f)+2, (int)(screenH*(1.0f-toolAreaHeight))+5, 
                                  editBoxWidth, editBoxHeight}, 
                                  "filename", res, true);
    ((EditBox *)component)->setBuffer(defaultFilename);

    component = ui.createComponent(EDIT_BOX, {component->getArea().left, component->getArea().top + (int)(screenH*(toolAreaHeight/3)-2), 
                                  editBoxWidth, editBoxHeight }, 
                                  "Author", res, true);
    ((EditBox *)component)->setBuffer(defaultAuthor);

    component = ui.createComponent(EDIT_BOX, {component->getArea().left, component->getArea().top + (int)(screenH*(toolAreaHeight/3)-2), 
                                  editBoxWidth, editBoxHeight }, 
                                  "Description", res, true);
    ((EditBox *)component)->setBuffer(defaultDescription);
}

int main( int argc, char **argv )
{
    bool release = true;
    Resources myResources;
    Map myMap;
    UI myUI;
    sf::RenderWindow window(sf::VideoMode(screenW, screenH), "Window");
    sf::View camera;

    camera.setSize(screenW, screenH);
    camera.setCenter(screenW/2, screenH/2);
    myMap.createNew("mapFile.map", 10000, 10000, "Map", "TeamGG");

    int cameraMinX = camera.getCenter().x;
    int cameraMinY = camera.getCenter().y;

    int cameraMaxX = myMap.getWidth() - camera.getSize().x/2;
    int cameraMaxY = myMap.getHeight() - camera.getSize().y/2;


    myResources.loadBlocks("blocks");

    if ( !myResources.loadFont("AkaashNormal.ttf") )
        std::cout << "Can't load font!" << std::endl;

    sf::RectangleShape viewOutlines;
    sf::IntRect viewArea;

    init( viewOutlines, viewArea, myUI, camera, &myResources);
    sf::Clock myClock;
    while(window.isOpen())
    {
        float deltaTime = myClock.restart().asSeconds();
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Event event;
        
        while(window.pollEvent(event))
        {
            Event myEvent;
            if ( event.type == sf::Event::Closed )
            {
                window.close();
            } else if( event.type == sf::Event::KeyReleased)
            {
                switch(event.key.code)
                {
                    case sf::Keyboard::Key::Escape:
                        window.close();
                    break;

                    default: break;
                }
            } else if ( event.type == sf::Event::MouseButtonPressed )
            {
                MouseClickEventData mouseClickEventData({event.mouseButton.x, event.mouseButton.y}, 
                                                        event.mouseButton.button, true);

                myUI.createEvent(myEvent, MOUSE_CLICK_EVENT, (void*)&mouseClickEventData, &myResources);
            } else if ( event.type == sf::Event::MouseButtonReleased )
            {
                MouseClickEventData mouseClickEventData({event.mouseButton.x, event.mouseButton.y}, 
                                                        event.mouseButton.button, false);

                myUI.createEvent(myEvent, MOUSE_CLICK_EVENT, (void*)&mouseClickEventData, &myResources);
            } else if (event.type == sf::Event::TextEntered)
            {
                
                myUI.createEvent(myEvent, TEXT_ENTERED_EVENT, (void *)&event.text.unicode);
            }

        }
    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        camera.move({-1, 0});
        if(camera.getCenter().x < cameraMinX)
            camera.setCenter(cameraMinX, camera.getCenter().y);

        std::cout << "Camera: " << camera.getCenter().x << " x " << camera.getCenter().y << std::endl; 
    }
    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        camera.move({1, 0});
        if(camera.getCenter().x > cameraMaxX)
            camera.setCenter(cameraMaxX, camera.getCenter().y);
        std::cout << "Camera: " << camera.getCenter().x << " x " << camera.getCenter().y << std::endl; 
    }
    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        camera.move({0, -1});
        if(camera.getCenter().y < cameraMinY)
            camera.setCenter(camera.getCenter().x, cameraMinY);
        std::cout << "Camera: " << camera.getCenter().x << " x " << camera.getCenter().y << std::endl; 
    }
    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        camera.move({0, 1});
        if(camera.getCenter().y > cameraMaxY)
            camera.setCenter(camera.getCenter().x, cameraMaxY);
        std::cout << "Camera: " << camera.getCenter().x << " x " << camera.getCenter().y << std::endl; 
    }

    if( sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    {
        if ( release )
        {
            if ( inRect(mousePos, viewArea) )
            {
                sf::Vector2f pos = window.mapPixelToCoords(mousePos, camera);
                int x = pos.x;
                int y = pos.y;
                myMap.addBlock(x, y, 0.0f, myUI.getBlockID());
            }
            release = false;
        }
    }
    else
        release = true;

    window.clear();
    
    myUI.update();
    myMap.draw(window, camera, &myResources);
    window.draw(viewOutlines);

    myUI.draw(&myResources, window);

    window.display();
    }

    return 0;
}

