/*
 * SoundEngineChannelView.cpp
 *
 *  Created on: Dec 29, 2020
 *      Author: jojo
 */

#include "SoundEngineChannelView.h"
#include "resources.h"
#include "SoundEngineView.h"

SoundEngineChannelView::SoundEngineChannelView(SoundEngineChannel& ch, int channel_index) : channel(ch) {
	this->channel_index = channel_index;
}

void SoundEngineChannelView::property_change(PropertyChange change) {
}

Scene SoundEngineChannelView::create(Frame &frame) {
	std::vector<Control*> controls;
	std::vector<PropertyHolder*> holders;

	holders.push_back(&channel);

	//Sound engines
	std::vector<std::string> engine_names;
	engine_names.push_back("None");
	for (SoundEngineBank* engine : frame.cube.engine.get_sound_engines()) {
		engine_names.push_back(engine->get_name());
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
	engine->bind(&channel, SoundEngineChannelProperty::pChannelSoundEngine);
	controls.push_back(engine);

	Button* edit_engine = new Button("Edit", main_font, 18, 10, 130, 300, 60);
	controls.push_back(edit_engine);

	CheckBox* active = new CheckBox(true, "Active", main_font, 18, 10, frame.get_height() - 95, 40, 40);
	active->bind(&channel, SoundEngineChannelProperty::pChannelActive);
	controls.push_back(active);
	//Col 2
	//Volume
	{
		Label* vol_label = new Label("Vol.", main_font, 24, 330, 10);
		controls.push_back(vol_label);

		Slider* volume = new Slider(0, 0, 1, main_font, 330, 45, 60, 400, 0.7, 0.1);
		volume->bind(&channel, SoundEngineChannelProperty::pChannelVolume);
		controls.push_back(volume);
	}
	//Panning
	{
		Label* pan_label = new Label("Pan.", main_font, 24, 400, 10);
		controls.push_back(pan_label);

		Slider* pan = new Slider(0, -1, 1, main_font, 400, 45, 60, 400, 0.7, 0.1);
		pan->bind(&channel, SoundEngineChannelProperty::pChannelPanning);
		controls.push_back(pan);
	}

	int tmp_y = 10;
	//Col 3
	//Source
	{
		tmp_y += 5;
		Label* source_label = new Label("Source", main_font, 18, 470, tmp_y);
		tmp_y += 30;
		controls.push_back(source_label);

		DragBox<int>* source = new DragBox<int>(0, -1, 15, main_font, 18, 470, tmp_y, 150, 60);
		source->bind(&channel, SoundEngineChannelProperty::pChannelInputDevice);
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
		input_channel->bind(&channel, SoundEngineChannelProperty::pChannelInputChannel);
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
		octave->bind(&channel, SoundEngineChannelProperty::pChannelOctave);
		controls.push_back(octave);
	}
	//Start Note
	{
		tmp_y += 5;
		Label* start_note_label = new Label("Start Note", main_font, 18, 470, tmp_y);
		tmp_y += 30;
		controls.push_back(start_note_label);

		DragBox<int>* start_note = new DragBox<int>(0, 0, 127, main_font, 18, 470, tmp_y, 150, 60);
		tmp_y += 60;
		start_note->bind(&channel, SoundEngineChannelProperty::pChannelStartNote);
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
		end_note->bind(&channel, SoundEngineChannelProperty::pChannelEndNote);
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
		cc->bind(&channel, SoundEngineChannelProperty::pChannelTransferCC);
		tmp_y += 40;
		controls.push_back(cc);
	}
	//Pitch
	{
		tmp_y += 20;
		CheckBox* pitch = new CheckBox(true, "Pitch", main_font, 18, 630, tmp_y, 40, 40);
		pitch->bind(&channel, SoundEngineChannelProperty::pChannelTransferPitchBend);
		tmp_y += 40;
		controls.push_back(pitch);
	}
	//Prog
	{
		tmp_y += 20;
		CheckBox* pitch = new CheckBox(true, "Prog", main_font, 18, 630, tmp_y, 40, 40);
		pitch->bind(&channel, SoundEngineChannelProperty::pChannelTransferProgChange);
		tmp_y += 40;
		controls.push_back(pitch);
	}
	tmp_y = 45;

	//Aftertouch
	{
		tmp_y += 5;
		CheckBox* cc = new CheckBox(true, "Aftertouch", main_font, 18, 780, tmp_y, 40, 40);
		cc->bind(&channel, SoundEngineChannelProperty::pChannelTransferCC);
		tmp_y += 40;
		controls.push_back(cc);
	}
	//Other
	{
		tmp_y += 20;
		CheckBox* pitch = new CheckBox(true, "Other", main_font, 18, 780, tmp_y, 40, 40);
		pitch->bind(&channel, SoundEngineChannelProperty::pChannelTransferOther);
		tmp_y += 40;
		controls.push_back(pitch);
	}

	//Back Button
	Button* back = new Button("Back", main_font, 18, frame.get_width() - 70, frame.get_height() - 40, 70, 40);
	back->set_on_click([&frame]() {
		frame.change_view(new SoundEngineView());
	});
	controls.push_back(back);

	return {controls, holders};
}


SoundEngineChannelView::~SoundEngineChannelView() {

}
