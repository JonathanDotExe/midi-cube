/*
 * PluginView.cpp
 *
 *  Created on: Oct 23, 2021
 *      Author: jojo
 */

#include "PluginView.h"

PluginView::PluginView(PluginInstance &p, std::function<ViewController* ()> b) : plugin(p), back(b){
}

PluginView::~PluginView() {
}

Scene PluginView::create(ViewHost &frame) {
	std::vector<Control*> controls;

	//Background
	Pane* bg = new Pane(sf::Color(80, 80, 80), 0, 0, frame.get_width(), frame.get_height());
	controls.push_back(bg);

	//Plugin View
	ViewContainer* container = new ViewContainer(0, 0, frame.get_width(), frame.get_height() - 40);
	container->change_view(plugin.create_view());
	controls.push_back(container);

	//Exit Button
	Button* exit = new Button("Exit", main_font, 18, frame.get_width() - 70, frame.get_height() - 40, 70, 40);
	exit->set_on_click([&frame, this]() {
		frame.change_view(back());
	});
	exit->rect.setFillColor(sf::Color::Yellow);
	controls.push_back(exit);


	return {controls};
}
