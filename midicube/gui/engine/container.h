/*
 * container.h
 *
 *  Created on: 10 Oct 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_ENGINE_CONTAINER_H_
#define MIDICUBE_GUI_ENGINE_CONTAINER_H_

#include "core.h"

class VBox : public Parent {
public:

	BackgroundStyle style;

	VBox() : Parent() {
		get_layout().width = MATCH_PARENT;
		get_layout().height = MATCH_PARENT;
	}

	virtual void draw(int parentX, int parentY, NodeEnv env);

	virtual void update_layout(int parent_width, int parent_height);

	virtual Vector get_content_size();

	virtual ~VBox() {

	}
};

class HBox : public Parent {
public:

	BackgroundStyle style;

	HBox() : Parent() {
		get_layout().width = MATCH_PARENT;
		get_layout().height = MATCH_PARENT;
	}

	virtual void draw(int parentX, int parentY, NodeEnv env);

	virtual void update_layout(int parent_width, int parent_height);

	virtual Vector get_content_size();

	virtual ~HBox() {

	}
};

#endif /* MIDICUBE_GUI_ENGINE_CONTAINER_H_ */
