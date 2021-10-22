/*
 * BOrganView.cpp
 *
 *  Created on: Dec 31, 2020
 *      Author: jojo
 */

#include "../view/B3OrganView.h"

#include "../../resources.h"

B3OrganView::B3OrganView(B3Organ& o) : organ(o), binder{o.get_lock(), [&o]() {
	return new B3OrganView(o);
}, main_font} {

}


Scene B3OrganView::create(ViewHost &frame) {
	std::vector<Control*> controls;
	SpinLock& lock = organ.get_lock();

	//Background
	Pane* bg = new Pane(sf::Color(0x53, 0x32, 0x00), 0, 0, frame.get_width(), frame.get_height());
	controls.push_back(bg);

	//Col 1
	int tmp_y = 20;
	//Col 2
	int tmp_x = 10;

	//Vibrato Mix
	{
		Label* label = new Label("Vibrato Mix", main_font, 18, tmp_x, tmp_y);
		label->text.setFillColor(sf::Color::White);
		controls.push_back(label);
		tmp_y += 25;

		DragBox<double>* vibrato = new DragBox<double>(0, 0, 1, main_font, 16, tmp_x, tmp_y, 80, 60);
		vibrato->property.bind(organ.data.preset.vibrato_mix, lock);
		controls.push_back(vibrato);

		tmp_y += 65;
	}

	//Organ Type
	{
		Label* label = new Label("Organ Type", main_font, 18,tmp_x, tmp_y);
		label->text.setFillColor(sf::Color::White);
		controls.push_back(label);
		tmp_y += 25;

		ComboBox* organ_type = new ComboBox(0, {"B3", "Transistor"}, main_font, 16, 0, tmp_x, tmp_y, 80, 60);
				organ_type->property.bind_cast(organ.data.preset.type, lock);
				controls.push_back(organ_type);

		tmp_y += 65;
	}

	//Click Attack
	{
		Label* label = new Label("Click Attack", main_font, 18, tmp_x, tmp_y);
		label->text.setFillColor(sf::Color::White);
		controls.push_back(label);
		tmp_y += 25;

		DragBox<double>* value = new DragBox<double>(0, 0, 0.0005, main_font, 16, tmp_x, tmp_y, 80, 60);
		value->drag_step = 2;
		value->property.bind(organ.data.preset.click_attack, lock);
		controls.push_back(value);

		tmp_y += 65;
	}
	//High Gain Reduction
	{
		Label* label = new Label("High Gain Reduction", main_font, 18, tmp_x, tmp_y);
		label->text.setFillColor(sf::Color::White);
		controls.push_back(label);
		tmp_y += 25;

		DragBox<double>* value = new DragBox<double>(0, 0, 1, main_font, 16, tmp_x, tmp_y, 80, 60);
		value->property.bind(organ.data.preset.high_gain_reduction, lock);
		controls.push_back(value);

		tmp_y += 65;
	}


	//Swell
	{
		Label* label = new Label("Swell", main_font, 18, tmp_x, tmp_y);
		label->text.setFillColor(sf::Color::White);
		controls.push_back(label);
		tmp_y += 25;

		DragBox<double>* value = new DragBox<double>(1, 0, 1, main_font, 16, tmp_x, tmp_y, 80, 60);
				value->property.bind(organ.data.preset.swell, lock);
				controls.push_back(value);
		tmp_y += 65;
	}

	tmp_x += 175 ;
	tmp_y -= (65 + 25) * 2 - 25;
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
		Drawbar* drawbar = new Drawbar(0, ORGAN_DRAWBAR_MAX, main_font, titles[i], tmp_x, 60, 60, 300, colors[i]);
		drawbar->text.setFillColor(sf::Color::White);
		drawbar->title_text.setFillColor(sf::Color::Yellow);
		drawbar->property.bind(organ.data.preset.drawbars.at(i), lock);
		controls.push_back(drawbar);

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

		OrganSwitch* percussion = new OrganSwitch(false, main_font, tmp_x, tmp_y, 80, 60);
		percussion->property.bind(organ.data.preset.percussion, lock);
		controls.push_back(percussion);

		tmp_y += 65;
	}
	//Percussion Decay
	{
		Label* label = new Label("Perc. Decay", main_font, 18, tmp_x, tmp_y);
		label->text.setFillColor(sf::Color::White);
		controls.push_back(label);
		tmp_y += 25;

		OrganSwitch* percussion_decay = new OrganSwitch(false, main_font, tmp_x, tmp_y, 80, 60, "Fast", "Slow");
		percussion_decay->property.bind(organ.data.preset.percussion_fast_decay, lock);
		controls.push_back(percussion_decay);

		tmp_y += 65;
	}
	//Percussion Soft
	{
		Label* label = new Label("Perc. Volume", main_font, 18, tmp_x, tmp_y);
		label->text.setFillColor(sf::Color::White);
		controls.push_back(label);
		tmp_y += 25;

		OrganSwitch* percussion_volume = new OrganSwitch(false, main_font, tmp_x, tmp_y, 80, 60, "Soft", "Hard");
		percussion_volume->property.bind(organ.data.preset.percussion_soft, lock);
		controls.push_back(percussion_volume);

		tmp_y += 65;
	}
	//Percussion Harmonic
	{
		Label* label = new Label("Perc. Harmonic", main_font, 18, tmp_x, tmp_y);
		label->text.setFillColor(sf::Color::White);
		controls.push_back(label);
		tmp_y += 25;

		OrganSwitch* percussion_harmonic = new OrganSwitch(false, main_font, tmp_x, tmp_y, 80, 60, "3rd", "2nd");
		percussion_harmonic->property.bind(organ.data.preset.percussion_third_harmonic, lock);
		controls.push_back(percussion_harmonic);

		tmp_y += 85;
	}

	//Harmonic Foldback Volume
	{
		Label* foldback_label = new Label("Harm. Foldback Vol.", main_font, 18, tmp_x, tmp_y);
		foldback_label->text.setFillColor(sf::Color::White);
		controls.push_back(foldback_label);
		tmp_y += 25;

		DragBox<double>* foldback = new DragBox<double>(0, 0, 1, main_font, 16, tmp_x, tmp_y, 80, 60);
		foldback->property.bind(organ.data.preset.harmonic_foldback_volume, lock);
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
		gain->property.bind(organ.data.preset.multi_note_gain, lock);
		controls.push_back(gain);

		tmp_y += 65;
	}

	controls.push_back(binder.create_button(frame.get_width() - 170, frame.get_height() - 40, &frame));



	return {controls};
}


B3OrganView::~B3OrganView() {

}

bool B3OrganView::on_action(Control *control) {
	return binder.on_action(control);
}
