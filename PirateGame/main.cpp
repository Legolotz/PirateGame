// Coder/Developer: Delfino Jimenez Vasquez, Logan Shroyer, Laurenz Becker
// Class          : COP 3003, Fall 2024
// Date           : 11/17/2024
/* Description    :
   A pirate game where you click on islands and if you guess the right one you win, but if you guess the wrong one you loose
*/

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <iostream>

// This program creates a game window where users interact with different types of islands.
// Depending on which island is clicked, the game state changes to "Victory," "Defeat," or stays the same for "Normal" islands.

/////////////////////////////////////////////
// Base class for Island
/////////////////////////////////////////////

class Island
{
protected:
    // Defines a rectangular area for the clickable region of the island
    sf::FloatRect bounds;

public:
    // Constructor: initializes the rectangular area of the island
    explicit Island(const sf::FloatRect& rect)
        : bounds(rect)
    {}

    // Virtual destructor: ensures proper cleanup for derived classes
    virtual ~Island() = default;

    // Function to check if a point (like the mouse position) is within the island's bounds
    // `sf::FloatRect::contains` checks whether the rectangle contains a given point
    [[nodiscard]] bool isClicked(const sf::Vector2f& point) const
    {
        return bounds.contains(point);
    }

    // Pure virtual function: forces derived classes to define their own behavior when clicked
    virtual void handleClick(sf::RenderWindow& window, sf::Sprite& sprite) const = 0;

    // Optional: Draws a red outline around the island's bounds for visualization (used for debugging)
    void drawBounds(sf::RenderWindow& window) const
    {
        sf::RectangleShape rectShape({bounds.width, bounds.height}); // Creates a rectangle with width and height
        rectShape.setPosition(bounds.left, bounds.top); // Positions it in the window
        rectShape.setFillColor(sf::Color(0, 0, 0, 0)); // Transparent fill
        rectShape.setOutlineColor(sf::Color::Red); // Red outline
        rectShape.setOutlineThickness(-1.0f); // Negative thickness draws the outline inward
        window.draw(rectShape); // Adds it to the window's rendering queue
    }
};

/////////////////////////////////////////////
// Derived classes for specific island types
/////////////////////////////////////////////

// A "NormalIsland" does nothing special when clicked
class NormalIsland : public Island
{
public:
    // Constructor: simply forwards the rectangle to the base class
    explicit NormalIsland(const sf::FloatRect& rect)
        : Island(rect)
    {}

    // Overrides handleClick to define behavior for "Normal" islands
    void handleClick(sf::RenderWindow& window, sf::Sprite& sprite) const override
    {
        std::cout << "This is a normal island.\n"; // Outputs a message to the console
    }
};

// A "VictoryIsland" changes the game state to "Victory" when clicked
class VictoryIsland : public Island
{
public:
    explicit VictoryIsland(const sf::FloatRect& rect)
        : Island(rect)
    {}

    void handleClick(sf::RenderWindow& window, sf::Sprite& sprite) const override
    {
        std::cout << "Victory!\n"; // Outputs a message to the console
        window.clear(); // Clears the window to prepare for new content
        window.draw(sprite); // Draws the victory sprite (e.g., celebratory image)
        window.display(); // Displays the new content on the screen
    }
};

// A "DefeatIsland" changes the game state to "Defeat" when clicked
class DefeatIsland : public Island
{
public:
    explicit DefeatIsland(const sf::FloatRect& rect)
        : Island(rect)
    {}

    void handleClick(sf::RenderWindow& window, sf::Sprite& sprite) const override
    {
        std::cout << "Defeat!\n"; // Outputs a message to the console
        window.clear(); // Clears the window
        window.draw(sprite); // Draws the defeat sprite (e.g., game-over image)
        window.display(); // Displays the updated content
    }
};

/////////////////////////////////////////////
// Main function: the entry point of the game
/////////////////////////////////////////////

int main()
{
    // Creates a game window (800x600 pixels) with the title "SFML Islands with OOP"
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Islands with OOP");
    window.setFramerateLimit(60); // Caps the frame rate at 60 frames per second

    // Loads textures for different states (background, victory, defeat screens)
    sf::Texture normalTexture, victoryTexture, defeatTexture;

    if (!normalTexture.loadFromFile("C:/Users/schwa/CLionProjects/PirateGame/Assets/Islands.png") ||
        !victoryTexture.loadFromFile("C:/Users/schwa/CLionProjects/PirateGame/Assets/Victory.png") ||
        !defeatTexture.loadFromFile("C:/Users/schwa/CLionProjects/PirateGame/Assets/Defeat.png"))
    {
        std::cerr << "Failed to load textures.\n"; // Outputs error if file loading fails
        return 1;
    }

    // Creates sprites (images) for the textures
    sf::Sprite normalSprite(normalTexture);
    sf::Sprite victorySprite(victoryTexture);
    sf::Sprite defeatSprite(defeatTexture);

    // Stores islands in a vector using polymorphism
    // std::unique_ptr is used for automatic memory management of dynamically allocated objects
    std::vector<std::unique_ptr<Island>> islands;

    // Adds islands to the vector with specific types and positions
    // `emplace_back` constructs objects directly in the vector, avoiding unnecessary copies
    islands.emplace_back(std::make_unique<VictoryIsland>(sf::FloatRect(30, 25, 200, 150))); // Top-left
    islands.emplace_back(std::make_unique<DefeatIsland>(sf::FloatRect(295, 25, 213, 150))); // Top-center
    islands.emplace_back(std::make_unique<NormalIsland>(sf::FloatRect(570, 25, 205, 150))); // Top-right
    islands.emplace_back(std::make_unique<NormalIsland>(sf::FloatRect(30, 225, 200, 154))); // Mid-Left
    islands.emplace_back(std::make_unique<VictoryIsland>(sf::FloatRect(295, 225, 213, 154))); // Mid-center
    islands.emplace_back(std::make_unique<DefeatIsland>(sf::FloatRect(570, 225, 205, 154))); // Mid-Right
    islands.emplace_back(std::make_unique<DefeatIsland>(sf::FloatRect(30, 430, 200, 150))); // Bottom-Left
    islands.emplace_back(std::make_unique<NormalIsland>(sf::FloatRect(295, 430, 213, 150))); // Bottom-Center
    islands.emplace_back(std::make_unique<VictoryIsland>(sf::FloatRect(570, 430, 205, 150))); // Bottom-Right

    // Tracks the current game state (Normal, Victory, Defeat)
    enum class GameState
    { Normal, Victory, Defeat };

    GameState currentState = GameState::Normal; // Initially in the "Normal" state

    // Tracks if the mouse button is being held (for click debouncing)
    bool mousePressed = false;

    // Main game loop: runs until the user closes the window
    while (window.isOpen())
    {
        sf::Event event{}; // Event object to capture user input

        while (window.pollEvent(event))
        { // Processes all events
            if (event.type == sf::Event::Closed) // Checks if the "close" event occurred
                window.close();
        }

        // Handles mouse clicks with debouncing
        // Checks if the left mouse button is pressed
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            // Prevents multiple clicks being registered while holding the button
            if (!mousePressed)
            {
                // Marks the mouse as pressed
                mousePressed = true;
                // Only processes clicks in the "Normal" state
                if (currentState == GameState::Normal)
                {
                    // Gets the mouse position in pixels
                    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

                    // Converts to world coordinates
                    //Will keep coordinates same if changing the window
                    sf::Vector2f worldPosition = window.mapPixelToCoords(mousePosition);

                    // Loops through all islands to check if one was clicked
                    for (const auto& island : islands)
                    {
                        // Calls the `isClicked` function to check bounds
                        if (island->isClicked(worldPosition))
                        {
                            // Updates the game state based on the type of island clicked
                            if (dynamic_cast<VictoryIsland*>(island.get()))
                            {
                                // `dynamic_cast` safely checks and converts the pointer to a specific type
                                currentState = GameState::Victory;
                            }

                            else if (dynamic_cast<DefeatIsland*>(island.get()))
                            {
                                currentState = GameState::Defeat;
                            }

                            // Executes the island-specific click behavior
                            island->handleClick(window, currentState == GameState::Victory ? victorySprite : defeatSprite);
                            // Exits the loop after handling one island
                            break;
                        }
                    }
                }
            }
        }

        else
        {
            mousePressed = false; // Resets the "pressed" state when the button is released
        }

        // Clears the window for a new frame
        window.clear();

        // Draws different content depending on the game state
        switch (currentState)
        {
            // In "Normal" state, draws the background and island bounds
            case GameState::Normal:
                window.draw(normalSprite);
                for (const auto& island : islands)
                {
                    // Displays red outlines for debugging
                    island->drawBounds(window);
                }
                break;

            // In "Victory" state, displays the victory screen
            case GameState::Victory:
                window.draw(victorySprite);
                break;

            // In "Defeat" state, displays the defeat screen
            case GameState::Defeat:
                window.draw(defeatSprite);
                break;
        }

        // Displays the current frame on the screen
        window.display();
    }

    // Return statement
    return 0;
}
