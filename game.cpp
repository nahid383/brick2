#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>

using namespace std;

// Game constants
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int PADDLE_WIDTH = 150;
const int PADDLE_HEIGHT = 15;
const int BALL_RADIUS = 10;
const int BRICK_WIDTH = 75;
const int BRICK_HEIGHT = 25;
const int BRICK_ROWS = 6;
const int BRICK_COLS = 10;

class BrickBreakerGame {
public:
    BrickBreakerGame();
    void run();

private:
    void processEvents();
    void update();
    void render();
    void resetBall();
    void initBricks();
    void handleCollisions();
    void updateScore();
    void updateLives();
    void gameOver();
    void gameWon();
    
    sf::RenderWindow window;
    sf::RectangleShape paddle;
    sf::CircleShape ball;
    std::vector<sf::RectangleShape> bricks;
    sf::Font font;
    sf::Text scoreText;
    sf::Text livesText;
    sf::Text gameOverText;
    sf::Text gameWonText;
    
    int score = 0;
    int lives = 3;
    float ballSpeedX = 5.0f;
    float ballSpeedY = -5.0f;
    float paddleSpeed = 8.0f;
    bool gameRunning = true;
};

BrickBreakerGame::BrickBreakerGame() 
    : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Brick Breaker Game") {
    
    paddle.setSize(sf::Vector2f(PADDLE_WIDTH, PADDLE_HEIGHT));
    paddle.setPosition(WINDOW_WIDTH / 2 - PADDLE_WIDTH / 2, WINDOW_HEIGHT - 30);
    paddle.setFillColor(sf::Color::White);
    
    ball.setRadius(BALL_RADIUS);
    ball.setFillColor(sf::Color::White);
    ball.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
    
    if (!font.loadFromFile("arial.ttf")) {
        std::cout << "Error loading font!" << std::endl;
    }

    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10, 10);

    livesText.setFont(font);
    livesText.setCharacterSize(24);
    livesText.setFillColor(sf::Color::White);
    livesText.setPosition(WINDOW_WIDTH - 150, 10);

    gameOverText.setFont(font);
    gameOverText.setCharacterSize(32);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setString("Game Over!");
    gameOverText.setPosition(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2);

    gameWonText.setFont(font);
    gameWonText.setCharacterSize(32);
    gameWonText.setFillColor(sf::Color::Green);
    gameWonText.setString("You Won!");
    gameWonText.setPosition(WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2);
}

void BrickBreakerGame::initBricks() {
    for (int r = 0; r < BRICK_ROWS; r++) {
        for (int c = 0; c < BRICK_COLS; c++) {
            sf::RectangleShape brick(sf::Vector2f(BRICK_WIDTH, BRICK_HEIGHT));
            brick.setPosition(c * (BRICK_WIDTH + 5) + 35, r * (BRICK_HEIGHT + 5) + 60);
            brick.setFillColor(sf::Color::Red);
            bricks.push_back(brick);
        }
    }
}

void BrickBreakerGame::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        if (paddle.getPosition().x > 0) {
            paddle.move(-paddleSpeed, 0);
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        if (paddle.getPosition().x + PADDLE_WIDTH < WINDOW_WIDTH) {
            paddle.move(paddleSpeed, 0);
        }
    }
}

void BrickBreakerGame::update() {
    if (!gameRunning) return;

    // Move ball
    ball.move(ballSpeedX, ballSpeedY);

    // Ball collision with walls
    if (ball.getPosition().x <= 0 || ball.getPosition().x + BALL_RADIUS * 2 >= WINDOW_WIDTH) {
        ballSpeedX = -ballSpeedX;
    }

    if (ball.getPosition().y <= 0) {
        ballSpeedY = -ballSpeedY;
    }

    // Ball falls below paddle
    if (ball.getPosition().y + BALL_RADIUS * 2 >= WINDOW_HEIGHT) {
        lives--;
        updateLives();
        if (lives == 0) {
            gameOver();
        } else {
            resetBall();
        }
    }

    // Ball collision with paddle
    if (ball.getGlobalBounds().intersects(paddle.getGlobalBounds())) {
        ballSpeedY = -ballSpeedY;
    }

    handleCollisions();
}

void BrickBreakerGame::render() {
    window.clear();

    // Draw bricks
    for (auto& brick : bricks) {
        window.draw(brick);
    }

    // Draw paddle and ball
    window.draw(paddle);
    window.draw(ball);

    // Draw score and lives
    window.draw(scoreText);
    window.draw(livesText);

    // Display game over or game won message
    if (!gameRunning) {
        if (lives == 0) {
            window.draw(gameOverText);
        } else {
            window.draw(gameWonText);
        }
    }

    window.display();
}

void BrickBreakerGame::handleCollisions() {
    for (auto& brick : bricks) {
        if (brick.getGlobalBounds().intersects(ball.getGlobalBounds())) {
            ballSpeedY = -ballSpeedY;
            brick.setFillColor(sf::Color::Transparent);  // Remove brick
            score += 10;
            updateScore();

            // Check if all bricks are destroyed
            bool allBricksDestroyed = true;
            for (auto& b : bricks) {
                if (b.getFillColor() != sf::Color::Transparent) {
                    allBricksDestroyed = false;
                    break;
                }
            }

            if (allBricksDestroyed) {
                gameWon();
            }
        }
    }
}

void BrickBreakerGame::resetBall() {
    ball.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
    ballSpeedX = 5.0f;
    ballSpeedY = -5.0f;
}

void BrickBreakerGame::updateScore() {
    scoreText.setString("Score: " + std::to_string(score));
}

void BrickBreakerGame::updateLives() {
    livesText.setString("Lives: " + std::to_string(lives));
}

void BrickBreakerGame::gameOver() {
    gameRunning = false;
}

void BrickBreakerGame::gameWon() {
    gameRunning = false;
}

void BrickBreakerGame::run() {
    initBricks();
    while (window.isOpen()) {
        processEvents();
        update();
        render();
    }
}

int main() {
    BrickBreakerGame game;
    game.run();
    return 0;
}
