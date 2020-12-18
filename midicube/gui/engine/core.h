/*
 * gui_engine.h
 *
 *  Created on: Sep 29, 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_ENGINE_CORE_H_
#define MIDICUBE_GUI_ENGINE_CORE_H_

#include <vector>
#include <string>
#include <functional>
#include <iostream>
#include <SFML/Graphics.hpp>

#define FOCUS_TRAVERSABLE virtual Node* traverse_focus(int x, int y) { return this; }

enum class VerticalAlignment {
	TOP, CENTER, BOTTOM
};

enum class HorizontalAlignment {
	LEFT, CENTER, RIGHT
};

enum class MouseButtonType {
	LEFT, RIGHT, MIDDLE
};

class Node;

struct NodeEnv {
	Node* focused;
	Node* hovered;
};

struct Vector {
	int x;
	int y;
};

#define MATCH_PARENT -1
#define WRAP_CONTENT -2

struct NodeLayout {
	int width = WRAP_CONTENT;
	int height = WRAP_CONTENT;
	VerticalAlignment valignment = VerticalAlignment::TOP;
	HorizontalAlignment halignment = HorizontalAlignment::LEFT;

	int padding_left = 0;
	int padding_right = 0;
	int padding_top = 0;
	int padding_bottom = 0;

	int margin_left = 0;
	int margin_right = 0;
	int margin_top = 0;
	int margin_bottom = 0;

	int x_weight = 1;
	int y_weight = 1;
};

class Frame;

class Node {
protected:
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
	bool visible = true;
	Frame* frame;

public:

	Node();

	virtual void set_frame(Frame* frame);

	virtual void draw(sf::Window window, NodeEnv env);

	virtual void set_visible(bool visible);

	virtual void update_dim (int x, int y, int width, int height);

	virtual bool is_visible() {
		return visible;
	}

	virtual void on_mouse_pressed(int x, int y, MouseButtonType button, NodeEnv env) {

	}

	virtual void on_mouse_drag(int x, int y, int x_motion, int y_motion, MouseButtonType button, NodeEnv env) {

	}

	virtual Node* traverse_focus(int x, int y) {
		return nullptr;
	}

	virtual void on_mouse_released(int x, int y, MouseButtonType button, NodeEnv env) {

	}

	int get_height() const {
		return height;
	}

	int get_width() const {
		return width;
	}

	int get_x() const {
		return x;
	}

	int get_y() const {
		return y;
	}

	virtual ~Node () {

	}
};

class Frame;

//View
class ViewController {
public:
	virtual std::vector<Node*> init(Frame* frame) = 0;
	virtual ~ViewController() {

	}
};

//Frame
class Frame {

private:

	ViewController* view;
	ViewController* next_view;

	std::vector<Node*> nodes;
	std::string title;
	int width;
	int height;

	int focused_x = -1;
	int focused_y = -1;

	Node* focused = nullptr;
	Node* hovered = nullptr;

	int last_mouse_x = -1;
	int last_mouse_y = -1;

	bool redraw = false;
	bool relayout = false;

public:

	Frame(std::string title, int width, int height);

	void run (ViewController* view);

	void change_view(ViewController* view);

	void request_view(ViewController* view) {
		delete next_view;
		next_view = view;
	}

	void request_relayout() {
		relayout = true;
	}

	void request_redraw() {
		redraw = true;
	}

	~Frame();

	int get_height() const {
		return height;
	}

	int get_width() const {
		return width;
	}
};

struct TextStyle {
	std::string font = "";
	int font_size = 10;
	sf::Color font_color = sf::Color::Black;
	HorizontalAlignment text_halignment = HorizontalAlignment::CENTER;
	VerticalAlignment text_valignment = VerticalAlignment::CENTER;
};

struct BoxStyle {
	sf::Color fill_color = sf::Color::White;
	sf::Color border_color = sf::Color::Transparent;
	sf::Color hover_color = sf::Color::White;
	float border_radius = 0;
	int border_thickness = 0;
};

struct BackgroundStyle {
	sf::Color fill_color = sf::Color::Transparent;
};


#endif /* MIDICUBE_GUI_ENGINE_CORE_H_ */
