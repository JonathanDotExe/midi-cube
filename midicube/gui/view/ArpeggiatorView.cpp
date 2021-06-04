/*
 * ArpeggiatorView.cpp
 *
 *  Created on: Dec 29, 2020
 *      Author: jojo
 */

#include "ArpeggiatorView.h"
#include "resources.h"
#include "SoundEngineChannelView.h"

ArpeggiatorView::ArpeggiatorView(SoundEngineChannel& ch, int channel_index) : channel(ch) {
	this->channel_index = channel_index;
}

Scene ArpeggiatorView::create(Frame &frame) {
	std::vector<Control*> controls;


	ActionHandler& handler = frame.cube.action_handler;

	//Sound engines
	std::vector<std::string> patterns{"Up", "Down", "Random", "Up/Down", "Down/Up"};

	//Background
	Pane* bg = new Pane(sf::Color(80, 80, 80), 0, 0, frame.get_width(), frame.get_height());
	controls.push_back(bg);

	Pane* pane = new Pane(sf::Color(120, 120, 120), 5, 5, frame.get_width() - 10, frame.get_height() - 50);
	controls.push_back(pane);

	//Title
	Label* title = new Label("Channel " + std::to_string(channel_index + 1), main_font, 24, 10, 10);
	controls.push_back(title);

	//Col 1
	//Active
	int tmp_y = 40;
	CheckBox* active = new CheckBox(true, "Active", main_font, 18, 10, tmp_y, 40, 40);
	active->property.bind(channel.arp.on, handler);
	controls.push_back(active);

	//Hold
	CheckBox* hold = new CheckBox(true, "Hold", main_font, 18, 150, tmp_y, 40, 40);
	hold->property.bind(channel.arp.preset.hold, handler);
	controls.push_back(hold);
	tmp_y += 50;

	//Pattern
	ComboBox* pattern = new ComboBox(0, patterns, main_font, 24, 0, 10, tmp_y, 300, 60);
	pattern->rect.setFillColor(sf::Color(0, 180, 255));
	pattern->property.bind(channel.arp.preset.pattern, handler);
	controls.push_back(pattern);
	tmp_y += 70;

	//Octave
	{
		Label* octave_label = new Label("Octaves", main_font, 18, 10, tmp_y);
		tmp_y += 30;
		controls.push_back(octave_label);

		DragBox<int>* octave = new DragBox<int>(1, 1, 4, main_font, 18, 10, tmp_y, 150, 60);
		tmp_y += 70;
		octave->property.bind(channel.arp.preset.octaves, handler);
		controls.push_back(octave);
	}

	//BPM
	{
		Label* bpm_label = new Label("BPM", main_font, 18, 10, tmp_y);
		tmp_y += 30;
		controls.push_back(bpm_label);

		DragBox<int>* bpm = new DragBox<int>(0, 10, 480, main_font, 18, 10, tmp_y, 150, 60);
		tmp_y += 70;
		bpm->property.bind_function<unsigned int>(std::bind(&Metronome::get_bpm, &channel.arp.metronome), std::bind(&Metronome::set_bpm, &channel.arp.metronome, std::placeholders::_1), handler);
		controls.push_back(bpm);
	}

	//Step
	{
		Label* step_label = new Label("Step", main_font, 18, 10, tmp_y);
		tmp_y += 30;
		controls.push_back(step_label);

		DragBox<int>* step = new DragBox<int>(0, 1, 32, main_font, 18, 10, tmp_y, 150, 60);
		tmp_y += 70;
		step->property.bind(channel.arp.preset.value, handler);
		controls.push_back(step);
	}

	//Col 2
	tmp_y = 40;
	CheckBox* kb_sync = new CheckBox(true, "KB Sync", main_font, 18, 330, tmp_y, 40, 40);
	kb_sync->property.bind(channel.arp.preset.kb_sync, handler);
	controls.push_back(kb_sync);
	tmp_y += 50;
	CheckBox* repeat_edges = new CheckBox(true, "Repeat Edges", main_font, 18, 330, tmp_y, 40, 40);
	repeat_edges->property.bind(channel.arp.preset.repeat_edges, handler);
	controls.push_back(repeat_edges);
	tmp_y += 50;
	CheckBox* play_duplicates = new CheckBox(true, "Play Duplicates", main_font, 18, 330, tmp_y, 40, 40);
	play_duplicates->property.bind(channel.arp.preset.play_duplicates, handler);
	controls.push_back(play_duplicates);
	tmp_y += 50;
	CheckBox* master_sync = new CheckBox(true, "Master Sync", main_font, 18, 330, tmp_y, 40, 40);
	master_sync->property.bind(channel.arp.preset.master_sync, handler);
	controls.push_back(master_sync);
	tmp_y += 50;

	//Back Button
	Button* back = new Button("Back", main_font, 18, frame.get_width() - 70, frame.get_height() - 40, 70, 40);
	back->set_on_click([&frame, this]() {
		frame.change_view(new SoundEngineChannelView(channel, channel_index));
	});
	controls.push_back(back);

	return {controls};
}


ArpeggiatorView::~ArpeggiatorView() {

}
