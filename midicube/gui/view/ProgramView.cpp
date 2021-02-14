/*
 * ProgramView.cpp
 *
 *  Created on: Feb 14, 2021
 *      Author: jojo
 */

#include "ProgramView.h"
#include "SoundEngineView.h"

ProgramView::ProgramView() {
	// TODO Auto-generated constructor stub

}

void ProgramView::property_change(PropertyChange change) {
}

Scene ProgramView::create(Frame &frame) {
	std::vector<Control*> controls;
	std::vector<PropertyHolder*> holders;

	engine = &frame.cube.engine;
	prog_mgr = &frame.cube.prog_mgr;

	holders.push_back(prog_mgr);

	//Background
	Pane* bg = new Pane(sf::Color(80, 80, 80), 0, 0, frame.get_width(), frame.get_height());
	controls.push_back(bg);

	Pane* pane = new Pane(sf::Color(120, 120, 120), 5, 5, frame.get_width() - 10, frame.get_height() - 50);
	controls.push_back(pane);

	//Title
	Label* title = new Label("Programs", main_font, 24, 10, 10);
	controls.push_back(title);

	//Back Button
	Button* back = new Button("Back", main_font, 18, frame.get_width() - 70, frame.get_height() - 40, 70, 40);
	back->set_on_click([&frame]() {
		frame.change_view(new SoundEngineView());
	});
	controls.push_back(back);

	return {controls, holders};
}

ProgramView::~ProgramView() {
	// TODO Auto-generated destructor stub
}
