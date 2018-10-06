#include <iostream>
#include <SFML/Graphics.hpp>
#include <fstream>

#include "Resources.hpp"
#include "Map.hpp"
#include "UI.hpp"
#include "Utils.hpp"

const int screenW = 1920;
const int screenH = 1080;

const float TextShownTime = 3.0f;

const float toolAreaHeight = 0.1f;
const float toolAreaWidth  = 0.9f;

const std::string defaultFilename = "DefaultMapName";
const std::string defaultAuthor   = "DefaultAuthor";
const std::string defaultDescription = "MapTitle";

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
                                  "author", res, true);
    ((EditBox *)component)->setBuffer(defaultAuthor);

    component = ui.createComponent(EDIT_BOX, {component->getArea().left, component->getArea().top + (int)(screenH*(toolAreaHeight/3)-2), 
                                  editBoxWidth, editBoxHeight }, 
                                  "name", res, true);
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
    sf::Sprite selectedBlockSprite;
    sf::Text text;

    camera.setSize(screenW, screenH);
    camera.setCenter(screenW/2, screenH/2);
    myMap.createNew("mapFile.map", 10000, 10000, "Map", "TeamGG");

    int cameraMinX = camera.getCenter().x;
    int cameraMinY = camera.getCenter().y;

    int cameraMaxX = myMap.getWidth() - camera.getSize().x/2;
    int cameraMaxY = myMap.getHeight() - camera.getSize().y/2;

    std::cout << "Loading blocks." << std::endl; 
    myResources.loadBlocks("blocks");

    std::cout << "Loading fonts" << std::endl; 
    if ( !myResources.loadFont("AkaashNormal.ttf") )
        std::cout << "Can't load font!" << std::endl;

    sf::RectangleShape viewOutlines;
    sf::IntRect viewArea;
   
    std::cout << "Initializing." << std::endl; 
    init( viewOutlines, viewArea, myUI, camera, &myResources);
    int oldTexture = -1;
    
    // Font
    text.setFont(*myResources.getFont(0));
    text.setCharacterSize(12);
    float textTimeInScreen = 0.0f;
    
    sf::Clock myClock;
    while(window.isOpen())
    {
        float deltaTime = myClock.restart().asSeconds();
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Event event;

        if ( oldTexture != myUI.getSelectedBlock() )
        {
            oldTexture = myUI.getSelectedBlock();
            selectedBlockSprite.setTexture(*myResources.getTexture(myUI.getSelectedBlock()), true);
        }

        while(window.pollEvent(event))
        {
            Event myEvent;
            if ( event.type == sf::Event::Closed )
            {
                window.close();
            } else if( event.type == sf::Event::KeyReleased && !myUI.isComponentFocused())
            {
                switch(event.key.code)
                {
                    case sf::Keyboard::Key::Escape:
                        window.close();
                    break;

                    case sf::Keyboard::Key::F1:
                    {
                        std::string fname  = myUI.getComponentByName<EditBox>("filename")->getBuffer();
                        std::string name   = myUI.getComponentByName<EditBox>("name")->getBuffer();
                        std::string author = myUI.getComponentByName<EditBox>("author")->getBuffer();
                        
                        // TODO(Jonne): Better validation :P
                        if ( !fname.empty() && !name.empty() && !author.empty() )
                        {
                            std::cout << "Saving '" << name << "' map from '" << author << "' to '" << fname << ".map'" << std::endl;
                            
                            myMap.setFilename(fname);
                            myMap.setName(name);
                            myMap.setAuthor(author);
                            myMap.saveMap(); 
                        }
                    }
                    break;

                    case sf::Keyboard::Key::F2:
                    {
                        myMap.loadMap("myFirstMap.map");
                        myUI.getComponentByName<EditBox>("filename")->setBuffer(myMap.getFilename());
                        myUI.getComponentByName<EditBox>("name")->setBuffer(myMap.getName());
                        myUI.getComponentByName<EditBox>("author")->setBuffer(myMap.getAuthor());
                    }
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
    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        camera.move({-1, 0});
        if(camera.getCenter().x < cameraMinX)
            camera.setCenter(cameraMinX, camera.getCenter().y);
        textTimeInScreen = TextShownTime;
    }
    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        camera.move({1, 0});
        if(camera.getCenter().x > cameraMaxX)
            camera.setCenter(cameraMaxX, camera.getCenter().y);
        textTimeInScreen = TextShownTime;
    }
    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        camera.move({0, -1});
        if(camera.getCenter().y < cameraMinY)
            camera.setCenter(camera.getCenter().x, cameraMinY);
        textTimeInScreen = TextShownTime;
    }
    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        camera.move({0, 1});
        if(camera.getCenter().y > cameraMaxY)
            camera.setCenter(camera.getCenter().x, cameraMaxY);
        textTimeInScreen = TextShownTime;
    }

    if( sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    {
        if ( release )
        {
            if ( inRect(mousePos, viewArea) )
            {
                sf::Vector2f pos = window.mapPixelToCoords(mousePos, camera);
                myMap.addBlock(pos.x, pos.y, 0.0f, myUI.getSelectedBlock());

                std::cout << "Mouse: " << mousePos.x << "x" << mousePos.y << std::endl;
                std::cout << "Map: " << pos.x << "x" << pos.y << std::endl;
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
    
    if ( inRect(mousePos, viewArea) )
    {

        selectedBlockSprite.setOrigin(selectedBlockSprite.getLocalBounds().width/2.0f, selectedBlockSprite.getLocalBounds().height/2.0f);
        selectedBlockSprite.setColor(sf::Color(255, 255, 255, 128));
        sf::Vector2f pos = window.mapPixelToCoords(mousePos, camera);
        selectedBlockSprite.setPosition(pos);
        window.setView(camera);
        window.draw(selectedBlockSprite);
        window.setView(window.getDefaultView());
    }

    if ( textTimeInScreen > 0.0f )
    {
        std::string str;
        text.setPosition(10.0f, 10.0f);
        text.setFillColor(sf::Color::White);
        str = "X: ";
        str += std::to_string((int)(camera.getCenter().x-camera.getSize().x/2.0f)) + " / " ;
        str += std::to_string((int)(myMap.getWidth()-camera.getSize().x)) + "\n";

        str += "Y: ";
        str += std::to_string((int)(camera.getCenter().y-camera.getSize().y/2.0f)) + " / "; 
        str += std::to_string((int)(myMap.getHeight()-camera.getSize().y));
        
        if ( textTimeInScreen <= 1.0f)
            text.setFillColor(sf::Color(255,255,255, 255*textTimeInScreen));

        text.setString(str);
        window.draw(text);

        textTimeInScreen -= deltaTime;
    }

    window.display();
    }

    return 0;
}

