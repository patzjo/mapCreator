#include "Console.hpp"
#include "Resources.hpp"
#include <functional>
#include <algorithm>
#include <filesystem>

#include "Map.hpp"

namespace fs = std::filesystem;


void Console::init(class Resources *res)
{
    resources = res;

    background.setFillColor(sf::Color(0, 0, 0, 128));
    background.setOutlineColor(sf::Color::Red);
    background.setOutlineThickness(outlineThickness);
    background.setSize({(float)res->getWindowWidth(), (float)res->getWindowHeight()*consoleSize});


    logBufferText.setFont(*res->getFont(0));
    logBufferText.setFillColor(sf::Color::Yellow);
    logBufferText.setCharacterSize(16.0f);

    commandBufferText.setFont(*res->getFont(0));
    commandBufferText.setFillColor(sf::Color::White);
    commandBufferText.setCharacterSize(20.0f);

    cursor.setSize({2.0f, 17.0f});
    cursor.setFillColor(sf::Color::White);
    
    logAreaHeight = background.getSize().y - 30.0f;
    logLineCount = (int)(logAreaHeight/logBufferText.getCharacterSize()+0.5f);

    addCommand("help", std::bind(&Console::helpCommand, this, std::placeholders::_1));    
    addCommand("new", std::bind(&Console::newCommand, this, std::placeholders::_1));    
    addCommand("load", std::bind(&Console::loadCommand, this, std::placeholders::_1));    
    addCommand("setangle", std::bind(&Console::setAngle, this, std::placeholders::_1));    
}

void Console::updateLogBufferPosition()
{
    int position = logBuffer.size() - logLineCount +1;
    if ( position < 0 )
        position = 0;

    logBufferPosition = position;    
}

void Console::draw(sf::RenderWindow& window)
{
    if ( !active && !hideAnimation )
        return;

    window.draw(background);

    float yIncrease = logAreaHeight/logLineCount;
    float y = background.getPosition().y;
    float x = 5.0f; // Margin
    for ( unsigned int c = 0; c < logLineCount-1 && c < logBuffer.size(); c++)
    {
        logBufferText.setString(logBuffer[logBufferPosition + c]);
        logBufferText.setPosition(x, y);
        window.draw(logBufferText);
        y += yIncrease;    
    }

    y = background.getPosition().y + logAreaHeight;
    commandBufferText.setPosition(x, y);

    if ( !commandBuffer.empty() )
        commandBufferText.setString(": " + commandBuffer);
    else
        commandBufferText.setString(": ");


    window.draw(commandBufferText);

    if ( blink && !hideAnimation )
    {
        float posX = commandBufferText.getPosition().x + commandBufferText.getGlobalBounds().width + 4.0f;
        float posY = y + 6.0f;

        cursor.setPosition(posX, posY);
        window.draw(cursor);
    }
}

void Console::update(float deltaTime)
{
    float y = background.getPosition().y;
    
    if ( active || hideAnimation )
    {
        if ( !hideAnimation )
        {
            if ( y < 0.0f )
                y += animateSpeed * deltaTime;
            else
                y = 0.0f;
        }
        else
        {
            if ( y > -background.getSize().y)
                y -= animateSpeed * deltaTime;
            else
                hideAnimation = false;
        }
    }
    
    background.setPosition(0.0f, y);

    blinkyCurrentTime += deltaTime;
    if(blinkyCurrentTime > blinkyTime)
    {
        blink = !blink;
        blinkyCurrentTime = 0.0f;
    }

}

void Console::show()
{
    if ( !hideAnimation )
        background.setPosition(0.0f, -background.getSize().y);
    active = true;
}

void Console::hide()
{ 
    active = false;
    hideAnimation = true;
}

void Console::addLogLine(std::string logLine)
{
    if ( logBuffer.size() >= maxLogLines )
        logBuffer.erase(logBuffer.begin());

    logBuffer.push_back(logLine);

    if ( keepLogBufferPositionUpdated )
        updateLogBufferPosition();
}

void Console::addInput(int character)
{
    if ( std::isprint(character))
    {
        commandBuffer.push_back(character);
    }
    else
    {
        if ( character == 13) // Enter
        {
            if ( !commandBuffer.empty() )
            {
                execute(commandBuffer);
                commandBuffer.clear();
            }
        } else if ( character == 8) // Backspace
        {
            if ( !commandBuffer.empty() )
            {
                commandBuffer.pop_back();
            }
        } else if ( character == 27 ) // Escape
        {
            if ( !commandBuffer.empty() )   // First escape clears the buffer, second hides
                commandBuffer.clear();
            else
                hide();
        }
    }

    blinkyCurrentTime = 0.0f;
    blink = true;
}

void Console::addCommand(std::string commandName, std::function<void(std::vector <std::string>)> func)
{
    std::transform(commandName.begin(), commandName.end(), commandName.begin(), ::tolower);
    commands[commandName] = func;
}


void Console::execute(std::string command)
{
    addLogLine("Executing: '" + commandBuffer + "'");
    std::string onlyCommand = command.substr(0, command.find_first_of(' '));
    std::transform(onlyCommand.begin(), onlyCommand.end(), onlyCommand.begin(), ::tolower);

    if ( commands.find(onlyCommand) == commands.end() )
    {
        addLogLine("\tCommand not found!");
    }
    else
    {
        commands[onlyCommand](getArgs(command));
    }

}

std::vector <std::string>Console::getArgs(std::string str)
{
    std::vector <std::string> result;
    std::string arg;
    std::stringstream ss(str);
    
    while(ss >> arg)
        result.push_back(arg);

    return result;
}

void Console::helpCommand(std::vector <std::string> args)
{
    addLogLine("Help");
    addLogLine("\tCommand\t\tArguments\t\t\t\t\t\tDescription");
    addLogLine("\t   help\t\t-\t\tThis help");
    addLogLine("\t   new\t\t[width] [height]\t\tCreates new map");
    
}

void Console::newCommand(std::vector <std::string> args)
{
    if ( args.size() != 3 )
    {
        addLogLine("\tError: Too few parameters! (new [width] [height])");
    }
    else
    {
        if ( resources->getMap()->isSaved() )
        {
            addLogLine("Creating new " + args[1] + "x" + args[2] + " map.");
            resources->getMap()->createNew("Filename", std::stoi(args[1]), std::stoi(args[2]), "Name", "Author");
        } else
        {
            addLogLine("Map havent been saved, please use (new [width] [height] nosave) to proceed without saving.");
        }

    }
}

void Console::loadCommand(std::vector <std::string> args)
{
    if (args.size() == 2)
    {
        bool returnCode = false;
        if ( args[1].find(".map") == std::string::npos )
            returnCode = resources->getMap()->loadMap(args[1] + ".map");
        else
            returnCode = resources->getMap()->loadMap(args[1]);
        
        if ( !returnCode )
        {
            addLogLine("\tCan't open mapfile. Check your spelling.");
        }
        else
            addLogLine("\tMap loaded.");

    } else
    {
        addLogLine("\tWrong number of arguments.");
        addLogLine("All maps that you can load:");
        for(auto& p: fs::directory_iterator("."))
        {
            fs::path path = p;
            if ( path.extension().string() == ".map" )
            {
                addLogLine("\t-> " + path.filename().string());
            }
            
        }
        
    }

}

void Console::setAngle(std::vector <std::string> args)
{
    if ( args.size() != 2)
    {
        addLogLine("\tWrong number of arguments. (changeAngle angle)");
    }
    else
    {
        float newAngle = std::stof(args[1]);
        addLogLine("Setting block angle to: " + std::to_string(newAngle));
        resources->setBlockAngle(newAngle);
    }

}
