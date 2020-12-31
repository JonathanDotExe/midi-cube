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
	holders.push_back(&channel);

	//Background
	Pane* bg = new Pane(sf::Color(0x8d, 0x56, 0x04), 0, 0, frame.get_width(), frame.get_height());
	controls.push_back(bg);

	//Drawbars
	int tmp_x = 200;
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

	for (size_t i = 0; i < colors.size(); ++i) {
		Drawbar<ORGAN_DRAWBAR_MAX>* drawbar = new Drawbar<ORGAN_DRAWBAR_MAX>(0, main_font, tmp_x, 40, 60, 300, colors[i]);
		controls.push_back(drawbar);
		tmp_x += 80;
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
