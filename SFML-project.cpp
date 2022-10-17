#include <memory>
#include <iostream>
#include <math.h>
#include <numbers>
#include <list>

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */



#include <SFML/Graphics.hpp>
//#include <SFML/Mouse.hpp>
long double pi = atan(1) * 4;
bool chaos = true;
bool reload = true;
class Ball {
private:
    std::shared_ptr<sf::CircleShape> shape;
    float velocity;
    float acceleration;
public:
    Ball() {
        shape = std::make_shared<sf::CircleShape>(100.f);
        shape->setPosition(400.f, 400.f);
        shape->setFillColor(sf::Color::Green);
        this->velocity = 0;
        this->acceleration = 0.32f;
    }
    void jump() {
        if (this->shape->getPosition().y == 700) {
            this->velocity = -20.f;
        }
    }
    void update() {
        if (shape->getPosition().y > 700) {
            this->shape->setPosition(this->shape->getPosition().x, 700.f);
            this->velocity = 0;
        }
        else {
            this->velocity += acceleration;
            this->shape->move(0.f, velocity);
        }

    }
    void move(float offsetX) {
        this->shape->move(offsetX,0);
    }
    void render(sf::RenderWindow& win) {
        win.draw(*(this->shape));
    }
};
class CanonBall {
private:
    sf::CircleShape shape;
    long double xVelocity;
    long double yVelocity;
    long double xAcceleration;
    long double yAcceleration;
public:
    CanonBall(int angle) {
        
        this->shape = sf::CircleShape(12.f);
        this->shape.setFillColor(sf::Color::Green);
        double Xmod = cos( pi * abs(angle)/180), Ymod = sin( pi * abs(angle)/180);
        this->shape.setPosition(88+80*Xmod,488-80*Ymod);
        this->xVelocity = 15 * Xmod;
        this->yVelocity = -15 * Ymod;
        if (chaos) {
            double randomizer = ((rand() % 100) - 50);
            randomizer = randomizer / 200;
            this->xVelocity += randomizer;
            this->xVelocity += randomizer;
        }
        this->xAcceleration = 0;
        this->yAcceleration = 0.32;
    }
    void update() {
        if (this->shape.getPosition().y > 500) {
            
            this->yVelocity = -1*abs(0.8*this->yVelocity);           
            this->shape.setPosition(this->shape.getPosition().x, 500);
            if (chaos) {
                double randomizer = ((rand() % 100) - 50);
                randomizer = randomizer / 300;
                this->yVelocity += randomizer;
            }
        }
        else if (this->shape.getPosition().y < 500) {
            this->yAcceleration = 0.32;
        }
        if (abs(this->xVelocity) < 0.02 && this->shape.getPosition().y==500) {
            this->xVelocity = 0;
            this->xAcceleration = 0;
        }
        if (this->yVelocity!=0 && abs(this->yVelocity) < 0.2 && this->shape.getPosition().y == 500 && this->xVelocity!=0) {
            this->yVelocity = 0;
            this->yAcceleration = 0;
            this->xAcceleration = -0.02;
        }
       

        this->yVelocity += this->yAcceleration;
        this->xVelocity += this->xAcceleration;
        this->shape.move(this->xVelocity, this->yVelocity);
        //std::cout << this->xVelocity << "  " << this->yVelocity << "\n";
    }
    bool isOutside(int range) {
        return range < this->shape.getPosition().x;
    }
    void render(sf::RenderWindow& win) {
        win.draw(this->shape);
    }
    bool intersects(CanonBall& b) {
        //double distance = sqrt(pow((b.shape.getPosition().y-this->shape.getPosition().y),2)+ pow((b.shape.getPosition().x - this->shape.getPosition().x), 2));
        sf::FloatRect bounds = this->shape.getGlobalBounds();
        sf::FloatRect other = b.shape.getGlobalBounds();
        return bounds.intersects(other);
        //return distance < this->shape.getRadius();
    }
    bool operator==(CanonBall& a) {
        if (this->yVelocity != a.yVelocity) return false;
        else if (this->xVelocity != a.xVelocity) return false;
        else if (this->shape.getGlobalBounds() != a.shape.getGlobalBounds()) return false;
        return true;
    }
    void handle_collision(CanonBall& b) {
        std::cout << b.shape.getPosition().x << " " << this->shape.getPosition().x << "\n";
        double angleA=atan(this->yVelocity*(-1)/this->xVelocity), angleB=atan(b.yVelocity*(-1)/b.xVelocity);
        double sumVelocityA = sqrt(this->xVelocity * this->xVelocity + this->yVelocity * this->yVelocity);
        double sumVelocityB = sqrt(b.xVelocity * b.xVelocity + b.yVelocity * b.yVelocity);
        if (sumVelocityB == 0) angleB = 0;
        if (sumVelocityA == 0) angleA = 0;
        double angleOfCollison = ((-1)*(b.shape.getPosition().y - this->shape.getPosition().y) / (b.shape.getPosition().x - this->shape.getPosition().x));
        this->xVelocity = sumVelocityB * cos(angleB - angleOfCollison) * cos(angleOfCollison) + sumVelocityA*sin(angleA-angleOfCollison)*cos(angleOfCollison+pi/2);
        this->yVelocity = sumVelocityB * cos(angleB - angleOfCollison) * sin(angleOfCollison) + sumVelocityA * sin(angleA - angleOfCollison) * sin(angleOfCollison + pi / 2);
        
        b.xVelocity = sumVelocityA * cos(angleA - angleOfCollison) * cos(angleOfCollison) + sumVelocityB * sin(angleB - angleOfCollison) * cos(angleOfCollison + pi / 2);
        b.yVelocity = sumVelocityA * cos(angleA - angleOfCollison) * sin(angleOfCollison) + sumVelocityB * sin(angleB - angleOfCollison) * sin(angleOfCollison + pi / 2);
        this->yVelocity *= -1;
        b.yVelocity *= -1;

    }
};
class Canon {
private:
    sf::RectangleShape shape;
    int angle;
    std::list<std::shared_ptr<CanonBall>> balls;
    int reloadTime;
    int sinceFired;
public:
    Canon() {
        this->angle = 0;
        this->shape.setFillColor(sf::Color::White);
        this->shape.setSize(sf::Vector2f(100,50));
        this->shape.setPosition(100, 500);
        this->shape.setOrigin(0, 25);
        this->reloadTime = 5;
        this->sinceFired = 0;
    }
    void update() {
        this->sinceFired++;
        for (auto& a : balls)
        {
            //if (a != nullptr) {
                (*a).update();
                for (auto& b : balls) {
                    if ((*a).intersects(*b) && !((*a)==(*b))) {
                        (*a).handle_collision(*b);
                        (*a).update();
                        (*b).update();
                        //std::cout << "collision!\n";
                    }
                }
                //if ((*a).isOutside(1000)) {
                //    a.reset();
                //    //remove(balls.begin(), balls.end(), nullptr);
                //   }
            //}

        }
        this->balls.remove_if([](std::shared_ptr<CanonBall>& a) {return (*a).isOutside(1000); });
        //std::cout << balls.size() << "\n";
        
    }
    void render(sf::RenderWindow& win) {
        win.draw(this->shape);
        for (auto& a : balls) {
            if(a!=nullptr) (*a).render(win);
        }
    }
    void rotate(int angle) {
        if (this->angle + angle> 0 || this->angle + angle < -90) return;
        this->shape.rotate(angle);
        this->angle += angle;
        //std::cout << this->angle << "\n";
    }
    void fire() {
        if(reload) {
            if (this->sinceFired > this->reloadTime) {
                this->sinceFired = 0;
            }
            else return;
        }
        balls.push_back(std::make_shared<CanonBall>(this->angle));

    }
};

int main()
{
    srand(time(NULL));
    sf::RenderWindow window(sf::VideoMode(1000,800), "SFML works!");
    window.setFramerateLimit(60);
    //Ball a;
    //sf::CircleShape shape(100.f);
   // shape.setPosition(400, 400);
    //shape.setFillColor(sf::Color::Green);
    Canon c;
    while (window.isOpen())
    {
        //std::cout << sf::Mouse::getPosition().x << "  " << sf::Mouse::getPosition(window).y << "\n";
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        //if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        //    a.jump();
        //}
        //if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        //    a.move(-10.f);
        //}
        //else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        //    a.move(10.f);
        //}
        //a.update();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            c.rotate(-1);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            c.rotate(1);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
            c.fire();
        }
        c.update();
        window.clear();
        c.render(window);
        //a.render(window);
        window.display();
    }

    return 0;
}