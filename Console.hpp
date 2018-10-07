#pragma once

#include <SFML/Graphics.hpp>

class Console
{
public:
    void init(class Resources *res);
    void draw(sf::RenderWindow& window);
    void update(float deltaTime);
    void execute(std::string command);
    void addInput(int character);
    void updateLogBufferPosition(); // Used to show newest log inputs
 
    void show();
    void hide();
    void addLogLine(std::string logLine);

    bool isActive() { return active; }
    bool isActiveOrHiding() { return (active || hideAnimation); }

private:
    sf::RectangleShape background;
    sf::Text commandBufferText;
    sf::Text logBufferText;
    sf::RectangleShape cursor;
    std::string commandBuffer;

    class Resources *resources = nullptr;
    bool active = false;
    float animateSpeed = 1000.0f;
    float outlineThickness = -1.0f;

    float consoleSize = 0.4f;   // Percentage from screen

    bool hideAnimation = false;
    
    int maxLogLines = 500;
    std::vector <std::string> logBuffer;

    float logAreaHeight = 0.0f; // Area which can be used to print the log lines
    int logLineCount = 0;
    int logBufferPosition = 0; // Position of logbuffer

    bool keepLogBufferPositionUpdated = true;
    float blinkyCurrentTime = 0.0f;
    float blinkyTime = 0.3f;
    bool blink = true;
};
