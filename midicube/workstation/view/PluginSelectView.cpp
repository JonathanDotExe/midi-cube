/*
 * PluginSelectView.cpp
 *
 *  Created on: 24 Oct 2021
 *      Author: jojo
 */

#include "PluginSelectView.h"


PluginSelectView::PluginSelectView(PluginSlot &p, std::vector<Plugin*> pl,
		std::function<ViewController* ()> b, size_t page) : plugin(p), plugins(pl), back(b){
	this->page = page;
}

PluginSelectView::~PluginSelectView() {

}

Scene PluginSelectView::create(ViewHost &frame) {
	std::vector<Control*> controls;

	int rows = 4;
	int cols = 8;

	const size_t values = plugins.size() + 1;
	//Adjust values
	page = std::min(std::max((size_t) 0, (size_t) page), (size_t) ceil((double) values/(rows * cols)));

	//Background
	Pane* bg = new Pane(sf::Color(80, 80, 80), 0, 0, frame.get_width(), frame.get_height());
	controls.push_back(bg);

	Pane* pane = new Pane(sf::Color(120, 120, 120), 5, 5, frame.get_width() - 10, frame.get_height() - 50);
	controls.push_back(pane);


	//Title
	Label* title = new Label("Select Plugin - Page " + std::to_string(page + 1), main_font, 24, 10, 10);
	controls.push_back(title);

	//Programs
	int pane_width = (frame.get_width() - 15) / cols;
	int pane_height = pane_width;

	const size_t start = page * rows * cols;
	const size_t size = std::min((size_t) (values - start), (size_t) (rows * cols));
	for (size_t i = 0; i < size; ++i) {
		Plugin* pl = nullptr;
		if (start + i > 0) {
			pl = plugins[start + i];
		}

		int x = 10 + pane_width * (i % cols);
		int y = 50 + pane_height * (i / cols);

		//Button
		Button* button = new Button(plugin, main_font, 16, x, y,  pane_width - 5, pane_height - 5);
		if (prog_mgr->get_curr_bank_index() == this->bank && prog_mgr->get_curr_program_index() == i + start) {
			button->rect.setFillColor(sf::Color(0, 180, 255));
		}
		else {
			button->rect.setFillColor(sf::Color(200, 200, 200));
		}
		controls.push_back(button);
		button->set_on_click([i, start, prog_mgr, &frame, this]() {
			prog_mgr->lock();
			prog_mgr->apply_program(this->bank, start + i);
			prog_mgr->unlock();
			frame.change_view(new ProgramView(cube, this->bank, page));
		});
	}

	//Previous bank
	Button* previous_bank = new Button("<<", main_font, 18, 0, frame.get_height() - 40, 60, 40);
	previous_bank->set_on_click([&frame, this]() {
		frame.change_view(new ProgramView(cube, std::max((ssize_t)this->bank - 1, (ssize_t) 0), 0));
	});
	previous_bank->rect.setFillColor(sf::Color(200, 200, 200));
	controls.push_back(previous_bank);
	//Previous page
	Button* previous_page = new Button("<", main_font, 18, 60, frame.get_height() - 40, 60, 40);
	previous_page->set_on_click([&frame, this]() {
		frame.change_view(new ProgramView(cube, this->bank, std::max((ssize_t) page - 1, (ssize_t) 0)));
	});
	controls.push_back(previous_page);

	//Save
	Button* save = new Button("Save", main_font, 18, 125, frame.get_height() - 40, 100, 40);
	save->set_on_click([&frame, prog_mgr, this]() {
		prog_mgr->lock();
		std::string name = prog_mgr->program_name;
		prog_mgr->unlock();
		frame.change_view(new ProgramRenameView(cube, name, [prog_mgr](std::string name) {
			prog_mgr->lock();
			prog_mgr->program_name = name;
			prog_mgr->overwrite_program();
			prog_mgr->unlock();
		}));
	});
	controls.push_back(save);

	//New
	Button* new_prog = new Button("New", main_font, 18, 230, frame.get_height() - 40, 100, 40);
	new_prog->set_on_click([&frame, prog_mgr, this]() {
		prog_mgr->lock();
		std::string name = prog_mgr->program_name;
		prog_mgr->unlock();
		frame.change_view(new ProgramRenameView(cube, name, [prog_mgr](std::string name) {
			prog_mgr->lock();
			prog_mgr->program_name = name;
			prog_mgr->save_new_program();
			prog_mgr->unlock();
		}));
	});
	controls.push_back(new_prog);

	//New
	Button* init_prog = new Button("New Init", main_font, 18, 335, frame.get_height() - 40, 100, 40);
	init_prog->set_on_click([&frame, prog_mgr, this]() {
		prog_mgr->lock();
		std::string name = prog_mgr->program_name;
		prog_mgr->unlock();
		frame.change_view(new ProgramRenameView(cube, name, [prog_mgr](std::string name) {
			prog_mgr->lock();
			prog_mgr->program_name = name;
			prog_mgr->save_init_program();
			prog_mgr->unlock();
		}));
	});
	controls.push_back(init_prog);

	//Delete
	Button* del = new Button("Delete", main_font, 18, 440, frame.get_height() - 40, 100, 40);
	del->set_on_click([&frame, prog_mgr, this]() {
		prog_mgr->lock();
		prog_mgr->delete_program();
		prog_mgr->unlock();
		frame.change_view(new ProgramView(cube, this->bank, page));
	});
	controls.push_back(del);

	//Rename Bank
	Button* rename = new Button("Rename", main_font, 18, frame.get_width() - 70 - 60 * 2 - 210, frame.get_height() - 40, 100, 40);
	rename->set_on_click([&frame, prog_mgr, this]() {
		prog_mgr->lock();
		std::string name = prog_mgr->bank_name;
		prog_mgr->unlock();
		frame.change_view(new ProgramRenameView(cube, name, [prog_mgr](std::string name) {
			prog_mgr->lock();
			prog_mgr->bank_name = name;
			prog_mgr->overwrite_bank();
			prog_mgr->unlock();
		}));
	});
	controls.push_back(rename);

	//New Bank
	Button* new_bank = new Button("New Bank", main_font, 18, frame.get_width() - 70 - 60 * 2 - 105, frame.get_height() - 40, 100, 40);
	new_bank->set_on_click([&frame, prog_mgr, this]() {
		prog_mgr->lock();
		std::string name = prog_mgr->bank_name;
		prog_mgr->unlock();
		frame.change_view(new ProgramRenameView(cube, name, [prog_mgr](std::string name) {
			prog_mgr->lock();
			prog_mgr->bank_name = name;
			prog_mgr->save_new_bank();
			prog_mgr->unlock();
		}));
	});
	controls.push_back(new_bank);

	//Next page
	Button* next_page = new Button(">", main_font, 18, frame.get_width() - 70 - 60 * 2, frame.get_height() - 40, 60, 40);
	if (start + size < bank->programs.size()) {
		next_page->set_on_click([&frame, this]() {
			frame.change_view(new ProgramView(cube, this->bank, page + 1));
		});
	}
	controls.push_back(next_page);

	//Next bank
	Button* next_bank = new Button(">>", main_font, 18, frame.get_width() - 70 - 60, frame.get_height() - 40, 60, 40);
	next_bank->set_on_click([&frame, this]() {
		frame.change_view(new ProgramView(cube, this->bank + 1, 0));
	});
	next_bank->rect.setFillColor(sf::Color(200, 200, 200));
	controls.push_back(next_bank);

	//Back Button
	Button* back = new Button("Back", main_font, 18, frame.get_width() - 70, frame.get_height() - 40, 70, 40);
	back->set_on_click([&frame, this]() {
		frame.change_view(new SoundEngineView(cube));
	});
	back->rect.setFillColor(sf::Color::Yellow);
	controls.push_back(back);

	return {controls};
}
