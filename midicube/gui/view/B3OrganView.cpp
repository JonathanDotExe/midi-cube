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

	//Background
	Pane* bg = new Pane(sf::Color(0x53, 0x32, 0x00), 0, 0, frame.get_width(), frame.get_height());
	controls.push_back(bg);

	//Col 1
	int tmp_y = 20;
	//Rotary
	{
		OrganSwitch* rotary = new OrganSwitch(false, "Rotary Speaker", main_font, 18, 10, tmp_y, 80, 60);
		rotary->text.setFillColor(sf::Color::White);
		rotary->bind(&organ, B3OrganProperty::pB3Rotary);
		controls.push_back(rotary);

		tmp_y += 70;
	}
	//Rotary Speed
	{
		OrganSwitch* rotary_speed = new OrganSwitch(false, "Rotary Speed", main_font, 18, 10, tmp_y, 80, 60, "Fast", "Slow");
		rotary_speed->text.setFillColor(sf::Color::White);
		rotary_speed->bind(&organ, B3OrganProperty::pB3RotarySpeed);
		controls.push_back(rotary_speed);

		tmp_y += 80;
	}
	//Distortion Type
	{
		ComboBox* distortion_type = new ComboBox(0, {"Digital", "Analog 1", "Analog 2"}, main_font, 16, 0, 10, tmp_y, 80, 60);
		distortion_type->bind(&organ, B3OrganProperty::pB3DistortionType);
		controls.push_back(distortion_type);

		Label* distortion_type_label = new Label("Distortion Type", main_font, 18, 95, tmp_y + 20);
		distortion_type_label->text.setFillColor(sf::Color::White);
		controls.push_back(distortion_type_label);

		tmp_y += 70;
	}

	//Distortion Amount
	{
		DragBox<double>* overdrive = new DragBox<double>(0, 0, 1, main_font, 16, 10, tmp_y, 80, 60);
		overdrive->bind(&organ, B3OrganProperty::pB3Overdrive);
		controls.push_back(overdrive);

		Label* overdrive_label = new Label("Overdrive", main_font, 18, 95, tmp_y + 20);
		overdrive_label->text.setFillColor(sf::Color::White);
		controls.push_back(overdrive_label);

		tmp_y += 80;
	}

	//Rotary Type
	{
		OrganSwitch* rotary_type = new OrganSwitch(false, "Rotary Type", main_font, 18, 10, tmp_y, 80, 60, "1", "2");
		rotary_type->text.setFillColor(sf::Color::White);
		rotary_type->bind(&organ, B3OrganProperty::pB3RotaryType);
		controls.push_back(rotary_type);

		tmp_y += 70;
	}
	//Normalize overdrive
	{
		OrganSwitch* rotary_type = new OrganSwitch(false, "Normalize Overdrive", main_font, 18, 10, tmp_y, 80, 60);
		rotary_type->text.setFillColor(sf::Color::White);
		rotary_type->bind(&organ, B3OrganProperty::pB3RotaryType);
		controls.push_back(rotary_type);

		tmp_y += 70;
	}
	//Harmonic Foldback Volume
	{
		DragBox<double>* foldback = new DragBox<double>(0, 0, 1, main_font, 16, 10, tmp_y, 80, 60);
		foldback->bind(&organ, B3OrganProperty::pB3HarmonicFoldbackVolume);
		controls.push_back(foldback);

		Label* foldback_label = new Label("Harm. Foldback Vol.", main_font, 18, 95, tmp_y + 20);
		foldback_label->text.setFillColor(sf::Color::White);
		controls.push_back(foldback_label);

		tmp_y += 70;
	}
	//Multi Note Gain
	{
		DragBox<double>* gain = new DragBox<double>(0, 0, 1, main_font, 16, 10, tmp_y, 80, 60);
		gain->bind(&organ, B3OrganProperty::pB3MultiNoteGain);
		controls.push_back(gain);

		Label* gain_label = new Label("Multi Note Gain", main_font, 18, 95, tmp_y + 20);
		gain_label->text.setFillColor(sf::Color::White);
		controls.push_back(gain_label);

		tmp_y += 70;
	}

	//Col 2
	//Drawbars
	int tmp_x = 230;
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
		tmp_x += 70;
	}

	//Back Button
	Button* back = new Button("Back", main_font, 18, frame.get_width() - 70, frame.get_height() - 40, 70, 40);
	back->rect.setFillColor(sf::Color::Yellow);
	back->set_on_click([&frame, this]() {
		frame.change_view(new SoundEngineChannelView(channel, channel_index));
	});
	controls.push_back(back);

	return {controls, holders};
}


B3OrganView::~B3OrganView() {

}
