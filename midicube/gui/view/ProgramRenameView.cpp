/*
 * ProgramRenameView.cpp
 *
 *  Created on: 18 Feb 2021
 *      Author: jojo
 */

#include "ProgramRenameView.h"
#include "ProgramView.h"

ProgramRenameView::ProgramRenameView() {
	// TODO Auto-generated constructor stub

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

		//Rename
		std::vector<DragBox<size_t>*> boxes;
		std::string name = prog_mgr->program_name;
		for (size_t i = 0; i < PROGRAM_NAME_LENGTH; ++i) {
			size_t index = 0;
			if (i < name.size()) {
				char ch = name.at(i);
				auto pos = std::find(chars.begin(), chars.end(), ch);
				if (pos < chars.end()) {
					index = pos - chars.begin();
				}
			}

			DragBox<size_t>* box = new DragBox<size_t>(index, 0, chars.size() - 1, main_font, 18, 50 + 45 * i, 100, 40, 40);
			box->to_string = [&chars](size_t i) {
				return std::to_string(chars.at(i));
			};
			boxes.push_back(box);
			controls.push_back(box);
		}

		//Back Button
		Button* back = new Button("Save", main_font, 18, frame.get_width() - 100, frame.get_height() - 40, 100, 40);
		back->set_on_click([&frame]() {
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
