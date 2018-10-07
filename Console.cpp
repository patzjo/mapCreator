#include "Console.hpp"
#include "Resources.hpp"




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
    commandBufferText.setString("W");

    cursor.setSize({2.0f, 17.0f});
    cursor.setFillColor(sf::Color::White);
    
    logAreaHeight = background.getSize().y - 30.0f;
    logLineCount = (int)(logAreaHeight/logBufferText.getCharacterSize()+0.5f);
}

void Console::updateLogBufferPosition()
{
    int position = logBuffer.size() - logLineCount;
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
            execute(commandBuffer);
            commandBuffer.clear();
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


void Console::execute(std::string command)
{
    addLogLine("Executing: '" + commandBuffer + "'");
    
    

    

}