#include <SFML/Graphics.hpp>

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

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        window.clear();
        window.draw(mapSprite); // Draw the map
        window.display();
    }
}
