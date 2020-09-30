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

class Parent;

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
};

class StyleableNode : public Node {

public:
	NodeStyle style;

public:

	StyleableNode(int x, int y, int width, int height);

	virtual ~StyleableNode();

};


#endif /* MIDICUBE_GUI_ENGINE_GUI_ENGINE_H_ */
