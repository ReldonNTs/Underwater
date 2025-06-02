#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <vector>
#include <random>
#include <deque>
#include "func.h"

void WeaponControl(sf::Sprite& cann,sf::RectangleShape& weapon, float& deltaTime, float& wrotationspeed, float& currentAngle, float& angularVelocity, float& wrate, sf::Vector2f& mouseWorldPos) { ///// Контроль Пушки

    sf::Vector2f direction = mouseWorldPos - weapon.getPosition(); // Вычисляем направление от пушки к мыши

    float length = sqrt(direction.x * direction.x + direction.y * direction.y); // Нормализуем вектор направления в единичный
    if (length > 0)
        direction /= length;
    float targetangle = static_cast<float>(atan2(direction.y, direction.x) * 180 / acos(-1.0)); // Вычисляем угол поворота в градусах
    float angularDifference = targetangle - currentAngle; // Вычисляем разницу между текущим и целевым углом

    // Ограничиваем угол в пределах 0..359 градусов
    if (angularDifference > 180.f)
        angularDifference -= 360.f;
    else if (angularDifference < -180.f)
        angularDifference += 360.f;

    // Интерполяция и торможение
    if (abs(angularDifference) > 0.01f)
    {
        // Увеличиваем угловую скорость пропорционально разнице углов
        angularVelocity += angularDifference * wrate * deltaTime;
        // Ограничиваем максимальную скорость
        if (angularVelocity > wrotationspeed)
            angularVelocity = wrotationspeed;
        else if (angularVelocity < -wrotationspeed)
            angularVelocity = -wrotationspeed;
        // Поворачиваем пушку
        currentAngle += angularVelocity * deltaTime;
    }
    if (abs(angularDifference) < 0.7f)
    {
        // Если разница мала, начинаем тормозить
        angularVelocity *= (0.5f); // Замедление скорости
        currentAngle += angularVelocity * deltaTime;
        // Останавливаем вращение, если скорость стала близка к нулю
        if (abs(angularVelocity) < 0.01f)
            angularVelocity = 0.f;
    }

    // Ограничиваем угол в пределах 0..359 градусов
    if (currentAngle >= 360.f)
        currentAngle -= 360.f;
    else if (currentAngle < 0.f)
        currentAngle += 360.f;

    sf::Angle angle = sf::degrees(currentAngle);

    weapon.setRotation(angle);
    cann.setRotation(angle);

}

void CameraControl(sf::Vector2u& resolution, sf::View& camera, float& deltaTime, short& fps, sf::RenderWindow& window, sf::Vector2f& mouseWorldPos) {  ///// Контроль камеры ПОФИКСИТЬ БАГ С ДЕЛЬТА ТАЙМ

    float CAMERA_FOLLOW_SPEED = 5.f; // Скорость следования камеры за объектом

    float halfWidth = static_cast<float>(resolution.x) / 2.f;
    float halfHeight = static_cast<float>(resolution.y) / 2.f;

    sf::Vector2f halfResfloat(halfWidth, halfHeight);
    sf::Vector2i halfResint(static_cast<int>(halfResfloat.x), static_cast<int>(halfResfloat.y));

    
    // Движение камеры
    sf::Vector2f cameraPosition = camera.getCenter();
    sf::Vector2f worldCenter = window.mapPixelToCoords(halfResint);
    sf::Vector2f cameraOffset = mouseWorldPos - worldCenter;
    if ((sqrt(pow(mouseWorldPos.x - worldCenter.y, 2) + pow(mouseWorldPos.y - worldCenter.y, 2)) > 0.1f))
    {
        cameraOffset *= CAMERA_FOLLOW_SPEED * 0.006061f * 1.5f * 2.5f * fps * 0.006f;
        cameraOffset.x += halfResint.x;
        cameraOffset.y += halfResint.y;
        camera.setCenter(cameraOffset);
        //std::cout << cameraOffset.x << " " << cameraOffset.y<<std::endl;   /////////////////---------------консооооль
    }

}

void CameraShake(sf::View& camera, short& fps) { // Тряска камеры

    sf::Clock shakeTimer; // Таймер для тряски камеры
    float CAMERA_SHAKE_DURATION = 0.5f; // Длительность тряски камеры в секундах
    float CAMERA_SHAKE_INTENSITY = (fps * 0.07f); // Интенсивность тряски камеры

    if (shakeTimer.getElapsedTime().asSeconds() < CAMERA_SHAKE_DURATION)
    {
        sf::Vector2f shakeOffset(std::rand() % (int)CAMERA_SHAKE_INTENSITY - CAMERA_SHAKE_INTENSITY / 2,
            std::rand() % (int)CAMERA_SHAKE_INTENSITY - CAMERA_SHAKE_INTENSITY / 2);
        camera.move(shakeOffset);
    }
    
}

//////////////////////////////////////

// Конструктор частиц
ParticleSystem::ParticleSystem() {
    vertices.setPrimitiveType(sf::PrimitiveType::Points);
}

void ParticleSystem::addParticle(sf::Vector2f position, sf::Vector2f velocity, sf::Color color) { // Добавление частиц
    Particle particle;
    particle.position = position;
    particle.velocity = velocity;
    particle.color = color;

    particles.push_back(particle);

    sf::Vertex vertex;
    vertex.position = position;
    vertex.color = color;
    vertices.append(vertex);
}

void ParticleSystem::update(sf::Time elapsed, sf::RenderWindow& window) { //  Обновление частиц

    std::vector<Particle> updatedParticles;
    sf::VertexArray updatedVertices(sf::PrimitiveType::Points);

    for (std::size_t i = 0; i < particles.size(); ++i) {
        Particle& particle = particles[i];
        particle.position += particle.velocity * elapsed.asSeconds();

        if (particle.position.x >= -200.f && particle.position.x <= window.getSize().x + 200.f &&
            particle.position.y >= -200.f && particle.position.y <= window.getSize().y + 200.f) {
            updatedParticles.push_back(particle);

            sf::Vertex vertex;
            vertex.position = particle.position;
            vertex.color = particle.color;
            updatedVertices.append(vertex);
        }
    }

    particles = std::move(updatedParticles);
    vertices = std::move(updatedVertices);
}

void ParticleSystem::draw(sf::RenderTarget& target, sf::RenderStates states) const { // Отрисовка частиц
    states.transform *= getTransform();
    target.draw(vertices, states);
}

//////////////////////////////////////

void UpdateBubbles(std::vector<sf::Sprite>& bubbles, sf::RenderWindow& window, sf::Vector2f& spawnPos, short& fps, sf::Texture& bubbleTexture) {

    // Создаем новый пузырек, если его еще нет
    if (bubbles.empty()) {
        sf::Sprite newBubble(bubbleTexture);
        // Установим начальную позицию нового пузырька
        newBubble.setPosition(spawnPos);

        // Добавляем новый пузырек в вектор
        bubbles.push_back(newBubble);
    }

    for (size_t i = 0; i < bubbles.size(); ++i) {
        sf::Sprite& bubble = bubbles[i];

        // Проверяем, жив ли пузырек
        if (bubble.getColor().a > 0) { // Если прозрачность больше нуля, значит пузырек активен
            // Обновляем состояние пузырька
            int alpha = 255;

            alpha -= 10; // Постепенно уменьшаем прозрачность после достижения максимума


            // Устанавливаем новую прозрачность
            sf::Color color = bubble.getColor();
            color.a = static_cast<uint8_t>(alpha);

            bubble.setColor(color);

            // Перемещаем пузырек случайным образом
            bubble.move({ static_cast<float>(rand() % 3 - 1), static_cast<float>(rand() % 3 - 1) - 1 });

            // Рисуем пузырек
            window.draw(bubble);
        }
        else {
            // Удаляем мертвый пузырек
            bubbles.erase(bubbles.begin() + i);
            --i; // Смещаемся назад, чтобы учесть удаление элемента
        }
    }
}

//////////////////////////////////////

// Конструктор рыб
FishSystem::FishSystem() {
    fvertices.setPrimitiveType(sf::PrimitiveType::Points);
}

void FishSystem::addFish(sf::Vector2f position, sf::Vector2f velocity, sf::Color color) { // Добавление частиц
    Fish fish;
    fish.position = position;
    fish.velocity = velocity;
    fish.color = color;

    float ftargetangle = static_cast<float>(atan2(velocity.y, velocity.x) * 180 / acos(-1.0)); // Вычисляем угол поворота в градусах
    if (ftargetangle < 0) {
        ftargetangle += 360.0f;
    }
    sf::Angle angle = sf::degrees(ftargetangle);
    fish.rotation = angle;
    Fishes.push_back(fish);

    sf::Vertex vertex;
    vertex.position = position;
    vertex.color = color;
    fvertices.append(vertex);

}

void FishSystem::update(sf::Time elapsed, sf::RenderWindow& window, sf::Sprite& fish1, sf::Vector2f& bullet1Pos, sf::Vector2f& bullet2Pos, sf::Vector2f& rwPos, bool& shot1, bool& shot2, bool& b1active, bool& b2active, int& score, bool& kill) { //  Обновление рыб,

    std::vector<Fish> updatedFishes;
    sf::VertexArray updatedFVertices(sf::PrimitiveType::Points);

    for (std::size_t i = 0; i < Fishes.size(); ++i) { // Тут писать движение
        Fish& fish = Fishes[i];

        fish.position += fish.velocity * elapsed.asSeconds();

        bool hit = 0;

        if (fish.position.x >= bullet1Pos.x - 20.f && fish.position.x <= bullet1Pos.x + 20.f && fish.position.y >= bullet1Pos.y - 20.f && fish.position.y <= bullet1Pos.y + 20.f && b1active)
        {
            hit = 1;
            shot1 = 1;
        }
            
        if (fish.position.x >= bullet2Pos.x - 20.f && fish.position.x <= bullet2Pos.x + 20.f && fish.position.y >= bullet2Pos.y - 20.f && fish.position.y <= bullet2Pos.y + 20.f && b2active)
        {
            hit = 1;
            shot2 = 1;
        }

        if (fish.position.x >= rwPos.x && fish.position.x <= rwPos.x + 1425.f && fish.position.y >= rwPos.y && fish.position.y <= rwPos.y + 260.f) //1425 267
        {
            hit = 1;
            kill = 1;
        }
        
        if (hit)
            score = score + 15;
            
        if (fish.position.x >= -310.f && fish.position.x <= window.getSize().x + 310.f &&
            fish.position.y >= -310.f && fish.position.y <= window.getSize().y + 310.f && !hit) {

            updatedFishes.push_back(fish);

            sf::Vertex vertex;
            vertex.position = fish.position;
            vertex.color = fish.color;
            updatedFVertices.append(vertex);

            fish1.setPosition(fish.position);
            fish1.setRotation(fish.rotation);
            window.draw(fish1);
        }

    }

    Fishes = std::move(updatedFishes);
    fvertices = std::move(updatedFVertices);
    
}

void FishSystem::draw(sf::RenderTarget& target, sf::RenderStates states) const { // Отрисовка частиц
    states.transform *= getTransform();
    
}

//////////////////////////////////////

