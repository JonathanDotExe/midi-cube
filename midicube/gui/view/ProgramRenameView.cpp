/*
 * ProgramRenameView.cpp
 *
 *  Created on: 18 Feb 2021
 *      Author: jojo
 */

#include "ProgramRenameView.h"
#include "ProgramView.h"
#include <boost/algorithm/string.hpp>

ProgramRenameView::ProgramRenameView(std::function<void(ProgramManager* prog_mgr)> action) {
	this->action = action;
}

void ProgramRenameView::property_change(PropertyChange change) {
}

Scene ProgramRenameView::create(Frame &frame) {
	std::vector<Control*> controls;
	std::vector<PropertyHolder*> holders;

	prog_mgr = &frame.cube.prog_mgr;

	prog_mgr->lock();
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
		std::string name = prog_mgr->program_name;
		for (size_t i = 0; i < PROGRAM_NAME_LENGTH; ++i) {
			size_t index = 0;
			if (i < name.size()) {
				char ch = name.at(i);
				auto pos = std::find(chars.begin(), chars.end(), ch);
				if (pos != chars.end()) {
					index = pos - chars.begin();
				}
			}

			DragBox<int>* box = new DragBox<int>(index, 0, chars.size() - 1, main_font, 30, 0, 0, 0, 0);
			box->drag_step = 4;
			box->to_string = [this](int i) {
				return std::string{chars.at(i)};
			};
			box->update_position(frame.get_width()/2 - width/2 + 45 * i, 150, 40, 40);
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
			prog_mgr->lock();
			prog_mgr->program_name = name;
			prog_mgr->unlock();
			action(prog_mgr);
			//Change view
			frame.change_view(new ProgramView());
		});
		back->rect.setFillColor(sf::Color::Yellow);
		controls.push_back(back);
	}
	prog_mgr->unlock();

	return {controls, holders};
}


ProgramRenameView::~ProgramRenameView() {
	// TODO Auto-generated destructor stub
}