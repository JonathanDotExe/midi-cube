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

	StyleableNode(int x, int y, int width, int height);

	virtual NodeStyle get_style();

	virtual void set_style(NodeStyle style);

	virtual ~StyleableNode();

};

class TextPositioner {
private:
	int x_off = 0;
	int y_off = 0;
public:
	void draw(int x, int y, std::string text, NodeStyle& style);
	void recalc(int width, int height, std::string text, NodeStyle& style);
};

class Label : public StyleableNode {

private:
	TextPositioner positioner;
	std::string text;

public:

	Label(std::string text, int x, int y, int width, int height);

	virtual void draw(int parentX, int parentY);

	virtual void update_style();

	virtual ~Label();

};

class Button : public StyleableNode {

private:
	TextPositioner positioner;
	std::string text;
	int inner_padding = 5;

public:

	Button(std::string text, int x, int y, int width, int height);

	virtual void draw(int parentX, int parentY);

	virtual void update_style();

	virtual ~Button();

};


#endif /* MIDICUBE_GUI_ENGINE_GUI_ENGINE_H_ */
