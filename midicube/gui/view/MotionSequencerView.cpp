/*
 * MotionSequencerView.cpp
 *
 *  Created on: Jun 14, 2021
 *      Author: jojo
 */

#include "MotionSequencerView.h"
#include "SoundEngineView.h"

MotionSequencerView::MotionSequencerView() {
	// TODO Auto-generated constructor stub

}

Scene MotionSequencerView::create(Frame &frame) {

	std::vector<Control*> controls;

	ActionHandler& handler = frame.cube.action_handler;

	//Background
	Pane* bg = new Pane(sf::Color(80, 80, 80), 0, 0, frame.get_width(), frame.get_height());
	controls.push_back(bg);
	Pane* pane = new Pane(sf::Color(120, 120, 120), 5, 5, frame.get_width() - 10, frame.get_height() + 10);
	controls.push_back(pane);

	SoundEngineDevice& sound_engine = frame.cube.engine;




	//Exit Button
	Button* exit = new Button("Exit", main_font, 18, frame.get_width() - 75, frame.get_height() - 45, 70, 40);
	exit->set_on_click([&frame]() {
		frame.change_view(new SoundEngineView());
	});
	exit->rect.setFillColor(sf::Color::Yellow);
	controls.push_back(exit);


	return {controls};
}

MotionSequencerView::~MotionSequencerView() {
	// TODO Auto-generated destructor stub
}
