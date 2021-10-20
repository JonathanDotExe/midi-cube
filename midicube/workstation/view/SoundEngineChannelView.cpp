/*
 * SoundEngineChannelView.cpp
 *
 *  Created on: Dec 29, 2020
 *      Author: jojo
 */

#include "../view/SoundEngineChannelView.h"

#include "../soundengine/organ.h"
#include "../view/AnalogSynthView.h"
#include "../view/ArpeggiatorView.h"
#include "../view/B3OrganView.h"
#include "../view/EffectView.h"
#include "../view/resources.h"
#include "../view/SamplerView.h"
#include "../view/SoundEngineView.h"

SoundEngineChannelView::SoundEngineChannelView(MidiCube& c, SoundEngineChannel& ch, int channel_index) : cube(c), channel(ch), binder{c.lock, [&c, &ch, channel_index]() {
	return new SoundEngineChannelView(c, ch, channel_index);
}} {

	this->channel_index = channel_index;
}

Scene SoundEngineChannelView::create(ViewHost &frame) {
	std::vector<Control*> controls;

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
	//TODO engine selector
	/*ComboBox* engine = new ComboBox(0, engine_names, main_font, 24, -1, 10, 45, 300, 80);
	engine->rect.setFillColor(sf::Color(0, 180, 255));
	engine->property.bind_function<ssize_t>(std::bind(&SoundEngineChannel::get_engine_index, &channel), std::bind(&SoundEngineChannel::set_engine_index, &channel, std::placeholders::_1), frame.cube.lock);
	controls.push_back(engine);*/

	//Edit
	Button* edit_engine = new Button("Edit", main_font, 18, 10, 130, 300, 60);
	edit_engine->set_on_click([this, &frame]() {
		//TODO not optimal solution
		cube.lock.lock();
		PluginInstance* engine = channel.engine.get_plugin();
		if (engine) {
			frame.change_view(engine->create_view()); //TODO make back function
		}
		cube.lock.unlock();
	});
	controls.push_back(edit_engine);
	//TODO sequencer edit button
	int tmp_y = 260;
	//Effects
	for (size_t i = 0; i < CHANNEL_INSERT_EFFECT_AMOUNT; ++i) {
		//Effect
		//TODO Effect selector
		/*ComboBox* effect = new ComboBox(0, effect_names, main_font, 18, -1, 10, tmp_y, 200, 60);
		effect->rect.setFillColor(sf::Color(128, 255, 255));
		effect->property.bind_function<ssize_t>(std::bind(&InsertEffect::get_effect_index, &channel.effects[i]), std::bind(&InsertEffect::set_effect_index, &channel.effects[i], std::placeholders::_1), frame.cube.lock);
		controls.push_back(effect);*/
		//Edit
		Button* edit_effect = new Button("Edit", main_font, 18, 220, tmp_y, 90, 60);

		edit_effect->set_on_click([this, &frame, i]() {
			cube.lock.lock();
			PluginInstance* effect = channel.effects[i].get_plugin();
			if (effect) {
				frame.change_view(effect->create_view()); //TODO make back function
			}
			cube.lock.unlock();
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
		master_send->property.bind(channel.master_send, cube.lock);
		controls.push_back(master_send);
	}

	tmp_y -= 95;

	//Polyphony
	{
		tmp_y += 5;
		Label* octave_label = new Label("Polyphony", main_font, 18, 170, tmp_y);
		tmp_y += 30;
		controls.push_back(octave_label);

		DragBox<int>* polyphony = new DragBox<int>(0, 0, 256, main_font, 18, 170, tmp_y, 150, 60);
		tmp_y += 60;
		polyphony->property.bind(channel.polyphony_limit, cube.lock);
		controls.push_back(polyphony);
	}


	CheckBox* active = new CheckBox(true, "Active", main_font, 18, 10, frame.get_height() - 95, 40, 40);
	active->property.bind_function<bool>(std::bind(&SoundEngineChannel::is_active, &channel), std::bind(&SoundEngineChannel::set_active, &channel, std::placeholders::_1), cube.lock);
	controls.push_back(active);
	//Col 2
	//Volume
	{
		Label* vol_label = new Label("Vol.", main_font, 24, 330, 10);
		controls.push_back(vol_label);

		Slider* volume = new Slider(0, 0, 1, main_font, 330, 45, 60, 400, 0.7, 0.1);
		volume->property.bind(channel.volume, cube.lock);
		controls.push_back(volume);
	}
	//Panning
	{
		Label* pan_label = new Label("Pan.", main_font, 24, 400, 10);
		controls.push_back(pan_label);

		Slider* pan = new Slider(0, -1, 1, main_font, 400, 45, 60, 400, 0.7, 0.1);
		pan->property.bind(channel.panning, cube.lock);
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
		source->property.bind_function<ssize_t>(std::bind(&SoundEngineChannel::get_input, &channel), std::bind(&SoundEngineChannel::set_input, &channel, std::placeholders::_1), cube.lock);
		tmp_y += 60;
		controls.push_back(source);
	}
	//Octave
	{
		tmp_y += 5;
		Label* octave_label = new Label("Octave", main_font, 18, 470, tmp_y);
		tmp_y += 30;
		controls.push_back(octave_label);

		DragBox<int>* octave = new DragBox<int>(0, -3, 3, main_font, 18, 470, tmp_y, 150, 60);
		tmp_y += 60;
		octave->property.bind_function<int>(std::bind(&SoundEngineChannel::get_octave, &channel), std::bind(&SoundEngineChannel::set_octave, &channel, std::placeholders::_1), cube.lock);
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
		start_note->property.bind_function<unsigned int>(std::bind(&SoundEngineChannel::get_start_note, &channel), std::bind(&SoundEngineChannel::set_start_note, &channel, std::placeholders::_1), cube.lock);
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
		end_note->property.bind_function<unsigned int>(std::bind(&SoundEngineChannel::get_end_note, &channel), std::bind(&SoundEngineChannel::set_end_note, &channel, std::placeholders::_1), cube.lock);
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
		cc->property.bind_function<bool>(std::bind(&SoundEngineChannel::is_transfer_cc, &channel), std::bind(&SoundEngineChannel::set_transfer_cc, &channel, std::placeholders::_1), cube.lock);
		tmp_y += 40;
		controls.push_back(cc);
	}
	//Pitch
	{
		tmp_y += 20;
		CheckBox* pitch = new CheckBox(true, "Bender", main_font, 18, 630, tmp_y, 40, 40);
		pitch->property.bind_function<bool>(std::bind(&SoundEngineChannel::is_transfer_pitch_bend, &channel), std::bind(&SoundEngineChannel::set_transfer_pitch_bend, &channel, std::placeholders::_1), cube.lock);
		tmp_y += 40;
		controls.push_back(pitch);
	}
	//Prog
	{
		tmp_y += 20;
		CheckBox* pitch = new CheckBox(true, "Prog", main_font, 18, 630, tmp_y, 40, 40);
		pitch->property.bind_function<bool>(std::bind(&SoundEngineChannel::is_transfer_prog_change, &channel), std::bind(&SoundEngineChannel::set_transfer_prog_change, &channel, std::placeholders::_1), cube.lock);
		tmp_y += 40;
		controls.push_back(pitch);
	}
	tmp_y = 45;

	//Aftertouch
	{
		tmp_y += 5;
		CheckBox* cc = new CheckBox(true, "Aftertouch", main_font, 18, 790, tmp_y, 40, 40);
		cc->property.bind_function<bool>(std::bind(&SoundEngineChannel::is_transfer_channel_aftertouch, &channel), std::bind(&SoundEngineChannel::set_transfer_channel_aftertouch, &channel, std::placeholders::_1), cube.lock);
		tmp_y += 40;
		controls.push_back(cc);
	}
	//Other
	{
		tmp_y += 20;
		CheckBox* pitch = new CheckBox(true, "Other", main_font, 18, 790, tmp_y, 40, 40);
		pitch->property.bind_function<bool>(std::bind(&SoundEngineChannel::is_transfer_other, &channel), std::bind(&SoundEngineChannel::set_transfer_other, &channel, std::placeholders::_1), cube.lock);
		tmp_y += 40;
		controls.push_back(pitch);
	}

	//Sustain
	{
		tmp_y += 20;
		CheckBox* sustain = new CheckBox(true, "Sustain", main_font, 18, 790, tmp_y, 40, 40);
		sustain->property.bind_function<bool>(std::bind(&SoundEngineChannel::is_sustain, &channel), std::bind(&SoundEngineChannel::set_sustain, &channel, std::placeholders::_1), cube.lock);
		tmp_y += 40;
		controls.push_back(sustain);
	}

	//Pitch Bend
	{
		tmp_y += 20;
		CheckBox* pitch_bend = new CheckBox(true, "Pitch Bend", main_font, 18, 790, tmp_y, 40, 40);
		pitch_bend->property.bind_function<bool>(std::bind(&SoundEngineChannel::is_pitch_bend, &channel), std::bind(&SoundEngineChannel::set_pitch_bend, &channel, std::placeholders::_1), cube.lock);
		tmp_y += 40;
		controls.push_back(pitch_bend);
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
		start_velocity->property.bind_function<unsigned int>(std::bind(&SoundEngineChannel::get_start_velocity, &channel), std::bind(&SoundEngineChannel::set_start_velocity, &channel, std::placeholders::_1), cube.lock);
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
		end_velocity->property.bind_function<unsigned int>(std::bind(&SoundEngineChannel::get_end_velocity, &channel), std::bind(&SoundEngineChannel::set_end_velocity, &channel, std::placeholders::_1), cube.lock);
		controls.push_back(end_velocity);
	}

	controls.push_back(binder.create_button(frame.get_width() - 170, frame.get_height() - 40, &frame));
	//Back Button
	Button* back = new Button("Back", main_font, 18, frame.get_width() - 70, frame.get_height() - 40, 70, 40);
	back->set_on_click([&frame, this]() {
		frame.change_view(new SoundEngineView(cube));
	});
	controls.push_back(back);

	return {controls};
}

SoundEngineChannelView::~SoundEngineChannelView() {

}

bool SoundEngineChannelView::on_action(Control *control) {
	return binder.on_action(control);
}