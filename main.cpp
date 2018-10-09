#include <iostream>
#include <SFML/Graphics.hpp>
#include <fstream>

#include "Resources.hpp"
#include "Map.hpp"
#include "UI.hpp"
#include "Console.hpp"
#include "Utils.hpp"

const int screenW = 1920;
const int screenH = 1080;

const float TextShownTime = 3.0f;

const float toolAreaHeight = 0.1f;
const float toolAreaWidth  = 0.9f;

const float blockRotateSpeed = 200.0f;
const float cameraMoveSpeed = 5000.0f;
const float zoomSpeed = 10.0f;

const std::string defaultFilename       = "DefaultMapName";
const std::string defaultAuthor         = "DefaultAuthor";
const std::string defaultDescription    = "MapTitle";

float getMin(float v1, float v2)
{
    return v1<v2?v1:v2;
}

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
    Console myConsole;
    sf::RenderWindow window(sf::VideoMode(screenW, screenH), "Window");
    window.setFramerateLimit(60.0f);
   
    sf::View camera;
    sf::Sprite selectedBlockSprite;
    sf::Text text;

    myResources.setWindowWidth(screenW);
    myResources.setWindowHeight(screenH);
    myResources.setConsole(&myConsole);
    myResources.setMap(&myMap);

    myMap.init(&myResources);
    camera.setSize(screenW, screenH);
    camera.setCenter(screenW/2, screenH/2);
//    myMap.createNew("mapFile.map", 10000, 10000, "Map", "TeamGG");

    myResources.loadBlocks("blocks");

//    std::cout << "Loading fonts" << std::endl; 
    myConsole.addLogLine("Loading fonts");
    if ( !myResources.loadFont("AkaashNormal.ttf") )
    {
        std::cout << "Can't load font!" << std::endl;
    }

    sf::RectangleShape viewOutlines;
    sf::IntRect viewArea;
   
    myConsole.addLogLine("Setting up the console.");
//    std::cout << "Setting up console." << std::endl;
    myConsole.init(&myResources);

//    std::cout << "Initializing." << std::endl; 
    myConsole.addLogLine("Initializing.");
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

/********************************** EVENTS ***********************************/

        while(window.pollEvent(event))
        {
            Event myEvent;

            if( event.type == sf::Event::KeyReleased && !myUI.isComponentFocused() && !myConsole.isActive())
            {
                switch(event.key.code)
                {
                    case sf::Keyboard::Key::Escape:
                            if ( !myConsole.isActiveOrHiding() )
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

                    case sf::Keyboard::Key::Enter:
                    {
                        if ( !myConsole.isActive() )
                            myConsole.show();
                    }
                    break;

                    default: break;
                }
            } 

            if ( event.type == sf::Event::Closed )
            {
                window.close();
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
                if ( myConsole.isActive() )
                    myConsole.addInput(event.text.unicode);
                else
                    myUI.createEvent(myEvent, TEXT_ENTERED_EVENT, (void *)&event.text.unicode);
            } else if ( event.type == sf::Event::MouseWheelScrolled )
            {
                if ( event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel )
                {
                    if ( event.mouseWheelScroll.delta > 0)                       
                        myResources.changeBlockAngle(-blockRotateSpeed * deltaTime);
                    else
                        myResources.changeBlockAngle(blockRotateSpeed * deltaTime);
                }
            }

        }

/********************************** MOVEMENT ***********************************/
    if ( !myUI.isComponentFocused() && !myConsole.isActive() && myMap.getReady() )
    {
        

        if ( sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            camera.move({-cameraMoveSpeed*deltaTime, 0});
            textTimeInScreen = TextShownTime;
        }
        if ( sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            camera.move({cameraMoveSpeed*deltaTime, 0});
            textTimeInScreen = TextShownTime;
        }
        if ( sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            camera.move({0, -cameraMoveSpeed*deltaTime});
            textTimeInScreen = TextShownTime;
        }
        if ( sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            camera.move({0, cameraMoveSpeed*deltaTime});
            textTimeInScreen = TextShownTime;
        }


        if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Add))
        {
            camera.setSize({camera.getSize().x * (1.0f - zoomSpeed*deltaTime), 
                            camera.getSize().y * (1.0f - zoomSpeed*deltaTime)});
            textTimeInScreen = TextShownTime;
        }

        if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Subtract))
        {
            camera.setSize({camera.getSize().x * (1.0f + zoomSpeed*deltaTime), 
                            camera.getSize().y * (1.0f + zoomSpeed*deltaTime)});
            textTimeInScreen = TextShownTime;
        }
       
        int smallestAspectFactor = (int)getMin(myMap.getWidth()/screenW, myMap.getHeight()/screenH);
        
        int cameraMinX = 0; 
        int cameraMaxX = 0;
        int cameraMinY = 0;
        int cameraMaxY = 0;

        if ( camera.getSize().x >= (float)smallestAspectFactor*screenW || 
             camera.getSize().y >= (float)smallestAspectFactor*screenH)
        {
            cameraMinX = myMap.getWidth()/2.0f;
            cameraMaxX = cameraMinX;

            cameraMinY = camera.getSize().y/2.0f;
            cameraMaxY = cameraMinY;

            camera.setSize({(float)smallestAspectFactor*screenW, (float)smallestAspectFactor*screenH});
        }
        else
        {
            cameraMinX = camera.getSize().x/2.0f;
            cameraMaxX = myMap.getWidth() - camera.getSize().x/2;
            cameraMinY = camera.getSize().y/2.0f;
            cameraMaxY = myMap.getHeight() - camera.getSize().y/2;
        }

        if(camera.getCenter().x < cameraMinX)
            camera.setCenter(cameraMinX, camera.getCenter().y);
        
        if(camera.getCenter().x > cameraMaxX)
            camera.setCenter(cameraMaxX, camera.getCenter().y);
        
        if(camera.getCenter().y < cameraMinY)
            camera.setCenter(camera.getCenter().x, cameraMinY);
        
        if(camera.getCenter().y > cameraMaxY)
            camera.setCenter(camera.getCenter().x, cameraMaxY);

        
    }

    if( sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    {
        if ( release )
        {
            if ( inRect(mousePos, viewArea) )
            {
                sf::Vector2f pos = window.mapPixelToCoords(mousePos, camera);
                if ( pos.x < myMap.getWidth() && pos.x >= 0.0f && 
                     pos.y < myMap.getHeight() && pos.y >= 0.0f)
                {
                    myMap.addBlock(pos.x, pos.y, myResources.getBlockAngle(), myUI.getSelectedBlock());
                }
                else
                    myConsole.addLogLine("Error: Block out of the map boundaries!");
            }
            release = false;
        }
    }
    else
        release = true;

    

/********************************** UPDATE ***********************************/

    window.clear();
    
    myUI.update();
    myConsole.update(deltaTime);

/*********************************** DRAW ************************************/
    myMap.draw(window, camera);
    window.draw(viewOutlines);

    myUI.draw(&myResources, window);
    
/********************* DRAW SELECTED BLOCK AT MOUSE POSITION *****************/
    if ( inRect(mousePos, viewArea) )
    {

        selectedBlockSprite.setOrigin(selectedBlockSprite.getLocalBounds().width/2.0f, selectedBlockSprite.getLocalBounds().height/2.0f);
        selectedBlockSprite.setColor(sf::Color(255, 255, 255, 128));
        sf::Vector2f pos = window.mapPixelToCoords(mousePos, camera);
        selectedBlockSprite.setRotation(myResources.getBlockAngle());
        selectedBlockSprite.setPosition(pos);
        window.setView(camera);
        window.draw(selectedBlockSprite);
        window.setView(window.getDefaultView());
    }

/************************** DRAW CAMERA POSITION TEXT ************************/
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
        str += std::to_string((int)(myMap.getHeight()-camera.getSize().y)) + "\n";

        str += "Scale: ";
        str += std::to_string(camera.getSize().x / (float)screenW);
        
        if ( textTimeInScreen <= 1.0f)
            text.setFillColor(sf::Color(255,255,255, 255*textTimeInScreen));

        text.setString(str);
        window.draw(text);

        textTimeInScreen -= deltaTime;
    }

    myConsole.draw(window);

    window.display();
    }

    return 0;
}

