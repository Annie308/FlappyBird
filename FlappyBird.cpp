#include <iostream>
#include <chrono>
#include <thread>
#include <conio.h>
#include <atomic>
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>


using namespace std;
using namespace std::chrono;


const int TARGET_FPS = 60; // Target frames per second
const milliseconds FRAME_DURATION(1000/ TARGET_FPS); // Duration of each frame in milliseconds


atomic<bool> running(true); // Atomic variable to control the game loop

const int SCREEN_WIDTH = 1200; // Width of the window
const int SCREEN_HEIGHT = 1000; // Height of the window
int score =0;


float x, y; // Initial position

class Bird {
  public:
    int height, width;
    float gravity =  300;
    float velocity = 0.0f; // Initial velocity

    sf::RectangleShape shape;

    Bird() {
      x = 100;
      y = 200;
      height = 70;
      width = 70;

      shape.setSize(sf::Vector2f(width, height));
      shape.setFillColor(sf::Color::Yellow);
      shape.setPosition(x, y);
    }

    void flap(){
        velocity = -150.0f;
    }

    void fly(float time){
      velocity += gravity * time; // Update velocity based on gravity
      y += velocity*time;         // Update posiition based on velocity
      shape.setPosition(x, y);
    }

    void draw(sf::RenderWindow& window){
      window.draw(shape);
    }
};
Bird myBird; 

class PipePair {
  public:
    int width, height;

    sf::RectangleShape bottomPipe;
    sf::RectangleShape topPipe;

  PipePair(){
    width = 70;
    height = 100;

    bottomPipe.setSize(sf::Vector2f(width,height));
    bottomPipe.setFillColor(sf::Color::Green);
    bottomPipe.setPosition(x,y);

    topPipe.setSize(sf::Vector2f(width,height));
    topPipe.setFillColor(sf::Color::Green);
    topPipe.setPosition(x,y);
   
  }


  void move(float dx){
    bottomPipe.move(-dx, 0);
    topPipe.move(-dx, 0);
  }

  void setHeight(float h, float h2){
    bottomPipe.setSize(sf::Vector2f(width, h2));
    topPipe.setSize(sf::Vector2f(width,h));
    bottomPipe.setPosition(SCREEN_WIDTH, SCREEN_HEIGHT-h2);
    topPipe.setPosition(SCREEN_WIDTH, 0);
  }

  void draw(sf::RenderWindow& window){
    window.draw(bottomPipe);
    window.draw(topPipe);

  }

};

sf::Font font;

class Score{
  public:
  sf::Text text;
  string str_score = "0";

  Score(sf::Font& font){
    text.setFont(font);
    text.setString("Score: " + str_score);
    text.setPosition(50,50);
    text.setCharacterSize(70);
    text.setFillColor(sf::Color::White);
  }

  void addScore(){
    score ++;
    str_score = to_string(score);
    text.setString("Score: " + str_score);
  }

  void draw(sf::RenderWindow& window){
    window.draw(text);
  }
};

Score myScore(font);
vector<PipePair> pipes; // Vector to hold the pipes

void makePipe(){
    PipePair p;
    p.setHeight(rand() % 230 + 200, rand() % 230 + 200);
    pipes.push_back(p);

    for (int i = 0; i< pipes.size(); ) {
      if (pipes[i].topPipe.getPosition().x < 0){
        pipes.erase(pipes.begin()+i);
        myScore.addScore();
    } else{
      i++;
    }
    }
    
  }

void collision(){
  for (int i = 0; i < pipes.size(); ++i){
    float bottomPipeX = pipes[i].bottomPipe.getPosition().x;
    float bottomPipeY = pipes[i].bottomPipe.getPosition().y;
    float bottomPipeH = pipes[i].bottomPipe.getSize().y;
    float bottomPipeW = pipes[i].bottomPipe.getSize().x;

    float topPipeX = pipes[i].topPipe.getPosition().x;
    float topPipeY = pipes[i].topPipe.getPosition().y;
    float topPipeH = pipes[i].topPipe.getSize().y;
    float topPipeW = pipes[i].topPipe.getSize().x;

    float birdX = myBird.shape.getPosition().x;
    float birdY = myBird.shape.getPosition().y;
    float birdH = myBird.shape.getSize().y;
    float birdW = myBird.shape.getSize().x;


    bool bottomPipeXOverlap = birdX < bottomPipeX + bottomPipeW && birdX + birdW > bottomPipeX;
    bool bottomPipeYOverlap = birdY + birdH > bottomPipeY && birdY < bottomPipeY+bottomPipeH;

    bool topPipeXOverlap = birdX < topPipeX + topPipeW && birdX + birdW > topPipeX;
    bool topPipeYOverlap = birdY + birdH > topPipeY && birdY < topPipeY+topPipeH;

    if (bottomPipeXOverlap && bottomPipeYOverlap){
      cout <<"Game over!" << endl;
      running = false;
    }

    if (topPipeXOverlap && topPipeYOverlap){
      cout <<"Game over!" << endl;
      running = false;
    }
  }

  if (myBird.shape.getPosition().y + myBird.height > 1000){
    cout <<"Game over!" << endl;
    running = false;
  }
}

void keyListener() {
    while (running) {
        if (_kbhit()){
            char key = _getch(); // Get the key pressed
          if (key == 27){
              cout << "Exiting game..." << endl;
              running = false;
    }
          if (key == ' '){
              myBird.flap(); // Call the flap function of the Bird class
    }
  }
}
}

int main() {
  float pipeSpeed = 10;
  float pipeDelay = 2.0f;

  if (!font.loadFromFile("Roboto-VariableFont_wdth,wght.ttf")){
    cout << "Failed to load font" <<endl;
    return -1;
  }
  thread listener(keyListener);     //Start the key listener thread called "listener"
  
  // Grphics setup
      sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "window"); // Create a window with SFML
     
sf::Clock pipeClock; // Move pipeClock outside the loop to persist its state

while (running && window.isOpen()){

  auto frameStart = std::chrono::high_resolution_clock::now();

  // Game logic

  myBird.fly(0.016);    // time of each frame is approximately 16 milliseconds (1/60 seconds)
  collision();

  if (pipeClock.getElapsedTime().asSeconds() > pipeDelay) {
    makePipe();
    pipeSpeed = pipeSpeed + 0.2;
    pipeDelay = 8/pipeSpeed;
    pipeClock.restart();
  }

  window.clear(sf::Color::Black); // Clear the window with black color

  myBird.draw(window);

  for (int i = 0; i < pipes.size(); ++i) {
    pipes[i].move(pipeSpeed);
    pipes[i].draw(window);
  }

  myScore.draw(window);

  window.display(); // Display the contents of the window

  auto frameEnd = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart);

  // Frame rate control
  if (elapsed < FRAME_DURATION) {
    std::this_thread::sleep_for(FRAME_DURATION - elapsed);
  }
}
cout<< "Score: " << score << endl;
listener.join(); // Wait for the key listener thread to finish before continuing

}