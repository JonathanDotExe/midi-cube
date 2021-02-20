/*
 * BOrganView.cpp
 *
 *  Created on: Dec 31, 2020
 *      Author: jojo
 */

#include "B3OrganView.h"
#include "resources.h"
#include "SoundEngineChannelView.h"

B3OrganView::B3OrganView(B3Organ& o, SoundEngineChannel& ch, int channel_index) : organ(o), channel(ch) {
	this->channel_index = channel_index;
}

void B3OrganView::property_change(PropertyChange change) {

}

Scene B3OrganView::create(Frame &frame) {
	std::vector<Control*> controls;
	std::vector<PropertyHolder*> holders;
	holders.push_back(&organ);

	std::vector<Control*> hide_midi = {};
	std::vector<Control*> show_midi = {};

	//Background
	Pane* bg = new Pane(sf::Color(0x53, 0x32, 0x00), 0, 0, frame.get_width(), frame.get_height());
	controls.push_back(bg);

	//Col 1
	int tmp_y = 20;
	//Rotary
	{
		Label* label = new Label("Rotary Speaker", main_font, 18, 10, tmp_y);
		label->text.setFillColor(sf::Color::White);
		controls.push_back(label);
		tmp_y += 25;

		DragBox<int>* midi = new DragBox<int>(0, 0, 127, main_font, 16, 10, tmp_y, 80, 60);
		midi->bind(&organ, B3OrganProperty::pB3RotaryCC);
		controls.push_back(midi);
		show_midi.push_back(midi);

		OrganSwitch* rotary = new OrganSwitch(false, main_font, 10, tmp_y, 80, 60);
		rotary->bind(&organ, B3OrganProperty::pB3Rotary);
		hide_midi.push_back(rotary);
		controls.push_back(rotary);

		tmp_y += 65;
	}
	//Rotary Speed
	{
		Label* label = new Label("Rotary Speed", main_font, 18, 10, tmp_y);
		label->text.setFillColor(sf::Color::White);
		controls.push_back(label);
		tmp_y += 25;

		DragBox<int>* midi = new DragBox<int>(0, 0, 127, main_font, 16, 10, tmp_y, 80, 60);
		midi->bind(&organ, B3OrganProperty::pB3RotarySpeedCC);
		controls.push_back(midi);
		show_midi.push_back(midi);

		OrganSwitch* rotary_speed = new OrganSwitch(false, main_font, 10, tmp_y, 80, 60, "Fast", "Slow");
		rotary_speed->bind(&organ, B3OrganProperty::pB3RotarySpeed);
		controls.push_back(rotary_speed);
		hide_midi.push_back(rotary_speed);

		tmp_y += 75;
	}

	//Amplifier
	{
		Label* label = new Label("Amplifier", main_font, 18, 10, tmp_y);
		label->text.setFillColor(sf::Color::White);
		controls.push_back(label);
		tmp_y += 25;

		DragBox<int>* midi = new DragBox<int>(0, 0, 127, main_font, 16, 10, tmp_y, 80, 60);
		midi->bind(&organ, B3OrganProperty::pB3AmpOnCC);
		controls.push_back(midi);
		show_midi.push_back(midi);

		OrganSwitch* amplifier = new OrganSwitch(false, main_font, 10, tmp_y, 80, 60);
		amplifier->bind(&organ, B3OrganProperty::pB3AmpOn);
		controls.push_back(amplifier);
		hide_midi.push_back(amplifier);

		tmp_y += 65;
	}

	//Amplifier Boost
	{
		Label* label = new Label("Amp Boost", main_font, 18, 10, tmp_y);
		label->text.setFillColor(sf::Color::White);
		controls.push_back(label);
		tmp_y += 25;

		DragBox<int>* midi = new DragBox<int>(0, 0, 127, main_font, 16, 10, tmp_y, 80, 60);
		midi->bind(&organ, B3OrganProperty::pB3AmpBoostCC);
		controls.push_back(midi);
		show_midi.push_back(midi);


		DragBox<double>* boost = new DragBox<double>(0, 0, 1, main_font, 16, 10, tmp_y, 80, 60);
		boost->bind(&organ, B3OrganProperty::pB3AmpBoost);
		controls.push_back(boost);
		hide_midi.push_back(boost);

		tmp_y += 65;
	}

	//Distortion Amount
	{
		Label* overdrive_label = new Label("Amp Drive", main_font, 18, 10, tmp_y);
		overdrive_label->text.setFillColor(sf::Color::White);
		controls.push_back(overdrive_label);
		tmp_y += 25;

		DragBox<int>* midi = new DragBox<int>(0, 0, 127, main_font, 16, 10, tmp_y, 80, 60);
		midi->bind(&organ, B3OrganProperty::pB3AmpDriveCC);
		controls.push_back(midi);
		show_midi.push_back(midi);

		DragBox<double>* overdrive = new DragBox<double>(0, 0, 1, main_font, 16, 10, tmp_y, 80, 60);
		overdrive->bind(&organ, B3OrganProperty::pB3AmpDrive);
		controls.push_back(overdrive);
		hide_midi.push_back(overdrive);

		tmp_y += 65;
	}

	//Distortion Type
	{
		Label* distortion_type_label = new Label("Distortion Type", main_font, 18, 10, tmp_y);
		distortion_type_label->text.setFillColor(sf::Color::White);
		controls.push_back(distortion_type_label);
		tmp_y += 25;

		ComboBox* distortion_type = new ComboBox(0, {"Digital", "Polynomal", "Arctan"}, main_font, 16, 0, 10, tmp_y, 80, 60);
		distortion_type->bind(&organ, B3OrganProperty::pB3AmpDistortionType);
		controls.push_back(distortion_type);

		tmp_y += 65;
	}

	//Col 2
	tmp_y -= 65 + 25;
	int tmp_x = 180;
	//Amp Tone
	{
		Label* label = new Label("Amp Tone", main_font, 18, tmp_x, tmp_y);
		label->text.setFillColor(sf::Color::White);
		controls.push_back(label);
		tmp_y += 25;

		DragBox<int>* midi = new DragBox<int>(0, 0, 127, main_font, 16, tmp_x, tmp_y, 80, 60);
		midi->bind(&organ, B3OrganProperty::pB3AmpToneCC);
		controls.push_back(midi);
		show_midi.push_back(midi);

		DragBox<double>* amp_tone = new DragBox<double>(0, 0, 1, main_font, 16, tmp_x, tmp_y, 80, 60);
		amp_tone->bind(&organ, B3OrganProperty::pB3AmpTone);
		controls.push_back(amp_tone);
		hide_midi.push_back(amp_tone);

		tmp_y += 65;
	}
	tmp_y -= 65 + 25;
	tmp_x += 100;

	//Vibrato Mix
	{
		Label* label = new Label("Vibrato Mix", main_font, 18, tmp_x, tmp_y);
		label->text.setFillColor(sf::Color::White);
		controls.push_back(label);
		tmp_y += 25;

		DragBox<int>* midi = new DragBox<int>(0, 0, 127, main_font, 16, tmp_x, tmp_y, 80, 60);
		midi->bind(&organ, B3OrganProperty::pB3VibratoMixCC);
		controls.push_back(midi);
		show_midi.push_back(midi);

		DragBox<double>* vibrato = new DragBox<double>(0, 0, 1, main_font, 16, tmp_x, tmp_y, 80, 60);
		vibrato->bind(&organ, B3OrganProperty::pB3VibratoMix);
		controls.push_back(vibrato);
		hide_midi.push_back(vibrato);

		tmp_y += 65;
	}
	tmp_y -= 65 + 25;
	tmp_x += 100;

	//Rotary Stereo Mix
	{
		Label* label = new Label("Rot. Stereo", main_font, 18, tmp_x, tmp_y);
		label->text.setFillColor(sf::Color::White);
		controls.push_back(label);
		tmp_y += 25;

		DragBox<double>* rotary_stereo = new DragBox<double>(0, 0, 1, main_font, 16, tmp_x, tmp_y, 80, 60);
		rotary_stereo->bind(&organ, B3OrganProperty::pB3RotaryStereoMix);
		controls.push_back(rotary_stereo);

		tmp_y += 65;
	}
	tmp_y -= 65 + 25;
	tmp_x += 100;

	//Rotary Type
	{
		Label* label = new Label("Rotary Type", main_font, 18, tmp_x, tmp_y);
		label->text.setFillColor(sf::Color::White);
		controls.push_back(label);
		tmp_y += 25;

		OrganSwitch* rotary_type = new OrganSwitch(false, main_font, tmp_x, tmp_y, 80, 60, "1", "2");
		rotary_type->bind(&organ, B3OrganProperty::pB3RotaryType);
		controls.push_back(rotary_type);

		tmp_y += 65;
	}

	tmp_x += 100;
	tmp_y -= 65 + 25;
	//Swell
	{
		Label* label = new Label("Swell", main_font, 18, tmp_x, tmp_y);
		label->text.setFillColor(sf::Color::White);
		controls.push_back(label);
		show_midi.push_back(label);
		tmp_y += 25;

		DragBox<int>* midi = new DragBox<int>(0, 0, 127, main_font, 16, tmp_x, tmp_y, 80, 60);
		midi->bind(&organ, B3OrganProperty::pB3SwellCC);
		controls.push_back(midi);
		show_midi.push_back(midi);
		tmp_y += 65;
	}

	tmp_x -= 100 * 4;
	tmp_y -= (65 + 25) * 2;
	//Drawbars
	std::vector<sf::Color> colors = {
			sf::Color(150, 0, 0),
			sf::Color(150, 0, 0),
			sf::Color::White,
			sf::Color::White,
			sf::Color::Black,
			sf::Color::White,
			sf::Color::Black,
			sf::Color::Black,
			sf::Color::White
	};
	std::vector<std::string> titles = {
		"16'",
		"5 1/3'",
		"8'",
		"4'",
		"2 2/3'",
		"2'",
		"1 3/5'",
		"1 1/3",
		"1'"
	};

	for (size_t i = 0; i < colors.size(); ++i) {
		Drawbar<ORGAN_DRAWBAR_MAX>* drawbar = new Drawbar<ORGAN_DRAWBAR_MAX>(0, main_font, titles[i], tmp_x, 60, 60, 300, colors[i]);
		drawbar->text.setFillColor(sf::Color::White);
		drawbar->title_text.setFillColor(sf::Color::Yellow);
		drawbar->bind(&organ, B3OrganProperty::pB3Drawbar1 + i);
		controls.push_back(drawbar);

		DragBox<int>* midi = new DragBox<int>(0, 0, 127, main_font, 16, tmp_x, tmp_y, 60, 60);
		midi->bind(&organ, B3OrganProperty::pB3DrawbarCC1 + i);
		controls.push_back(midi);
		show_midi.push_back(midi);
		tmp_x += 70;
	}

	//Col 3
	tmp_y = 20;
	tmp_x += 20;
	//Percussion
	{
		Label* label = new Label("Percussion", main_font, 18, tmp_x, tmp_y);
		label->text.setFillColor(sf::Color::White);
		controls.push_back(label);
		tmp_y += 25;

		DragBox<int>* midi = new DragBox<int>(0, 0, 127, main_font, 16, tmp_x, tmp_y, 80, 60);
		midi->bind(&organ, B3OrganProperty::pB3PercussionCC);
		controls.push_back(midi);
		show_midi.push_back(midi);

		OrganSwitch* percussion = new OrganSwitch(false, main_font, tmp_x, tmp_y, 80, 60);
		percussion->bind(&organ, B3OrganProperty::pB3Percussion);
		controls.push_back(percussion);
		hide_midi.push_back(percussion);

		tmp_y += 65;
	}
	//Percussion Decay
	{
		Label* label = new Label("Perc. Decay", main_font, 18, tmp_x, tmp_y);
		label->text.setFillColor(sf::Color::White);
		controls.push_back(label);
		tmp_y += 25;

		DragBox<int>* midi = new DragBox<int>(0, 0, 127, main_font, 16, tmp_x, tmp_y, 80, 60);
		midi->bind(&organ, B3OrganProperty::pB3PercussionFastDecayCC);
		controls.push_back(midi);
		show_midi.push_back(midi);

		OrganSwitch* percussion_decay = new OrganSwitch(false, main_font, tmp_x, tmp_y, 80, 60, "Fast", "Slow");
		percussion_decay->bind(&organ, B3OrganProperty::pB3PercussionFastDecay);
		controls.push_back(percussion_decay);
		hide_midi.push_back(percussion_decay);

		tmp_y += 65;
	}
	//Percussion Soft
	{
		Label* label = new Label("Perc. Volume", main_font, 18, tmp_x, tmp_y);
		label->text.setFillColor(sf::Color::White);
		controls.push_back(label);
		tmp_y += 25;

		DragBox<int>* midi = new DragBox<int>(0, 0, 127, main_font, 16, tmp_x, tmp_y, 80, 60);
		midi->bind(&organ, B3OrganProperty::pB3PercussionSoftCC);
		controls.push_back(midi);
		show_midi.push_back(midi);

		OrganSwitch* percussion_volume = new OrganSwitch(false, main_font, tmp_x, tmp_y, 80, 60, "Soft", "Hard");
		percussion_volume->bind(&organ, B3OrganProperty::pB3PercussionSoft);
		controls.push_back(percussion_volume);
		hide_midi.push_back(percussion_volume);

		tmp_y += 65;
	}
	//Percussion Harmonic
	{
		Label* label = new Label("Perc. Harmonic", main_font, 18, tmp_x, tmp_y);
		label->text.setFillColor(sf::Color::White);
		controls.push_back(label);
		tmp_y += 25;

		DragBox<int>* midi = new DragBox<int>(0, 0, 127, main_font, 16, tmp_x, tmp_y, 80, 60);
		midi->bind(&organ, B3OrganProperty::pB3PercussionThirdHarmonicCC);
		controls.push_back(midi);
		show_midi.push_back(midi);

		OrganSwitch* percussion_harmonic = new OrganSwitch(false, main_font, tmp_x, tmp_y, 80, 60, "3rd", "2nd");
		percussion_harmonic->bind(&organ, B3OrganProperty::pB3PercussionThirdHarmonic);
		controls.push_back(percussion_harmonic);
		hide_midi.push_back(percussion_harmonic);

		tmp_y += 85;
	}

	//Harmonic Foldback Volume
	{
		Label* foldback_label = new Label("Harm. Foldback Vol.", main_font, 18, tmp_x, tmp_y);
		foldback_label->text.setFillColor(sf::Color::White);
		controls.push_back(foldback_label);
		tmp_y += 25;

		DragBox<double>* foldback = new DragBox<double>(0, 0, 1, main_font, 16, tmp_x, tmp_y, 80, 60);
		foldback->bind(&organ, B3OrganProperty::pB3HarmonicFoldbackVolume);
		controls.push_back(foldback);

		tmp_y += 65;
	}
	//Multi Note Gain
	{
		Label* gain_label = new Label("Multi Note Gain", main_font, 18, tmp_x, tmp_y);
		gain_label->text.setFillColor(sf::Color::White);
		controls.push_back(gain_label);
		tmp_y += 25;

		DragBox<double>* gain = new DragBox<double>(0.5, 0.5, 1, main_font, 16, tmp_x, tmp_y, 80, 60);
		gain->bind(&organ, B3OrganProperty::pB3MultiNoteGain);
		controls.push_back(gain);

		tmp_y += 65;
	}

	//Edit MIDI Button
	Button* edit_midi = new Button("Edit MIDI", main_font, 18, frame.get_width() - 320, frame.get_height() - 40, 100, 40);
	edit_midi->rect.setFillColor(sf::Color::Yellow);
	edit_midi->set_on_click([show_midi, hide_midi, this]() {
		this->edit_midi = !this->edit_midi;
		for (Control* control : show_midi) {
			control->set_visible(this->edit_midi);
		}
		for (Control* control : hide_midi) {
			control->set_visible(!this->edit_midi);
		}
	});
	controls.push_back(edit_midi);

	//Back Button
	Button* back = new Button("Back", main_font, 18, frame.get_width() - 70, frame.get_height() - 40, 70, 40);
	back->rect.setFillColor(sf::Color::Yellow);
	back->set_on_click([&frame, this]() {
		frame.change_view(new SoundEngineChannelView(channel, channel_index));
	});
	controls.push_back(back);

	//Hide midi controls
	for (Control* control : show_midi) {
		control->set_visible(this->edit_midi);
	}
	for (Control* control : hide_midi) {
		control->set_visible(!this->edit_midi);
	}

	return {controls, holders};
}


B3OrganView::~B3OrganView() {

}
