#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>

struct Node {
    sf::Vector2f position;
    bool isDestination;
};

int main()
{
    // Calculate scaled dimensions to fit 1920x1080 screen
    // Using 80% of screen height to leave some margin
    const float scale = (1080.0f * 0.8f) / 1479.0f;
    const unsigned int windowWidth = static_cast<unsigned int>(1479 * scale);
    const unsigned int windowHeight = static_cast<unsigned int>(1479 * scale);

    // Create window with scaled dimensions
    auto window = sf::RenderWindow(sf::VideoMode({windowWidth, windowHeight}), "Path Finder");
    window.setFramerateLimit(144);

    // Get the screen dimensions
    auto desktop = sf::VideoMode::getDesktopMode();
    
    // Calculate the center position
    int centerX = (desktop.size.x - window.getSize().x) / 2;
    int centerY = (desktop.size.y - window.getSize().y) / 2;
    
    // Set the window position to center
    window.setPosition(sf::Vector2i(centerX, centerY));

    // Load the map texture
    sf::Texture mapTexture;
    if (!mapTexture.loadFromFile("map.png"))
    {
        return -1; // Exit if image loading fails
    }

    // Create a sprite to display the texture
    sf::Sprite mapSprite(mapTexture);
    
    // Scale the sprite to fit the window
    mapSprite.setScale(sf::Vector2f(scale, scale));

    // Create button
    sf::RectangleShape button(sf::Vector2f(150, 40));
    button.setPosition(sf::Vector2f(10, 10));
    button.setFillColor(sf::Color(100, 100, 100));
    button.setOutlineThickness(2);
    button.setOutlineColor(sf::Color::White);

    // Create button text
    sf::Font font;
    if (!font.openFromFile("OpenSans-Regular.ttf")) {
        return -1; // Exit if font loading fails
    }
    sf::Text buttonText(font, "Add Node", 20);
    buttonText.setFillColor(sf::Color::White);
    buttonText.setPosition(sf::Vector2f(20, 15));

    // Node type selection buttons (hidden by default)
    sf::RectangleShape destButton(sf::Vector2f(130, 35));
    destButton.setPosition(sf::Vector2f(20, 60));
    destButton.setFillColor(sf::Color(200, 80, 80));
    destButton.setOutlineThickness(2);
    destButton.setOutlineColor(sf::Color::White);
    sf::Text destText(font, "Destination", 18);
    destText.setFillColor(sf::Color::White);
    destText.setPosition(sf::Vector2f(30, 65));

    sf::RectangleShape roadButton(sf::Vector2f(130, 35));
    roadButton.setPosition(sf::Vector2f(20, 105));
    roadButton.setFillColor(sf::Color(80, 80, 200));
    roadButton.setOutlineThickness(2);
    roadButton.setOutlineColor(sf::Color::White);
    sf::Text roadText(font, "Road", 18);
    roadText.setFillColor(sf::Color::White);
    roadText.setPosition(sf::Vector2f(30, 110));

    // Remove Node button
    sf::RectangleShape removeButton(sf::Vector2f(150, 40));
    removeButton.setPosition(sf::Vector2f(10, 155));
    removeButton.setFillColor(sf::Color(120, 40, 40));
    removeButton.setOutlineThickness(2);
    removeButton.setOutlineColor(sf::Color::White);
    sf::Text removeText(font, "Remove Node", 18);
    removeText.setFillColor(sf::Color::White);
    removeText.setPosition(sf::Vector2f(20, 165));

    // Node vectors
    std::vector<Node> destinationNodes;
    std::vector<Node> roadNodes;

    // Node selection state
    bool isAddingNode = false;
    bool isDestinationNode = false;
    bool showTypeButtons = false;
    bool isRemoveMode = false;

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (event->is<sf::Event::MouseButtonPressed>())
            {
                auto mousePos = sf::Mouse::getPosition(window);

                // Check if button was clicked
                if (button.getGlobalBounds().contains(sf::Vector2f(mousePos)))
                {
                    showTypeButtons = true;
                }
                // Check if destination button was clicked
                else if (showTypeButtons && destButton.getGlobalBounds().contains(sf::Vector2f(mousePos)))
                {
                    isAddingNode = true;
                    isDestinationNode = true;
                    showTypeButtons = false;
                }
                // Check if road button was clicked
                else if (showTypeButtons && roadButton.getGlobalBounds().contains(sf::Vector2f(mousePos)))
                {
                    isAddingNode = true;
                    isDestinationNode = false;
                    showTypeButtons = false;
                }
                // Check if remove button was clicked
                else if (removeButton.getGlobalBounds().contains(sf::Vector2f(mousePos)))
                {
                    isRemoveMode = !isRemoveMode; // Toggle remove mode
                }
                // If in remove mode, check if a node was clicked
                else if (isRemoveMode)
                {
                    // Check destination nodes
                    for (auto it = destinationNodes.begin(); it != destinationNodes.end(); ++it) {
                        sf::CircleShape nodeShape(5);
                        nodeShape.setPosition(it->position);
                        if (nodeShape.getGlobalBounds().contains(window.mapPixelToCoords(mousePos))) {
                            destinationNodes.erase(it);
                            isRemoveMode = false;
                            break;
                        }
                    }
                    // Check road nodes if not already removed
                    if (!isRemoveMode) continue;
                    for (auto it = roadNodes.begin(); it != roadNodes.end(); ++it) {
                        sf::CircleShape nodeShape(5);
                        nodeShape.setPosition(it->position);
                        if (nodeShape.getGlobalBounds().contains(window.mapPixelToCoords(mousePos))) {
                            roadNodes.erase(it);
                            isRemoveMode = false;
                            break;
                        }
                    }
                }
                // If we're in node adding mode and clicked on the map
                else if (isAddingNode && mousePos.x > 0 && mousePos.x < windowWidth &&
                         mousePos.y > 0 && mousePos.y < windowHeight)
                {
                    // Convert mouse position to world coordinates
                    sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);
                    
                    // Create and add the node
                    Node newNode;
                    newNode.position = worldPos;
                    newNode.isDestination = isDestinationNode;

                    if (isDestinationNode) {
                        destinationNodes.push_back(newNode);
                    } else {
                        roadNodes.push_back(newNode);
                    }

                    isAddingNode = false; // Reset the adding state
                }
            }
        }

        window.clear();
        window.draw(mapSprite); // Draw the map
        
        // Draw nodes
        for (const auto& node : destinationNodes) {
            sf::CircleShape nodeShape(5);
            nodeShape.setPosition(node.position);
            nodeShape.setFillColor(sf::Color::Red);
            window.draw(nodeShape);
        }
        
        for (const auto& node : roadNodes) {
            sf::CircleShape nodeShape(5);
            nodeShape.setPosition(node.position);
            nodeShape.setFillColor(sf::Color::Blue);
            window.draw(nodeShape);
        }

        // Draw button and text
        window.draw(button);
        window.draw(buttonText);
        if (showTypeButtons) {
            window.draw(destButton);
            window.draw(destText);
            window.draw(roadButton);
            window.draw(roadText);
        }
        window.draw(removeButton);
        window.draw(removeText);
        
        // Optionally, visually indicate remove mode
        if (isRemoveMode) {
            // Draw a red outline around the window or show a message
        }
        
        window.display();
    }
}
