#include "SFML/Graphics.hpp"
#include "escapi.h"
#include <iostream>
#include <fstream>

int main()
{
    int height =  sf::VideoMode::getDesktopMode().height;
    int width =  sf::VideoMode::getDesktopMode().width;
    sf::RenderWindow window(sf::VideoMode(width, height), "webCam");
    int nCameras = 0;
    SimpleCapParams capture;
    nCameras = setupESCAPI();   
    std::cout << nCameras << std::endl;
    if (nCameras == 0) {
        std::cout << "No cameras\n";
        exit(0);
    }

    capture.mWidth = width;
    capture.mHeight = height;
    capture.mTargetBuf = new int[width * height];
    if (initCapture(0, &capture) == 0)
    {
        std::cout << "Couldn't init\n";
        exit(0);
    }
    sf::Clock clock;
    sf::VertexArray vertices;
    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();
        window.setTitle(std::to_string(1.f / dt));
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        vertices.clear();
        vertices.resize(width * height);
        doCapture(0); while (isCaptureDone(0) == 0) {}
        for(int x = 0; x < capture.mWidth; x++)
            for (int y = 0; y < capture.mHeight; y++)
            {
                union RGBint {
                    int rgb;
                    unsigned char c[4];
                };

                RGBint col;
                int index = y * capture.mWidth + x;

                col.rgb = capture.mTargetBuf[index];
                int r = col.c[2];
                int g = col.c[1];
                int b = col.c[0];
                vertices[index].position = sf::Vector2f(x, y);
                vertices[index].color = sf::Color(r, g, b);
            }
        window.clear();
        window.draw(vertices);
        window.display();
    }

    return 0;
}