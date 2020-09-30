/*
 * gui_engine.h
 *
 *  Created on: Sep 29, 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_ENGINE_GUI_ENGINE_H_
#define MIDICUBE_GUI_ENGINE_GUI_ENGINE_H_

#include <vector>
#include <string>
#include <raylib.h>

enum class VerticalAlignment {
	TOP, CENTER, BOTTOM
};

enum class HorizontalAlignment {
	LEFT, CENTER, RIGHT
};


class Node {
protected:
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;

public:

	Node(int x, int y, int width, int height);

	virtual void draw(int parentX, int parentY);

	virtual ~Node();

};

class Parent : public Node {

private:
	std::vector<Node*> children;

public:

	Parent(int x, int y, int width, int height);

	virtual void draw(int parentX, int parentY);

	virtual void add_child(Node* child);

	virtual ~Parent();

};

struct NodeStyle {
	std::string font;
	int font_size = 0;
	Color font_color;
	Color fill_color;
	Color border_color;
	int border_radius;
	int border_thickness;
	HorizontalAlignment text_halignment;
	VerticalAlignment text_valignment;
};

class StyleableNode : public Node {

protected:
	NodeStyle style;
	virtual void update_style() {

	}

public:

	StyleableNode(int x, int y, int width, int height);

	virtual NodeStyle get_style();

	virtual void set_style(NodeStyle style);

	virtual ~StyleableNode();

};

class Label : public StyleableNode {

private:
	int text_x = 0;
	int text_y = 0;
	std::string text;

public:

	Label(std::string text, int x, int y, int width, int height);

	virtual void draw(int parentX, int parentY);

	virtual void update_style();

	virtual ~Label();

};

class Button : public StyleableNode {

public:

	Button(int x, int y, int width, int height);

	virtual void draw(int parentX, int parentY);

	virtual ~Button();

};


#endif /* MIDICUBE_GUI_ENGINE_GUI_ENGINE_H_ */
