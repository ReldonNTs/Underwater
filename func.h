#pragma once
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <vector>
#include <random>
#include <deque>


void WeaponControl(sf::Sprite& cann, sf::RectangleShape& weapon, float& deltaTime, float& wrotationspeed, float& currentAngle, float& angularVelocity, float& wrate, sf::Vector2f& mouseWorldPos); // Контроль Пушки

void CameraControl(sf::Vector2u& resolution, sf::View& camera, float& deltaTime, short& fps, sf::RenderWindow& window, sf::Vector2f& mouseWorldPos); // Контроль камеры

void CameraShake(sf::View& camera, short& fps); // Тряска камеры

void UpdateBubbles(std::vector<sf::Sprite>& bubbles, sf::RenderWindow& window, sf::Vector2f& spawnPos, short& fps, sf::Texture& bubbleTexture);

//////////////////////////////////////

struct Particle {

    sf::Vector2f velocity;

    sf::Vector2f position;

    sf::Color color;
};

class ParticleSystem : public sf::Drawable, public sf::Transformable {

public:

    ParticleSystem();

    void addParticle(sf::Vector2f position, sf::Vector2f velocity, sf::Color color);

    void update(sf::Time elapsed, sf::RenderWindow& window);

private:

    std::vector<Particle> particles;

    sf::VertexArray vertices;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

//////////////////////////////////////

struct Fish {
    
    sf::Vector2f velocity;

    sf::Vector2f position;

    sf::Color color;

    sf::Angle rotation;

};

class FishSystem : public sf::Drawable, public sf::Transformable {

public:

    FishSystem();

    void addFish(sf::Vector2f position, sf::Vector2f velocity, sf::Color color);

    void update(sf::Time elapsed, sf::RenderWindow& window, sf::Sprite& fish1, sf::Vector2f& bullet1Pos, sf::Vector2f& bullet2Pos, sf::Vector2f& rwPos, bool& shot1, bool& shot2, bool& b1active, bool& b2active, int& score, bool& kill);

private:

    std::vector<Fish> Fishes;

    sf::VertexArray fvertices;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

};