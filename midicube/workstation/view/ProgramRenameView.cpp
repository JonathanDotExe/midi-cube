/*
 * ProgramRenameView.cpp
 *
 *  Created on: 18 Feb 2021
 *      Author: jojo
 */

#include "../view/ProgramRenameView.h"

#include <boost/algorithm/string.hpp>
#include "../view/ProgramView.h"

ProgramRenameView::ProgramRenameView(MidiCubeWorkstation& c, std::string name, std::function<void(std::string name)> action) : cube(c) {
	this->action = action;
	this->name = name;
}

Scene ProgramRenameView::create(ViewHost &frame) {
	std::vector<Control*> controls;

	{

		//Background
		Pane* bg = new Pane(sf::Color(80, 80, 80), 0, 0, frame.get_width(), frame.get_height());
		controls.push_back(bg);

		Pane* pane = new Pane(sf::Color(120, 120, 120), 5, 5, frame.get_width() - 10, frame.get_height() - 50);
		controls.push_back(pane);

		chars = {' '};
		for (char i = 'a'; i <= 'z'; ++i) {
			chars.push_back(i);
		}
		for (char i = 'A'; i <= 'Z'; ++i) {
			chars.push_back(i);
		}
		for (char i = '0'; i <= '9'; ++i) {
			chars.push_back(i);
		}

		//Rename
		int width = PROGRAM_NAME_LENGTH * 45;
		std::vector<DragBox<int>*> boxes;
		for (size_t i = 0; i < PROGRAM_NAME_LENGTH; ++i) {
			size_t index = 0;
			if (i < name.size()) {
				char ch = name.at(i);
				auto pos = std::find(chars.begin(), chars.end(), ch);
				if (pos != chars.end()) {
					index = pos - chars.begin();
				}
			}

			DragBox<int>* box = new DragBox<int>(index, 0, chars.size() - 1, main_font, 30, frame.get_width()/2 - width/2 + 45 * i, 150, 40, 40);
			box->drag_step = 4;
			box->to_string = [this](int i) {
				return std::string{chars.at(i)};
			};
			boxes.push_back(box);
			controls.push_back(box);
		}

		//Back Button
		Button* back = new Button("Save", main_font, 18, frame.get_width() - 100, frame.get_height() - 40, 100, 40);
		back->set_on_click([&frame, boxes, this]() {
			//Collect name
			std::string name = "";
			for (DragBox<int>* box : boxes) {
				name += chars.at(box->get_value());
			}
			boost::trim(name);
			//Update name
			action(name);
			//Change view
			MidiCubeWorkstation& cube = this->cube;
			frame.menu_back();
		});
		back->rect.setFillColor(sf::Color::Yellow);
		controls.push_back(back);
	}

	return {controls};
}


ProgramRenameView::~ProgramRenameView() {

}
