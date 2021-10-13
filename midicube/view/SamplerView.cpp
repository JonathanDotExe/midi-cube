/*
 * SamplerView.cpp
 *
 *  Created on: Apr 2, 2021
 *      Author: jojo
 */

#include "../view/SamplerView.h"

#include "../view/resources.h"
#include "../view/SoundEngineChannelView.h"

SamplerView::SamplerView(Sampler &s, SoundEngineChannel &c, int i) : sampler(s), channel(c), channel_index(i) {
}

Scene SamplerView::create(Frame &frame) {
	std::vector<Control*> controls;
	ActionHandler& handler = frame.cube.action_handler;


	//Background
	Pane* bg = new Pane(sf::Color(80, 80, 80), 0, 0, frame.get_width(), frame.get_height());
	controls.push_back(bg);

	frame.cube.lock.lock();
	ssize_t index = sampler.get_sound_index();
	SampleSound* sample = sampler.get_sound();
	frame.cube.lock.unlock();

	//Sample Button
	Button* engine = new Button(sample ? sample->name : "None", main_font, 24, 10, 30, 300, 80);
	engine->rect.setFillColor(sf::Color(0, 180, 255));
	engine->set_on_click([this, index, &handler, &frame]() {
		size_t sample_amount = global_sample_store.get_sounds().size();
		if (index < (ssize_t) sample_amount - 1) {
			frame.cube.lock.lock();
			sampler.set_sound_index(index + 1);
			frame.cube.lock.unlock();
		}
		else {
			frame.cube.lock.lock();
			sampler.set_sound_index(-1);
			frame.cube.lock.unlock();
		}
		frame.change_view(new SamplerView(sampler, channel, channel_index));
	});
	controls.push_back(engine);

	//Preset
	std::vector<std::string> presets = {};
	if (sample) {
		for (SamplePreset preset : sample->presets) {
			presets.push_back(preset.name);
		}
	}
	if (presets.empty()) {
		presets.push_back("Default");
	}

	ComboBox* preset = new ComboBox(0, presets, main_font, 24, 0, 10, 120, 300, 80);
	preset->rect.setFillColor(sf::Color(0, 180, 255));
	preset->property.bind_function<size_t>(std::bind(&Sampler::get_preset_index, &sampler), std::bind(&Sampler::set_preset_index, &sampler, std::placeholders::_1), handler);
	controls.push_back(preset);


	//Create controls
	//Can be used without lock because the values are only read after loading
	int x = 400;
	int y = 30;
	if (sample) {
		for (SampleControl& control : sample->controls) {
			if (control.save) {
				Label* name = new Label(control.name, main_font, 12, x, y);
				controls.push_back(name);

				DragBox<double>* value = new DragBox<double>(0, 0, 1, main_font, 16, x, y + 15, 80, 40);
				value->property.bind(sampler.cc[control.cc], handler);
				controls.push_back(value);

				x += 90;
				if (x > frame.get_width() - 90) {
					x = 400;
					y += 60;
				}
			}
		}
	}

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
