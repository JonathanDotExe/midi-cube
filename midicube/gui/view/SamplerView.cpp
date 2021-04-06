/*
 * SamplerView.cpp
 *
 *  Created on: Apr 2, 2021
 *      Author: jojo
 */

#include "SamplerView.h"
#include "resources.h"
#include "SoundEngineChannelView.h"

SamplerView::SamplerView(Sampler &s, SoundEngineChannel &c, int i) : sampler(s), channel(c), channel_index(i) {
}

Scene SamplerView::create(Frame &frame) {
	std::vector<Control*> controls;
	ActionHandler& handler = frame.cube.action_handler;

	//Sound engines
	std::vector<std::string> samples;
	samples.push_back("None");
	for (auto sound : global_sample_store.get_sounds()) {
		samples.push_back(sound->name);
	}

	//Background
	Pane* bg = new Pane(sf::Color(80, 80, 80), 0, 0, frame.get_width(), frame.get_height());
	controls.push_back(bg);

	//Sample Button
	ComboBox* engine = new ComboBox(0, samples, main_font, 24, -1, frame.get_width()/2 - 150, 45, 300, 80);
	engine->rect.setFillColor(sf::Color(0, 180, 255));
	engine->property.bind_function<ssize_t>(std::bind(&Sampler::get_sound_index, &sampler), std::bind(&Sampler::set_sound_index, &sampler, std::placeholders::_1), handler);
	controls.push_back(engine);

	//Back Button
	Button* back = new Button("Back", main_font, 18, frame.get_width() - 70, frame.get_height() - 40, 70, 40);
	back->rect.setFillColor(sf::Color::Yellow);
	back->set_on_click([&frame, this]() {
		frame.change_view(new SoundEngineChannelView(channel, channel_index));
	});
	controls.push_back(back);

	return {controls};
}

SamplerView::~SamplerView() {
	// TODO Auto-generated destructor stub
}

