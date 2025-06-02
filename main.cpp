#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <vector>
#include <random>
#include "func.h"

////////////////////////////////////// Рандом

std::random_device rd;
std::mt19937 gen(rd());

float randomFloat(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}

int randomInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}

//////////////////////////////////////

int main()
{
    //////////////////////////////////////

    std::cout << "Loading..." << '\n';

    sf::RenderWindow setupwindow(sf::VideoMode({ 1500,900 }), "RW-02 Setup", sf::Style::None); // окно
    setupwindow.setVerticalSyncEnabled(true);

    sf::Image sbgimage("textures/setuptexture.png"); // текстура фона
    sf::Texture sbgtexture(sbgimage);
    sf::Sprite sbgsprite(sbgtexture);

    std::cout << "Loading complete. Press ESC to play." << '\n';

    while (setupwindow.isOpen()) {


        while (const std::optional event = setupwindow.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                setupwindow.close();

            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) { //закрытие на esc
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
                    setupwindow.close();
                    std::cout << "the escape key was pressed" << std::endl;
                }

            }

        }

        //////////////////////////////////////

        setupwindow.clear();
        setupwindow.draw(sbgsprite);
        setupwindow.display();

    }

    //////////////////////////////////////  Ввод настроек

    std::ifstream filein("settings/settings.txt");
    std::string RESi, FPSi, HARDi, HEALTHi;
    getline(filein, RESi);
    int RESn = stoi(RESi);
    getline(filein, FPSi);
    int FPSn = stoi(FPSi);
    getline(filein, HARDi);
    int HARDn = stoi(HARDi);
    getline(filein, HEALTHi);
    int HEALTHn = stoi(HEALTHi);
    filein.close();

    //////////////////////////////////////  Начальные значения

    short fps = 165;
    float deltaTime;
    if (fps >= 60) {
        deltaTime = (1.f / fps); // ВРЕМЯ ОБРАБОТЧИКА АНИМАЦИЙ
    }
    else if (fps < 60) {
        deltaTime = (1.f / fps) - (1.f / fps) / 3.f;
    }
    short resolutionChoise = RESn;
    unsigned int resolutions[2][2]{ { 1920,1080 }, { 2560,1440 } };
    unsigned int width = resolutions[resolutionChoise][0];
    unsigned int height = resolutions[resolutionChoise][1];
    sf::Vector2u resolution({ width,height });

    sf::RenderWindow window(sf::VideoMode({ width,height }), "RW-02", sf::Style::None); // окно
    window.setFramerateLimit(fps);
    window.setVerticalSyncEnabled(false);
    sf::View camera(window.getDefaultView());


    sf::Image bgimage("textures/bg4k.png"); // текстура фона
    sf::Texture bgtexture(bgimage);
    sf::Sprite bgsprite(bgtexture);
    bgsprite.setPosition({ -960, -540 });

    sf::Image pauseimage("textures/pause.png"); // текстура паузы
    sf::Texture pausetexture(pauseimage);
    sf::Sprite pausesprite(pausetexture);

    sf::Image deathimage("textures/death.png"); // текстура смерти
    sf::Texture deathtexture(deathimage);
    sf::Sprite deathsprite(deathtexture);

    sf::Image panelimage("textures/panel.png"); // текстура панели
    sf::Texture paneltexture(panelimage);
    sf::Sprite panelsprite(paneltexture);

    sf::Image explosionimage("textures/explosion.png"); // текстура взрыва
    sf::Vector2u xx;
    sf::Color ecolor = explosionimage.getPixel(xx);
    ecolor.a = 200.f;
    explosionimage.setPixel( xx, ecolor );
    sf::Texture explosiontexture(explosionimage);
    sf::Sprite explosionsprite(explosiontexture);

    sf::Image rwimage("textures/rwtex.png"); // текстура рв
    sf::Texture rwtexture(rwimage);
    sf::Sprite rwsprite(rwtexture);
    sf::Vector2f rwPos;
    
    sf::Image lamp1image("textures/lamp1.png"); // текстура лампы 1 правая
    sf::Texture lamp1texture(lamp1image);
    sf::Sprite lamp1sprite(lamp1texture);

    sf::Image lamp2image("textures/lamp2.png"); // текстура лампы 2 левая
    sf::Texture lamp2texture(lamp2image);
    sf::Sprite lamp2sprite(lamp2texture);

    float wrotationspeed = 200.f; // скорость вращения пушки (200)
    float currentAngle = 0.f;
    float angularVelocity = 0.f;
    float reloadtime = 1.5f; // Время перезарядки
    float wrate = 60.f;
    sf::Vector2f vwpos({ rwsprite.getPosition().x + 1453.f + 320.f * resolutionChoise, rwsprite.getPosition().y + 138.f + 45.f * resolutionChoise });
    sf::RectangleShape weapon({ 3000.f, 1.f });
    weapon.setOrigin({0,0});
    weapon.setFillColor(sf::Color(255, 0, 0, 95));
    weapon.setPosition(vwpos);
    bool shaketrigger = 0;

    sf::Image bubblee("textures/bubble.png"); //текстура пузыря
    sf::Texture bubbleTexture(bubblee);
    sf::Sprite bubble(bubbleTexture);

    sf::Image fishh1("textures/fish2.png"); //текстура рыбы
    sf::Texture fish1Texture(fishh1);
    sf::Sprite fish1(fish1Texture);
    fish1.setOrigin({ 80,24 });
    
    sf::Image bullett1("textures/bullet.png"); //текстура снаряда1
    sf::Texture bullet1Texture(bullett1);
    sf::Sprite bullet1(bullet1Texture);
    sf::Vector2f bullet1Pos;
    float bSpeed = 30.f;
    bool b1active = 0;
    sf::Vector2f b1velocity;
    float b1Ang;
    bool shot1 = 0;

    sf::Image bullett2("textures/bullet.png"); //текстура снаряда2
    sf::Texture bullet2Texture(bullett2);
    sf::Sprite bullet2(bullet2Texture);
    sf::Vector2f bullet2Pos;
    bool b2active = 0;
    sf::Vector2f b2velocity;
    float b2Ang;
    bool shot2 = 0;

    sf::Image cannimg("textures/canntex.png"); //текстура пушки
    sf::Texture cannTexture(cannimg);
    sf::Sprite cann(cannTexture);
    cann.setPosition(vwpos);
    cann.setOrigin({30.f,9.f});

    //////////////////////////////////////

    sf::Music song1("sound/aperture.mp3"); //музыка
    song1.setVolume(2);
    song1.setLooping(true);
    song1.play();

    sf::Music cann1("sound/cann1.mp3");
    cann1.setVolume(20);
    sf::Music cann2("sound/cann2.mp3");
    cann2.setVolume(20);
    sf::Music cann3("sound/cann3.mp3");
    cann3.setVolume(20);
    sf::Music cann4("sound/cann4.mp3");
    cann4.setVolume(20);
    sf::Music cann5("sound/cann5.mp3");
    cann5.setVolume(20);

    sf::Music reload("sound/reload.wav");
    reload.setVolume(9);
    sf::Music reload1("sound/reload.wav");
    reload1.setVolume(9);

    sf::Music lamp("sound/lamp.wav");
    lamp.setVolume(2);

    sf::Music hitsound1("sound/hitsound1.wav");
    hitsound1.setVolume(30);
    sf::Music hitsound2("sound/hitsound2.wav");
    hitsound2.setVolume(30);
    sf::Music killsound("sound/killsound.wav");
    killsound.setVolume(45);
    sf::Music creak("sound/creak.wav");
    creak.setVolume(40);
    sf::Music repairsound("sound/repairsound.wav");
    repairsound.setVolume(80);
    sf::Music explosionsound("sound/explosionsounds.wav");
    explosionsound.setVolume(30);

    //////////////////////////////////////

    const sf::Font font("fonts/cour.ttf"); //шрифт

    sf::Text scoretext(font, "", 30);
    int score = 0;
    scoretext.setFillColor(sf::Color(163, 139, 65, 255));

    sf::Text dscoretext(font, "", 30);
    sf::Text dlifetimer(font, "", 30);
    sf::Text dfishcount(font, "", 30);
    sf::Text dshotcount(font, "", 30);
    sf::Text dmoneycount(font, "", 30);

    ////////////////////////////////////// система частиц, таймеры
    
    ParticleSystem particles;
    sf::Vector2f emitterPosition;
    
    FishSystem Fishes;
    sf::Vector2f femitterPosition;

    sf::Clock clock;
    sf::Clock mousePressTimer;
    sf::Clock fishspawndelaytimer;
    sf::Clock bullet1timer;
    sf::Clock bullet2timer;
    sf::Clock timer1shake;
    sf::Clock timer2shake;
    sf::Clock b1timer;
    sf::Clock b2timer;
    sf::Clock lifetimer;
    sf::Clock hardtimer;
    sf::Clock repairtimer;
    hardtimer.start();
    lifetimer.start();

    ////////////////////////////////////// константы

    float fishspawndelay = 1.7f; // Перерыв между спавном рыб в секундах

    float repairtime = randomFloat(15.f, 30.f); // Время починки

    bool hardness = HARDn;  // 0 - отключено усложнение

    int hp = HEALTHn; // хп


    bool pause = 0;

    bool death = 0;
    
    bool kill = 0;

    int dfishcountn = 0;
    int dshotcountn = 0;

    bool isMousePressed = false;

    //////////////////////////////////////

    while (window.isOpen()) {


        while (const std::optional event = window.pollEvent()) // События
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) { // закрытие на esc
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
                    window.close();
                    std::cout << "the escape key was pressed" << std::endl;
                }

                if (keyPressed->scancode == sf::Keyboard::Scan::P)
                {
                    pause = !pause;
                    
                }
            }

            else if (event->is<sf::Event::FocusLost>()){
                pause = 1;
                std::cout << "game paused" << std::endl;
            }

            else if (event->is<sf::Event::FocusGained>()) {
                pause = 0;
                std::cout << "game continued" << std::endl;
            }
            
        }

        //////////////////////////////////////

        sf::Time elapsed = clock.restart();

        if (!pause&&!death) {

            if (!bullet1timer.isRunning()) // Старт таймера после паузы
                bullet1timer.start();
            if (!bullet2timer.isRunning())
                bullet2timer.start();
            if (!hardtimer.isRunning())
                hardtimer.start();
            if (!repairtimer.isRunning())
                repairtimer.start();

            sf::Vector2i mousePos = sf::Mouse::getPosition(window); // Получаем текущую позицию мыши
            sf::Vector2f mouseWorldPos = window.mapPixelToCoords(mousePos); // Преобразуем координаты мыши в мировое пространство
            //std::cout << mouseWorldPos.x << " " << mouseWorldPos.y << '\n';

            window.setView(camera);

            ////////////////////////////////////// Интерфейс

            float halfWidth = static_cast<float>(resolution.x) / 2.f;
            float halfHeight = static_cast<float>(resolution.y) / 2.f;
            
            sf::Vector2f halfResfloat(halfWidth, halfHeight);
            sf::Vector2i halfResint(static_cast<int>(halfResfloat.x), static_cast<int>(halfResfloat.y));

            sf::Vector2f cameraPosition = camera.getCenter();
            sf::Vector2f interfacePos(cameraPosition.x - halfResfloat.x + 330.f * resolutionChoise, cameraPosition.y - halfResfloat.y + 1.f + 360.f * resolutionChoise);
            panelsprite.setPosition(interfacePos);
            lamp1sprite.setPosition(interfacePos);
            lamp2sprite.setPosition(interfacePos);
            rwsprite.setPosition({halfResfloat.x - 795.f, 10.f + 45.f * resolutionChoise});
            rwPos = rwsprite.getPosition();

            std::ostringstream playerScoreString;
            playerScoreString << score;
            scoretext.setString("Score " + playerScoreString.str());
            scoretext.setPosition({interfacePos.x + 585.f, interfacePos.y + 991.f});
            pausesprite.setPosition({ interfacePos.x - 900.f - 400.f * resolutionChoise, interfacePos.y - 500 - 400.f * resolutionChoise });
            deathsprite.setPosition(interfacePos);

            dscoretext.setPosition({ interfacePos.x + 580.f, interfacePos.y + 335.f});
            dfishcount.setPosition({ interfacePos.x + 245.f, interfacePos.y + 237.f });
            dshotcount.setPosition({ interfacePos.x + 380.f, interfacePos.y + 187.f });
            dmoneycount.setPosition({ interfacePos.x + 280.f, interfacePos.y + 380.f });
            dlifetimer.setPosition({ interfacePos.x + 285.f, interfacePos.y + 287.f });

            //////////////////////////////////////  Логика

            WeaponControl(cann, weapon, deltaTime, wrotationspeed, currentAngle, angularVelocity, wrate, mouseWorldPos); // Контроль Пушки

            CameraControl(resolution, camera, deltaTime, fps, window, mouseWorldPos); // Контроль камеры

            if ((b1active) && (timer1shake.getElapsedTime().asSeconds() < 0.15f))
            {
                CameraShake(camera, fps); // Тряска камеры
            }
            if ((b2active) && (timer2shake.getElapsedTime().asSeconds() < 0.15f))
            {
                CameraShake(camera, fps); // Тряска камеры
            }

            //////////////////////////////////////

            if (hardtimer.getElapsedTime().asSeconds() > (15.f*hardness) && fishspawndelay > 0.1f) { // Усложнение игры с течением времени
                hardtimer.restart();
                fishspawndelay -= randomFloat(0.01f, 0.1f);
            }

            if (repairtimer.getElapsedTime().asSeconds() > repairtime && hp < 10 && hp >= 8) { // Починка
                repairtimer.restart();
                hp = hp + 1;
                repairsound.play();
            }

            if (repairtimer.getElapsedTime().asSeconds() > repairtime && hp < 8) { // Починка
                repairtimer.restart();
                hp = hp + randomInt(1,5);
                repairsound.play();
            }


            window.clear();
            window.draw(bgsprite);
            window.draw(rwsprite);

            ////////////////////////////////////// Выстрел 1

            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && (bullet1timer.getElapsedTime().asSeconds() > reloadtime)) // Задание угла и начальной позиции про нажатии спуска
            {
                shaketrigger = 1;
                bullet1timer.restart();
                b1Ang = currentAngle * 3.14f / 180.f;
                sf::Angle tempang(sf::degrees(currentAngle));
                bullet1.setPosition({ weapon.getPosition().x, weapon.getPosition().y + 6.f });
                bullet1.setRotation(tempang);
                sf::Vector2f tempvel(std::cos(b1Ang) * bSpeed, std::sin(b1Ang) * bSpeed);
                b1velocity = tempvel;
                b1active = 1;
                dshotcountn = dshotcountn + 1;

                reload.setPitch(randomFloat(0.97f, 1.1f));
                reload.play();
                int randcann = randomInt(1, 5);
                if (randcann == 1)
                    cann1.play();
                else if (randcann == 2)
                    cann2.play();
                else if (randcann == 3)
                    cann3.play();
                else if (randcann == 4)
                    cann4.play();
                else if (randcann == 5)
                    cann5.play();
            }

            bullet1Pos = bullet1.getPosition();

            if (bullet1Pos.x >= -310.f && bullet1Pos.x <= window.getSize().x + 310.f &&
                bullet1Pos.y >= -310.f && bullet1Pos.y <= window.getSize().y + 310.f && !shot1 && b1active) { // Движение и отрисовка

                bullet1.move(b1velocity);
                window.draw(bullet1);

            }
            else {
                b1active = 0; 
                shot1 = 0;
                timer1shake.restart();
            }

            ////////////////////////////////////// Выстрел 2

            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right) && (bullet2timer.getElapsedTime().asSeconds() > reloadtime)) // Задание угла и начальной позиции про нажатии спуска
            {
                shaketrigger = 1;
                bullet2timer.restart();
                b2Ang = currentAngle * 3.14f / 180.f;
                sf::Angle tempang(sf::degrees(currentAngle));
                bullet2.setPosition({ weapon.getPosition().x, weapon.getPosition().y - 6.f });
                bullet2.setRotation(tempang);
                sf::Vector2f tempvel(std::cos(b2Ang) * bSpeed, std::sin(b2Ang) * bSpeed);
                b2velocity = tempvel;
                b2active = 1;
                dshotcountn = dshotcountn + 1;
                
                reload1.setPitch(randomFloat(0.97f,1.1f));
                reload1.play();
                int randcann = randomInt(1, 5);
                if (randcann == 1)
                    cann1.play();
                else if (randcann == 2)
                    cann2.play();
                else if (randcann == 3)
                    cann3.play();
                else if (randcann == 4)
                    cann4.play();
                else if (randcann == 5)
                    cann5.play();
            }

            bullet2Pos = bullet2.getPosition();

            if (bullet2Pos.x >= -310.f && bullet2Pos.x <= window.getSize().x + 310.f &&
                bullet2Pos.y >= -310.f && bullet2Pos.y <= window.getSize().y + 310.f && !shot2 && b2active) { // Движение и отрисовка

                bullet2.move(b2velocity);
                window.draw(bullet2);

            }
            else {
                b2active = 0;
                shot2 = 0;
                timer2shake.restart();
            }

            ////////////////////////////////////// Система рыб

            if (fishspawndelaytimer.getElapsedTime().asSeconds() >= fishspawndelay) {
                fishspawndelaytimer.restart();

                femitterPosition = sf::Vector2f(randomFloat(0, window.getSize().x), window.getSize().y + 300.f);
                float fspeed = randomFloat(130.f, 150.f);

                sf::Vector2f fdirection{ rwsprite.getPosition().x + randomFloat(0.f,1425.f), rwsprite.getPosition().y }; // Вычисление направления
                fdirection = fdirection - femitterPosition;
                
                float flength = sqrt(fdirection.x * fdirection.x + fdirection.y * fdirection.y); // Нормализация вектора направления в единичный
                if (flength > 0)
                    fdirection /= flength;
                
                sf::Vector2f velocity = fdirection * fspeed; // Добавление скорости

                sf::Color fcolor = sf::Color(randomInt(0, 1), randomInt(135, 140), randomInt(0, 1));
                
                Fishes.addFish(femitterPosition, velocity, fcolor);
            }

            Fishes.update(elapsed, window, fish1, bullet1Pos, bullet2Pos, rwPos, shot1, shot2, b1active, b2active, score, kill);

            ////////////////////////////////////// Система частиц

            if (shot1 && b1timer.getElapsedTime().asMilliseconds() >= 1.0f) {
                b1timer.restart();

                int rhit = randomInt(0, 1);
                if (rhit == 0)
                    hitsound1.play();
                if (rhit == 1)
                    hitsound2.play();
                dfishcountn = dfishcountn + 1;
                for (int i = 0; i < 15; ++i) {
                    float angle = randomFloat(0.f, 360.f) * 3.14f / 180.f;
                    float speed = randomFloat(50.f, 200.f);

                    sf::Vector2f velocity(std::cos(angle)* speed, std::sin(angle)* speed);
                    sf::Color color = sf::Color(randomInt(135, 140), randomInt(135, 140), randomInt(135, 140));


                    particles.addParticle(bullet1Pos, velocity, color);
                }
            }

            if (shot2 && b2timer.getElapsedTime().asMilliseconds() >= 1.0f) {
                b2timer.restart();

                int rhit = randomInt(0, 1);
                if (rhit == 0)
                    hitsound1.play();
                if (rhit == 1)
                    hitsound2.play();
                dfishcountn = dfishcountn + 1;
                for (int i = 0; i < 15; ++i) {
                    float angle = randomFloat(0.f, 360.f) * 3.14f / 180.f;
                    float speed = randomFloat(50.f, 200.f);

                    sf::Vector2f velocity(std::cos(angle) * speed, std::sin(angle) * speed);
                    sf::Color color = sf::Color(randomInt(135, 140), randomInt(135, 140), randomInt(135, 140));


                    particles.addParticle(bullet2Pos, velocity, color);
                }
            }

            particles.update(elapsed, window);

            ////////////////////////////////////// Столкновение

            if (kill)
            {
                std::cout << 1;
                killsound.setPitch(randomFloat(0.9f, 1.05f));
                killsound.play();
                int krands = randomInt(1,8);
                if (krands == 1)
                    creak.play();
                hp = hp - 1;
                kill = 0;
                
            }

            if (hp <= 0) {
                death = 1;
                explosionsound.play();
            }
            ////////////////////////////////////// Отрисовка

            
            
            window.draw(weapon);
            window.draw(cann);
            window.draw(particles);
            window.draw(Fishes);
            window.draw(panelsprite);
            window.draw(scoretext);
            if (bullet2timer.getElapsedTime().asSeconds() > 2.f)
            {
                lamp.play();
                window.draw(lamp1sprite);
            }
            if (bullet1timer.getElapsedTime().asSeconds() > 2.f)
            {
                lamp.play();
                window.draw(lamp2sprite);
            }
                

            //////////////////////////
            /*sf::CircleShape center(5.f);
            sf::CircleShape leftup(5.f);
            center.setPosition(halfResfloat); ////////////----------------------отладочные круги
            leftup.setPosition({ 0,0 });
            window.draw(center); //960 540
            window.draw(leftup); //792 440
            *///////////////////////////

            window.display();

        }
        else if (pause && !death) {
            bullet1timer.stop();
            bullet2timer.stop();
            repairtimer.stop();
            hardtimer.stop();

            window.draw(pausesprite);
            
            window.display();

        }
        else if (death) {
            lifetimer.stop();
            bullet1timer.stop();
            bullet2timer.stop();

            std::ostringstream dplayerScoreString;
            dplayerScoreString << score;
            dscoretext.setString(dplayerScoreString.str());
            
            std::ostringstream dlifetimerstring;
            dlifetimerstring << lifetimer.getElapsedTime().asSeconds();
            dlifetimer.setString(dlifetimerstring.str());

            std::ostringstream dfishcountstring;
            dfishcountstring << dfishcountn;
            dfishcount.setString(dfishcountstring.str());

            std::ostringstream dshotcountstring;
            dshotcountstring << dshotcountn;
            dshotcount.setString(dshotcountstring.str());
            
            std::ostringstream dmoneycountstring;
            dmoneycountstring << dshotcountn * 370;
            dmoneycount.setString(dmoneycountstring.str() + "$");

            window.draw(explosionsprite);
            window.draw(dlifetimer);
            window.draw(dfishcount);
            window.draw(dshotcount);
            window.draw(dmoneycount);
            window.draw(dscoretext);
            window.draw(deathsprite);
            window.display();
        }

    }

    std::cout << '\n' << "Game ended. Score: " << score << " Life time: " << lifetimer.getElapsedTime().asSeconds() << " Killed: " << dfishcountn << " Shells shot: " << dshotcountn << " Cost: " << dshotcountn * 370 << "$" << '\n' << "Restart the game by relaunching the app." << '\n';
    return 0;

}
