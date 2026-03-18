#include "renderer.hpp"

// Core attributes
sf::RenderWindow* Renderer::window = nullptr;
unsigned int Renderer::width = 1;
unsigned int Renderer::height = 1;

// Core methods
bool Renderer::loop()
{
	if (!window->isOpen())
		window->create(sf::VideoMode({width, height}), "My window");
	while (const std::optional event = Renderer::window->pollEvent())
	{
		// "close requested" event: we close the Renderer::window
		if (event->is<sf::Event::Closed>())
			Renderer::window->close();
	}
    window->display();
	return window->isOpen();
}

void Renderer::background()
{
	window->clear(background_color);
}

void Renderer::init(const unsigned int width_, const unsigned int height_)
{
	window = new sf::RenderWindow(sf::VideoMode({width, height}), "My window");
	width = width_;
	height = height_;
}

void Renderer::draw(const sf::Drawable& drawable)
{
	window->draw(drawable);
}

// Drawing attributes
sf::Color Renderer::fill_color = sf::Color::White;
sf::Color Renderer::stroke_color = sf::Color::Black;
double Renderer::stroke_width = 1.0;
bool Renderer::fill = true;
bool Renderer::stroke = true;
char Renderer::center_mode = CORNER;
int Renderer::nb_points = 50;
sf::Color Renderer::background_color = sf::Color::Black;
double Renderer::rotate = 0.0;

// Shapes
void Renderer::circle(sf::Vector2f position, double radius)
{
		sf::CircleShape circle(radius);

		if (center_mode == CORNER)
			circle.setPosition(position);
		else if (center_mode == CENTER)
			circle.setPosition(position - sf::Vector2f(radius, radius));

		if (fill)
			circle.setFillColor(fill_color);
		else
			circle.setFillColor(sf::Color::Transparent);

		if (stroke)
		{
			circle.setOutlineColor(stroke_color);
			circle.setOutlineThickness(stroke_width);
		}

		circle.setPointCount(nb_points);
		window->draw(circle);
}

void Renderer::circle(double x, double y, double radius)
{
	circle(sf::Vector2f(x, y), radius);
}

void Renderer::rectangle(sf::Vector2f position, double width, double height)
{
	sf::RectangleShape rectangle(sf::Vector2f(width, height));

	if (center_mode == CORNER)
		rectangle.setPosition(position);
	else if (center_mode == CENTER)
		rectangle.setPosition(position - sf::Vector2f(width / 2, height / 2));

	if (fill)
		rectangle.setFillColor(fill_color);
	else
		rectangle.setFillColor(sf::Color::Transparent);

	if (stroke)
	{
		rectangle.setOutlineColor(stroke_color);
		rectangle.setOutlineThickness(stroke_width);
	}

	rectangle.rotate(sf::radians(rotate));

	window->draw(rectangle);
}

void Renderer::rectangle(double x, double y, double width, double height)
{
	rectangle(sf::Vector2f(x, y), width, height);
}

void Renderer::line(sf::Vector2f start, sf::Vector2f end)
{
	sf::Vector2f delta = end - start;
	sf::RectangleShape line({delta.length(), (float)stroke_width});
	sf::Vector2f stroke_width_adjust{0, -(float)stroke_width / 2.0f};
	line.rotate(delta.angle());
	line.setPosition(start + stroke_width_adjust.rotatedBy(delta.angle()));
	line.setFillColor(stroke_color);
	window->draw(line);
}

void Renderer::line(double x1, double y1, double x2, double y2)
{
	line(sf::Vector2f(x1, y1), sf::Vector2f(x2, y2));
}
