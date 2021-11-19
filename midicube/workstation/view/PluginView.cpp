/*
 * PluginView.cpp
 *
 *  Created on: Oct 23, 2021
 *      Author: jojo
 */

#include "PluginView.h"

PluginView::PluginView(PluginInstance &p, std::function<ViewController* ()> b, Metronome* metronome, bool* play_metronome, double* master_volume) : plugin(p), back(b){
	this->play_metronome = play_metronome;
	this->metronome = metronome;
	this->master_volume = master_volume;
}

PluginView::~PluginView() {
}

Scene PluginView::create(ViewHost &frame) {
	std::vector<Control*> controls;

	ActionHandler& handler = frame.get_master_host().get_action_handler();

	//Background
	Pane* bg = new Pane(sf::Color(80, 80, 80), 0, 0, frame.get_width(), frame.get_height());
	controls.push_back(bg);

	//Plugin View
	ViewContainer* container = new ViewContainer(0, 0, frame.get_width(), frame.get_height() - 50, plugin.create_view());
	controls.push_back(container);
	this->host = container;

	//Metronome
	if (this->play_metronome) {
		CheckBox* metronome = new CheckBox(false, "Metronome", main_font, 18, 10, frame.get_height() - 45, 40, 40);
		metronome->property.bind(*this->play_metronome, handler);
		controls.push_back(metronome);
	}

	if (this->metronome) {
		DragBox<int>* bpm = new DragBox<int>(120, 10, 480, main_font, 18, 200, frame.get_height() - 45, 100, 40);
		bpm->drag_mul = 0.00125;
		bpm->property.bind_function<unsigned int>(std::bind(&Metronome::get_bpm, this->metronome), std::bind(&Metronome::set_bpm, this->metronome, std::placeholders::_1), handler);
		controls.push_back(bpm);
	}

	//Volume
	if (this->master_volume) {
		DragBox<double>* volume = new DragBox<double>(0, 0, 1, main_font, 18, 330, frame.get_height() - 45, 100, 40);
		volume->property.bind(*master_volume, handler);
		controls.push_back(volume);
	}

	//Exit Button
	Button* exit = new Button("Exit", main_font, 18, frame.get_width() - 70, frame.get_height() - 40, 70, 40);
	exit->set_on_click([&frame, this]() {
		frame.change_view(back());
	});
	exit->rect.setFillColor(sf::Color::Yellow);
	controls.push_back(exit);


	return {controls};
}

ControlView* PluginView::create_control_view() {
	if (host && host->get_view()) {
		std::cout << "Setting view" << std::endl;
		return host->get_view()->create_control_view();
	}
	return nullptr;
}
