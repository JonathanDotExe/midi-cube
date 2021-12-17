/*
 * SoundEngineChannelView.cpp
 *
 *  Created on: Dec 29, 2020
 *      Author: jojo
 */

#include "SoundEngineChannelView.h"

#include "../../plugins/soundengine/organ.h"
#include "../../plugins/resources.h"
#include "SoundEngineView.h"
#include "PluginSelectView.h"

SoundEngineChannelView::SoundEngineChannelView(MidiCubeWorkstation& c, SoundEngineChannel& ch, int channel_index) : cube(c), channel(ch), binder{main_font} {

	this->channel_index = channel_index;
}

Scene SoundEngineChannelView::create(ViewHost &frame) {
	std::vector<Control*> controls;

	ActionHandler& handler = frame.get_action_handler();

	//Background
	Pane* bg = new Pane(sf::Color(80, 80, 80), 0, 0, frame.get_width(), frame.get_height());
	controls.push_back(bg);

	Pane* pane = new Pane(sf::Color(120, 120, 120), 5, 5, frame.get_width() - 10, frame.get_height() - 55);
	controls.push_back(pane);

	//Title
	Label* title = new Label("Channel " + std::to_string(channel_index + 1), main_font, 24, 10, 10);
	controls.push_back(title);

	//Col 1
	//Engine
	cube.lock.lock();
	std::string engine_name = channel.engine.get_plugin() ? channel.engine.get_plugin()->get_plugin().info.name : "None";
	cube.lock.unlock();
	Button* engine = new Button(engine_name, main_font, 24, 10, 45, 300, 80);
	engine->rect.setFillColor(sf::Color(0, 180, 255));
	engine->set_on_click([this, &frame]() {
		MidiCubeWorkstation& c = cube;
		SoundEngineChannel& ch = channel;
		frame.change_menu(VIEW_MENU(new PluginSelectView(ch.engine, c.plugin_mgr.get_plugins(PluginType::PLUGIN_TYPE_SOUND_ENGINE), c.lock, c.plugin_mgr, &c.clipboard), &ch, &c));
	});
	controls.push_back(engine);

	//Edit
	Button* edit_engine = new Button("Edit", main_font, 18, 10, 130, 300, 60);
	edit_engine->set_on_click([this, &frame]() {
		//TODO not optimal solution
		cube.lock.lock();
		PluginInstance* engine = channel.engine.get_plugin();
		cube.lock.unlock();
		if (engine) {
			frame.change_menu(engine->create_menu());
		}
	});
	controls.push_back(edit_engine);
	Label* seq_title = new Label("Sequencer", main_font, 18, 10, 200);
	controls.push_back(seq_title);
	int tmp_y = 225;
	//Sequencer
	for (size_t i = 0; i < CHANNEL_SEQUENCER_AMOUNT; ++i) {
		cube.lock.lock();
		std::string sequencer_name = channel.sequencers[i].get_plugin() ? channel.sequencers[i].get_plugin()->get_plugin().info.name : "None";
		cube.lock.unlock();
		Button* sequencer = new Button(sequencer_name, main_font, 18, 10, tmp_y, 200, 60);
		sequencer->rect.setFillColor(sf::Color(128, 255, 255));
		sequencer->set_on_click([this, &frame, i]() {
			MidiCubeWorkstation& c = cube;
			SoundEngineChannel& ch = channel;
			frame.change_menu(VIEW_MENU(new PluginSelectView(ch.sequencers[i], c.plugin_mgr.get_plugins(PluginType::PLUGIN_TYPE_SEQUENCER), c.lock, c.plugin_mgr, &c.clipboard), &ch, &c, i));
		});
		controls.push_back(sequencer);

		Button* edit_sequencer = new Button("Edit", main_font, 18, 220, tmp_y, 90, 60);
		edit_sequencer->set_on_click([this, &frame, i]() {
			cube.lock.lock();
			PluginInstance* sequencer = channel.sequencers[i].get_plugin();
			cube.lock.unlock();
			if (sequencer) {
				frame.change_menu(sequencer->create_menu());
			}
		});
		controls.push_back(edit_sequencer);
		tmp_y += 65;
	}

	Label* effect_title = new Label("Effects", main_font, 18, 10, tmp_y);
	controls.push_back(effect_title);
	tmp_y += 25;
	//Effects
	for (size_t i = 0; i < CHANNEL_INSERT_EFFECT_AMOUNT; ++i) {
		//Effect
		cube.lock.lock();
		std::string effect_name = channel.effects[i].get_plugin() ? channel.effects[i].get_plugin()->get_plugin().info.name : "None";
		cube.lock.unlock();
		Button* effect = new Button(effect_name, main_font, 18, 10, tmp_y, 200, 60);
		effect->rect.setFillColor(sf::Color(128, 255, 255));
		effect->set_on_click([this, &frame, i]() {
			MidiCubeWorkstation& c = cube;
			SoundEngineChannel& ch = channel;
			frame.change_menu(VIEW_MENU(new PluginSelectView(ch.effects[i], c.plugin_mgr.get_plugins(PluginType::PLUGIN_TYPE_EFFECT), c.lock, c.plugin_mgr, &c.clipboard), &ch, &c, i));
		});
		controls.push_back(effect);
		//Edit
		Button* edit_effect = new Button("Edit", main_font, 18, 220, tmp_y, 90, 60);

		edit_effect->set_on_click([this, &frame, i]() {
			cube.lock.lock();
			PluginInstance* effect = channel.effects[i].get_plugin();
			cube.lock.unlock();
			if (effect) {
				frame.change_menu(effect->create_menu());
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

	tmp_y -= 95;

	//Polyphony
	{
		tmp_y += 5;
		Label* octave_label = new Label("Polyphony", main_font, 18, 170, tmp_y);
		tmp_y += 30;
		controls.push_back(octave_label);

		DragBox<int>* polyphony = new DragBox<int>(0, 0, 256, main_font, 18, 170, tmp_y, 150, 60);
		tmp_y += 60;
		polyphony->property.bind(channel.polyphony_limit, handler);
		controls.push_back(polyphony);
	}


	CheckBox* active = new CheckBox(true, "Active", main_font, 18, 10, frame.get_height() - 45, 40, 40);
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
		source->property.bind<ssize_t>(channel.input, handler);
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
		end_note->property.bind_function<unsigned int>(std::bind(&SoundEngineChannel::get_end_note, &channel), std::bind(&SoundEngineChannel::set_end_note, &channel, std::placeholders::_1), handler);
		controls.push_back(end_note);
	}

	//Redirect To
	{
		tmp_y += 5;
		Label* label = new Label("Send To", main_font, 18, 470, tmp_y);
		tmp_y += 30;
		controls.push_back(label);

		DragBox<int>* value = new DragBox<int>(0, -1, 16, main_font, 18, 470, tmp_y, 150, 60);
		tmp_y += 60;
		value->property.bind<int>(channel.redirect.redirect_to, handler);
		controls.push_back(value);
	}

	//Redirect Channel
	{
		tmp_y += 5;
		Label* label = new Label("Send CH", main_font, 18, 470, tmp_y);
		tmp_y += 30;
		controls.push_back(label);

		DragBox<int>* value = new DragBox<int>(0, -1, 16, main_font, 18, 470, tmp_y, 150, 60);
		tmp_y += 60;
		value->property.bind<int>(channel.redirect.channel, handler);
		controls.push_back(value);
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
		CheckBox* pitch = new CheckBox(true, "Bender", main_font, 18, 630, tmp_y, 40, 40);
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
		CheckBox* cc = new CheckBox(true, "Aftertouch", main_font, 18, 790, tmp_y, 40, 40);
		cc->property.bind_function<bool>(std::bind(&SoundEngineChannel::is_transfer_channel_aftertouch, &channel), std::bind(&SoundEngineChannel::set_transfer_channel_aftertouch, &channel, std::placeholders::_1), handler);
		tmp_y += 40;
		controls.push_back(cc);
	}
	//Other
	{
		tmp_y += 20;
		CheckBox* pitch = new CheckBox(true, "Other", main_font, 18, 790, tmp_y, 40, 40);
		pitch->property.bind_function<bool>(std::bind(&SoundEngineChannel::is_transfer_other, &channel), std::bind(&SoundEngineChannel::set_transfer_other, &channel, std::placeholders::_1), handler);
		tmp_y += 40;
		controls.push_back(pitch);
	}

	//Sustain
	{
		tmp_y += 20;
		CheckBox* sustain = new CheckBox(true, "Sustain", main_font, 18, 790, tmp_y, 40, 40);
		sustain->property.bind_function<bool>(std::bind(&SoundEngineChannel::is_sustain, &channel), std::bind(&SoundEngineChannel::set_sustain, &channel, std::placeholders::_1), handler);
		tmp_y += 40;
		controls.push_back(sustain);
	}

	//Pitch Bend
	{
		tmp_y += 20;
		CheckBox* pitch_bend = new CheckBox(true, "Pitch Bend", main_font, 18, 790, tmp_y, 40, 40);
		pitch_bend->property.bind_function<bool>(std::bind(&SoundEngineChannel::is_pitch_bend, &channel), std::bind(&SoundEngineChannel::set_pitch_bend, &channel, std::placeholders::_1), handler);
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

	//MIDI Channel
	{
		tmp_y += 5;
		Label* label = new Label("MIDI Ch", main_font, 18, 630, tmp_y);
		tmp_y += 30;
		controls.push_back(label);

		DragBox<int>* value = new DragBox<int>(0, -1, 16, main_font, 18, 630, tmp_y, 150, 60);
		tmp_y += 60;
		value->property.bind_function<int>(std::bind(&SoundEngineChannel::get_update_channel, &channel), std::bind(&SoundEngineChannel::set_update_channel, &channel, std::placeholders::_1), handler);
		controls.push_back(value);
	}

	//Copy
	Button* copy = new Button("Copy", main_font, 18, frame.get_width() - 370, frame.get_height() - 40, 100, 40);
	copy->set_on_click([&frame, this]() {
		cube.lock.lock();
		channel.copy_channel();
		cube.lock.unlock();
	});
	controls.push_back(copy);

	//Paste
	Button* paste = new Button("Paste", main_font, 18, frame.get_width() - 270, frame.get_height() - 40, 100, 40);
	paste->set_on_click([&frame, this]() {
		cube.lock.lock();
		bool pasted = channel.paste_channel();
		cube.lock.unlock();
		if (pasted) {
			frame.change_view(new SoundEngineChannelView(cube, channel, channel_index)); //FIXME proper refresh
		}
	});
	controls.push_back(paste);
	//Binder
	controls.push_back(binder.create_button(frame.get_width() - 170, frame.get_height() - 40, &frame));
	//Back Button
	Button* back = new Button("Back", main_font, 18, frame.get_width() - 70, frame.get_height() - 40, 70, 40);
	back->set_on_click([&frame, this]() {
		frame.menu_back();
	});
	controls.push_back(back);

	return {controls};
}

SoundEngineChannelView::~SoundEngineChannelView() {

}

bool SoundEngineChannelView::on_action(Control *control) {
	return binder.on_action(control);
}
