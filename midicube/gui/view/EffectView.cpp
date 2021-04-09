/*
 * EffectView.cpp
 *
 *  Created on: Apr 9, 2021
 *      Author: jojo
 */

#include "EffectView.h"
#include "resources.h"

EffectView::EffectView() {
	// TODO Auto-generated constructor stub

}

EffectView::~EffectView() {
	// TODO Auto-generated destructor stub
}

Scene EffectView::create(Frame &frame) {
	std::vector<Control*> controls;

	ActionHandler& handler = frame.cube.action_handler;

	//Background
	Pane* bg = new Pane(sf::Color(80, 80, 80), 0, 0, frame.get_width(), frame.get_height());
	controls.push_back(bg);

	//Exit Button
	Button* exit = new Button("Exit", main_font, 18, frame.get_width() - 70, frame.get_height() - 40, 70, 40);
	exit->set_on_click([&frame]() {
		frame.close();
	});
	exit->rect.setFillColor(sf::Color::Yellow);
	controls.push_back(exit);


	return {controls};
}
