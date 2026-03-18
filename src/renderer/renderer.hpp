#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <math.h>

#define CORNER 0
#define CENTER 1

class Renderer
{
public:
	// ======================================================================
	// CORE ATTRIBUTES
	// ======================================================================
    static sf::RenderWindow* window;
	static unsigned int width;
	static unsigned int height;

	// ======================================================================
	// CORE METHODS
	// ======================================================================
	static bool loop();
	static void init(const unsigned int width, const unsigned int height);
	static void draw(const sf::Drawable& drawable);
	static void background();
	
	// ======================================================================
	// DRAWING ATTRIBUTES
	// ======================================================================
	
	static sf::Color fill_color;
	static sf::Color stroke_color;
	static double stroke_width;
	static bool fill;
	static bool stroke;
	static char center_mode;
	static int nb_points;
	static sf::Color background_color;
	static double rotate;

	// ======================================================================
	// SHAPES
	// ======================================================================

	// Circle
	static void circle(sf::Vector2f position, double radius);
	static void circle(double x, double y, double radius);

	// Rectangle
	static void rectangle(sf::Vector2f position, double width, double height);
	static void rectangle(double x, double y, double width, double height);

	// Line
	static void line(sf::Vector2f start, sf::Vector2f end);
	static void line(double x1, double y1, double x2, double y2);
};
