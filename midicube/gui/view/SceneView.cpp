/*
 * SceneView.cpp
 *
 *  Created on: Feb 28, 2021
 *      Author: jojo
 */

#include "SceneView.h"

SceneView::SceneView() {

}

void SceneView::property_change(PropertyChange change) {
}

Scene SceneView::create(Frame &frame) {
	std::vector<Control*> controls;
	std::vector<PropertyHolder*> holders;

	//Background
	Pane* bg = new Pane(sf::Color(80, 80, 80), 0, 0, frame.get_width(), frame.get_height());
	controls.push_back(bg);

	this->engine = &frame.cube.engine;




	//Exit Button
	Button* exit = new Button("Exit", main_font, 18, frame.get_width() - 70, frame.get_height() - 40, 70, 40);
	exit->set_on_click([&frame]() {
		frame.close();
	});
	exit->rect.setFillColor(sf::Color::Yellow);
	controls.push_back(exit);


	return {controls, holders};
}


SceneView::~SceneView() {

}
