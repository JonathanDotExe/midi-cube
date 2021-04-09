/*
 * SoundEngineChannelView.cpp
 *
 *  Created on: Dec 29, 2020
 *      Author: jojo
 */

#include "SoundEngineChannelView.h"
#include "resources.h"
#include "SoundEngineView.h"
#include "ArpeggiatorView.h"
#include "B3OrganView.h"
#include "AnalogSynthView.h"
#include "SamplerView.h"
#include "EffectView.h"
#include "../../soundengine/organ.h"

SoundEngineChannelView::SoundEngineChannelView(SoundEngineChannel& ch, int channel_index) : channel(ch) {
	this->channel_index = channel_index;
}

Scene SoundEngineChannelView::create(Frame &frame) {
	std::vector<Control*> controls;
	ActionHandler& handler = frame.cube.action_handler;

	//Sound engines
	std::vector<std::string> engine_names;
	engine_names.push_back("None");
	for (SoundEngineBuilder* engine : frame.cube.engine.get_engine_builders()) {
		engine_names.push_back(engine->get_name());
	}

	//Effects
	std::vector<std::string> effect_names;
	effect_names.push_back("None");
	for (EffectBuilder* effect : frame.cube.engine.get_effect_builders()) {
		effect_names.push_back(effect->get_name());
	}

	//Background
	Pane* bg = new Pane(sf::Color(80, 80, 80), 0, 0, frame.get_width(), frame.get_height());
	controls.push_back(bg);

	Pane* pane = new Pane(sf::Color(120, 120, 120), 5, 5, frame.get_width() - 10, frame.get_height() - 50);
	controls.push_back(pane);

	//Title
	Label* title = new Label("Channel " + std::to_string(channel_index + 1), main_font, 24, 10, 10);
	controls.push_back(title);

	//Col 1
	//Engine
	ComboBox* engine = new ComboBox(0, engine_names, main_font, 24, -1, 10, 45, 300, 80);
	engine->rect.setFillColor(sf::Color(0, 180, 255));
	engine->property.bind_function<ssize_t>(std::bind(&SoundEngineChannel::get_engine_index, &channel), std::bind(&SoundEngineChannel::set_engine_index, &channel, std::placeholders::_1), handler);
	controls.push_back(engine);

	//Edit
	Button* edit_engine = new Button("Edit", main_font, 18, 10, 130, 300, 60);
	edit_engine->set_on_click([this, &frame]() {
		//TODO not optimal solution
		ssize_t engine_index = channel.get_engine_index();
		if (engine_index >= 0) {
			std::string name = frame.cube.engine.get_engine_builders().at(engine_index)->get_name();
			SoundEngine& en = *channel.get_engine();
			ViewController* view = create_view_for_engine(name, en, channel, channel_index);
			if (view) {
				frame.change_view(view);
			}
		}
	});
	controls.push_back(edit_engine);

	//Arpeggiator
	Button* arpeggiator = new Button("Arpeggiator", main_font, 18, 10, 195, 300, 60);
	arpeggiator->set_on_click([&frame, this]() {
		frame.change_view(new ArpeggiatorView(channel, channel_index));
	});
	controls.push_back(arpeggiator);
	int tmp_y = 260;
	//Effects
	for (size_t i = 0; i < CHANNEL_INSERT_EFFECT_AMOUNT; ++i) {
		//Effect
		ComboBox* effect = new ComboBox(0, effect_names, main_font, 18, -1, 10, tmp_y, 200, 60);
		effect->rect.setFillColor(sf::Color(128, 255, 255));
		effect->property.bind_function<ssize_t>(std::bind(&InsertEffect::get_effect_index, &channel.effects[i]), std::bind(&InsertEffect::set_effect_index, &channel.effects[i], std::placeholders::_1), handler);
		controls.push_back(effect);
		//Edit
		Button* edit_effect = new Button("Edit", main_font, 18, 220, tmp_y, 90, 60);
		edit_effect->set_on_click([this, &frame, i]() {
			Effect* effect = channel.effects[i].get_effect();
			if (effect) {
				frame.change_view(new EffectView(effect));
			}
		});
		controls.push_back(edit_effect);
		tmp_y += 65;
	}
	//Master Effect Send
	{
		tmp_y += 5;
		Label* octave_label = new Label("Master Send", main_font, 18, 10, tmp_y);
		tmp_y += 30;
		controls.push_back(octave_label);

		DragBox<int>* master_send = new DragBox<int>(0, -1, SOUND_ENGINE_MASTER_EFFECT_AMOUNT - 1, main_font, 18, 10, tmp_y, 150, 60);
		tmp_y += 60;
		master_send->property.bind(channel.master_send, handler);
		controls.push_back(master_send);
	}


	CheckBox* active = new CheckBox(true, "Active", main_font, 18, 10, frame.get_height() - 95, 40, 40);
	active->property.bind_function<bool>(std::bind(&SoundEngineChannel::is_active, &channel), std::bind(&SoundEngineChannel::set_active, &channel, std::placeholders::_1), handler);
	controls.push_back(active);
	//Col 2
	//Volume
	{
		Label* vol_label = new Label("Vol.", main_font, 24, 330, 10);
		controls.push_back(vol_label);

		Slider* volume = new Slider(0, 0, 1, main_font, 330, 45, 60, 400, 0.7, 0.1);
		volume->property.bind(channel.volume, handler);
		controls.push_back(volume);
	}
	//Panning
	{
		Label* pan_label = new Label("Pan.", main_font, 24, 400, 10);
		controls.push_back(pan_label);

		Slider* pan = new Slider(0, -1, 1, main_font, 400, 45, 60, 400, 0.7, 0.1);
		pan->property.bind(channel.panning, handler);
		controls.push_back(pan);
	}

	tmp_y = 10;
	//Col 3
	//Source
	{
		tmp_y += 5;
		Label* source_label = new Label("Source", main_font, 18, 470, tmp_y);
		tmp_y += 30;
		controls.push_back(source_label);

		DragBox<int>* source = new DragBox<int>(0, -1, 15, main_font, 18, 470, tmp_y, 150, 60);
		source->property.bind_function<ssize_t>(std::bind(&SoundEngineChannel::get_input, &channel), std::bind(&SoundEngineChannel::set_input, &channel, std::placeholders::_1), handler);
		tmp_y += 60;
		controls.push_back(source);
	}
	//Channel
	{
		tmp_y += 5;
		Label* channel_label = new Label("Channel", main_font, 18, 470, tmp_y);
		tmp_y += 30;
		controls.push_back(channel_label);

		DragBox<int>* input_channel = new DragBox<int>(0, 0, 15, main_font, 18, 470, tmp_y, 150, 60);
		input_channel->property.bind_function<unsigned int>(std::bind(&SoundEngineChannel::get_source_channel, &channel), std::bind(&SoundEngineChannel::set_source_channel, &channel, std::placeholders::_1), handler);
		tmp_y += 60;
		controls.push_back(input_channel);
	}
	//Octave
	{
		tmp_y += 5;
		Label* octave_label = new Label("Octave", main_font, 18, 470, tmp_y);
		tmp_y += 30;
		controls.push_back(octave_label);

		DragBox<int>* octave = new DragBox<int>(0, -3, 3, main_font, 18, 470, tmp_y, 150, 60);
		tmp_y += 60;
		octave->property.bind_function<int>(std::bind(&SoundEngineChannel::get_octave, &channel), std::bind(&SoundEngineChannel::set_octave, &channel, std::placeholders::_1), handler);
		controls.push_back(octave);
	}
	int zone_y = tmp_y;
	//Start Note
	{
		tmp_y += 5;
		Label* start_note_label = new Label("Start Note", main_font, 18, 470, tmp_y);
		tmp_y += 30;
		controls.push_back(start_note_label);

		DragBox<int>* start_note = new DragBox<int>(0, 0, 127, main_font, 18, 470, tmp_y, 150, 60);
		tmp_y += 60;
		start_note->property.bind_function<unsigned int>(std::bind(&SoundEngineChannel::get_start_note, &channel), std::bind(&SoundEngineChannel::set_start_note, &channel, std::placeholders::_1), handler);
		controls.push_back(start_note);
	}

	//End Note
	{
		tmp_y += 5;
		Label* end_note_label = new Label("End Note", main_font, 18, 470, tmp_y);
		tmp_y += 30;
		controls.push_back(end_note_label);

		DragBox<int>* end_note = new DragBox<int>(0, 0, 127, main_font, 18, 470, tmp_y, 150, 60);
		tmp_y += 60;
		end_note->property.bind_function<unsigned int>(std::bind(&SoundEngineChannel::get_start_note, &channel), std::bind(&SoundEngineChannel::set_end_note, &channel, std::placeholders::_1), handler);
		controls.push_back(end_note);
	}

	//Col 4
	tmp_y = 10;
	Label* midi_filter = new Label("MIDI-Filter", main_font, 24, 630, 10);
	controls.push_back(midi_filter);
	tmp_y += 30;
	//CC
	{
		tmp_y += 5;
		CheckBox* cc = new CheckBox(true, "CC", main_font, 18, 630, tmp_y, 40, 40);
		cc->property.bind_function<bool>(std::bind(&SoundEngineChannel::is_transfer_cc, &channel), std::bind(&SoundEngineChannel::set_transfer_cc, &channel, std::placeholders::_1), handler);
		tmp_y += 40;
		controls.push_back(cc);
	}
	//Pitch
	{
		tmp_y += 20;
		CheckBox* pitch = new CheckBox(true, "Pitch", main_font, 18, 630, tmp_y, 40, 40);
		pitch->property.bind_function<bool>(std::bind(&SoundEngineChannel::is_transfer_pitch_bend, &channel), std::bind(&SoundEngineChannel::set_transfer_pitch_bend, &channel, std::placeholders::_1), handler);
		tmp_y += 40;
		controls.push_back(pitch);
	}
	//Prog
	{
		tmp_y += 20;
		CheckBox* pitch = new CheckBox(true, "Prog", main_font, 18, 630, tmp_y, 40, 40);
		pitch->property.bind_function<bool>(std::bind(&SoundEngineChannel::is_transfer_prog_change, &channel), std::bind(&SoundEngineChannel::set_transfer_prog_change, &channel, std::placeholders::_1), handler);
		tmp_y += 40;
		controls.push_back(pitch);
	}
	tmp_y = 45;

	//Aftertouch
	{
		tmp_y += 5;
		CheckBox* cc = new CheckBox(true, "Aftertouch", main_font, 18, 780, tmp_y, 40, 40);
		cc->property.bind_function<bool>(std::bind(&SoundEngineChannel::is_transfer_channel_aftertouch, &channel), std::bind(&SoundEngineChannel::set_transfer_channel_aftertouch, &channel, std::placeholders::_1), handler);
		tmp_y += 40;
		controls.push_back(cc);
	}
	//Other
	{
		tmp_y += 20;
		CheckBox* pitch = new CheckBox(true, "Other", main_font, 18, 780, tmp_y, 40, 40);
		pitch->property.bind_function<bool>(std::bind(&SoundEngineChannel::is_transfer_other, &channel), std::bind(&SoundEngineChannel::set_transfer_other, &channel, std::placeholders::_1), handler);
		tmp_y += 40;
		controls.push_back(pitch);
	}

	tmp_y = zone_y;
	//Start Velocity
	{
		tmp_y += 5;
		Label* start_velocity_label = new Label("Start Velocity", main_font, 18, 630, tmp_y);
		tmp_y += 30;
		controls.push_back(start_velocity_label);

		DragBox<int>* start_velocity = new DragBox<int>(0, 0, 127, main_font, 18, 630, tmp_y, 150, 60);
		tmp_y += 60;
		start_velocity->property.bind_function<unsigned int>(std::bind(&SoundEngineChannel::get_start_velocity, &channel), std::bind(&SoundEngineChannel::set_start_velocity, &channel, std::placeholders::_1), handler);
		controls.push_back(start_velocity);
	}

	//End Velocity
	{
		tmp_y += 5;
		Label* end_velocity_label = new Label("End Velocity", main_font, 18, 630, tmp_y);
		tmp_y += 30;
		controls.push_back(end_velocity_label);

		DragBox<int>* end_velocity = new DragBox<int>(0, 0, 127, main_font, 18, 630, tmp_y, 150, 60);
		tmp_y += 60;
		end_velocity->property.bind_function<unsigned int>(std::bind(&SoundEngineChannel::get_end_velocity, &channel), std::bind(&SoundEngineChannel::set_end_velocity, &channel, std::placeholders::_1), handler);
		controls.push_back(end_velocity);
	}

	//Back Button
	Button* back = new Button("Back", main_font, 18, frame.get_width() - 70, frame.get_height() - 40, 70, 40);
	back->set_on_click([&frame]() {
		frame.change_view(new SoundEngineView());
	});
	controls.push_back(back);

	return {controls};
}

SoundEngineChannelView::~SoundEngineChannelView() {

}

ViewController* create_view_for_engine(std::string name, SoundEngine& engine, SoundEngineChannel& channel, int channel_index) {
	if (get_engine_name<B3Organ>() == name) {
		return new B3OrganView(dynamic_cast<B3Organ&>(engine), channel, channel_index);
	}
	else if (get_engine_name<AnalogSynth>() == name) {
		return new AnalogSynthView(dynamic_cast<AnalogSynth&>(engine), channel, channel_index);
	}
	else if (get_engine_name<Sampler>() == name) {
		return new SamplerView(dynamic_cast<Sampler&>(engine), channel, channel_index);
	}
	return nullptr;
}
