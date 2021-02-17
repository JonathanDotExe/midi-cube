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

		Bank* bank = prog_mgr->get_bank(this->bank);

		//Title
		Label* title = new Label(bank->name + " - Page " + std::to_string(page + 1), main_font, 24, 10, 10);
		controls.push_back(title);

		//Programs
		int rows = 4;
		int cols = 8;
		int pane_width = (frame.get_width() - 15) / cols;
		int pane_height = pane_width;

		const size_t start = page * rows * cols;
		const size_t size = std::min((size_t) (bank->programs.size() - start), (size_t) (rows * cols));
		for (size_t i = 0; i < size; ++i) {
			Program* prog = bank->programs[start + i];

			int x = 10 + pane_width * (i % cols);
			int y = 50 + pane_height * (i / cols);

			//Button
			Button* button = new Button(prog->name, main_font, 16, x, y,  pane_width - 5, pane_height - 5);
			button->rect.setFillColor(sf::Color(200, 200, 200));
			controls.push_back(button);
			button->set_on_click([i, start, this]() {
				prog_mgr->lock();
				prog_mgr->apply_program(this->bank, start + i);
				prog_mgr->unlock();
			});
		}

		//Previous bank
		Button* previous_bank = new Button("<<", main_font, 18, 0, frame.get_height() - 40, 60, 40);
		previous_bank->set_on_click([&frame, this]() {
			frame.change_view(new ProgramView(this->bank - 1, 0));
		});
		previous_bank->rect.setFillColor(sf::Color(200, 200, 200));
		controls.push_back(previous_bank);
		//Previous page
		Button* previous_page = new Button("<", main_font, 18, 60, frame.get_height() - 40, 60, 40);
		previous_page->set_on_click([&frame, this]() {
			frame.change_view(new ProgramView(this->bank, page - 1));
		});
		controls.push_back(previous_page);

		//Next page
		Button* next_page = new Button(">", main_font, 18, frame.get_width() - 70 - 60 * 2, frame.get_height() - 40, 60, 40);
		next_page->set_on_click([&frame, this]() {
			frame.change_view(new ProgramView(this->bank, page + 1));
		});
		controls.push_back(next_page);

		//Next bank
		Button* next_bank = new Button(">>", main_font, 18, frame.get_width() - 70 - 60, frame.get_height() - 40, 60, 40);
		next_bank->set_on_click([&frame, this]() {
			frame.change_view(new ProgramView(this->bank + 1, 0));
		});
		next_bank->rect.setFillColor(sf::Color(200, 200, 200));
		controls.push_back(next_bank);

		//Back Button
		Button* back = new Button("Back", main_font, 18, frame.get_width() - 70, frame.get_height() - 40, 70, 40);
		back->set_on_click([&frame]() {
			frame.change_view(new SoundEngineView());
		});
		back->rect.setFillColor(sf::Color::Yellow);
		controls.push_back(back);
	}
	prog_mgr->unlock();

	return {controls, holders};
}

ProgramView::~ProgramView() {
	// TODO Auto-generated destructor stub
}
