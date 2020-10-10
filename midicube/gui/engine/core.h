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
#include <raylib.h>

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

	int layout_weight = 0;
};


class Node {
protected:
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
	NodeLayout layout;

public:

	Node();

	virtual void update_layout(int parent_width, int parent_height);

	virtual void update_position(int x, int y);

	virtual Vector calc_size(int parent_width, int parent_height, bool fit);

	virtual Vector get_content_size() {
		return {0, 0};
	}

	virtual void draw(int parentX, int parentY, NodeEnv env);

	virtual void on_mouse_pressed(int x, int y, MouseButtonType button, NodeEnv env) {

	}

	virtual Node* traverse_focus(int x, int y) {
		return nullptr;
	}

	virtual void on_mouse_released(int x, int y, MouseButtonType button, NodeEnv env) {

	}

	bool collides (int x, int y);

	virtual ~Node();

	NodeLayout& get_layout() {
		return layout;
	}

	virtual void set_layout(const NodeLayout &layout) {
		this->layout = layout;	//TODO relayout
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
};

class Parent : public Node {

private:
	std::vector<Node*> children;

public:

	Parent();

	virtual void draw(int parentX, int parentY, NodeEnv env);

	virtual void add_child(Node* child);

	virtual Vector get_content_size();

	virtual void update_layout(int parent_width, int parent_height);

	virtual void on_mouse_pressed(int x, int y, MouseButtonType button, NodeEnv env);

	virtual Node* traverse_focus(int x, int y);

	virtual void on_mouse_released(int x, int y, MouseButtonType button, NodeEnv env);

	std::vector<Node*> get_children() {
		return children;
	}

	virtual ~Parent();

};

class Frame;

//View
class ViewController {
public:
	virtual Node* init(Frame* frame) = 0;
	virtual ~ViewController() {

	}
};

//Frame
class Frame {

private:

	ViewController* view;
	Node* root;
	std::string title;
	int width;
	int height;

	Node* focused = nullptr;

public:

	Frame(std::string title, int width, int height);

	void run (ViewController* view);

	void change_view(ViewController* view);

	~Frame();

	int get_height() const {
		return height;
	}

	int get_width() const {
		return width;
	}
};


struct NodeStyle {
	std::string font = "";
	int font_size = 12;
	Color font_color = BLACK;
	Color fill_color = RAYWHITE;
	Color border_color = BLANK;
	float border_radius = 0;
	int border_thickness = 0;
	HorizontalAlignment text_halignment = HorizontalAlignment::CENTER ;
	VerticalAlignment text_valignment = VerticalAlignment::CENTER;
};

class StyleableNode : public Node {

protected:
	NodeStyle style;
	virtual void update_style() {

	}

public:

	StyleableNode();

	virtual void update_layout(int parent_width, int parent_height) {
		Node::update_layout(parent_width, parent_height);
		update_style();
	}

	virtual NodeStyle get_style();

	virtual void set_style(NodeStyle style);

	virtual ~StyleableNode();

};


#endif /* MIDICUBE_GUI_ENGINE_CORE_H_ */
