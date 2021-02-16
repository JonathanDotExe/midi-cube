/*
 * ProgramView.cpp
 *
 *  Created on: Feb 14, 2021
 *      Author: jojo
 */

#include "ProgramView.h"
#include "SoundEngineView.h"

ProgramView::ProgramView(size_t bank, size_t page) {
	this->bank = bank;
	this->page = page;
}

void ProgramView::property_change(PropertyChange change) {
}

Scene ProgramView::create(Frame &frame) {
	std::vector<Control*> controls;
	std::vector<PropertyHolder*> holders;

	engine = &frame.cube.engine;
	prog_mgr = &frame.cube.prog_mgr;

	prog_mgr->lock();
	{
		//Background
		Pane* bg = new Pane(sf::Color(80, 80, 80), 0, 0, frame.get_width(), frame.get_height());
		controls.push_back(bg);

		Pane* pane = new Pane(sf::Color(120, 120, 120), 5, 5, frame.get_width() - 10, frame.get_height() - 50);
		controls.push_back(pane);

		//Title
		Label* title = new Label("Programs", main_font, 24, 10, 10);
		controls.push_back(title);

		//Programs
		int rows = 4;
		int cols = 8;
		int pane_width = (frame.get_width() - 15) / cols;
		int pane_height = (frame.get_height() - 50 - 5) / rows;

		Bank* bank = prog_mgr->get_curr_bank();

		const size_t start = page * rows * cols;
		const size_t size = std::min((size_t) (bank->programs.size() - start), (size_t) (rows * cols));
		for (size_t i = 0; i < size; ++i) {
			Program* prog = bank->programs[start + i];

			int x = 10 + pane_width * (i % cols);
			int y = 10 + pane_height * (i / cols);

			//Button
			Button* button = new Button(prog->name, main_font, 12, x, y,  pane_width, pane_height);
			button->rect.setFillColor(sf::Color(0, 180, 255));
			controls.push_back(button);
			button->set_on_click([i, start]() {
				//TODO
			});
		}

		//Back Button
		Button* back = new Button("Back", main_font, 18, frame.get_width() - 70, frame.get_height() - 40, 70, 40);
		back->set_on_click([&frame]() {
			frame.change_view(new SoundEngineView());
		});
		controls.push_back(back);
	}
	prog_mgr->unlock();

	return {controls, holders};
}

ProgramView::~ProgramView() {
	// TODO Auto-generated destructor stub
}
