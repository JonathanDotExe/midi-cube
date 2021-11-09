/*
 * PluginSelectView.cpp
 *
 *  Created on: 24 Oct 2021
 *      Author: jojo
 */

#include "PluginSelectView.h"


PluginSelectView::PluginSelectView(PluginSlot &p, std::vector<Plugin*> pl, SpinLock& l,
		std::function<ViewController* ()> b, PluginManager& m, Clipboard* clipboard, size_t page) : plugin(p), plugins(pl), lock(l), back(b), mgr(m){
	this->clipboard = clipboard;
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

	lock.lock();
	const size_t start = page * rows * cols;
	const size_t size = std::min((size_t) (values - start), (size_t) (rows * cols));
	for (size_t i = 0; i < size; ++i) {
		Plugin* pl = nullptr;
		if (start + i > 0) {
			pl = plugins.at(start + i - 1);
		}

		int x = 10 + pane_width * (i % cols);
		int y = 50 + pane_height * (i / cols);

		//Button
		Button* button = new Button(pl ? pl->info.name : "None", main_font, 16, x, y,  pane_width - 5, pane_height - 5);
		if ((plugin.get_plugin() == nullptr) ? (pl == nullptr) : (pl == &plugin.get_plugin()->get_plugin())) {
			button->rect.setFillColor(sf::Color(0, 180, 255));
		}
		else {
			button->rect.setFillColor(sf::Color(200, 200, 200));
		}
		controls.push_back(button);
		button->set_on_click([i, start, &frame, this]() {
			Plugin* plugin = nullptr;
			if (start + i > 0) {
				plugin = plugins.at(start + i - 1);
			}
			this->plugin.set_plugin_locked(plugin, lock);
			frame.change_view(new PluginSelectView(this->plugin, plugins, lock, back, mgr, clipboard, page));
		});
	}
	lock.unlock();

	if (clipboard) {
		//Copy
		Button* copy = new Button("Copy", main_font, 18, frame.get_width() - 390, frame.get_height() - 40, 100, 40);
		copy->set_on_click([&frame, this]() {
			lock.lock();
			plugin.copy_plugin(*clipboard);
			lock.unlock();
		});
		controls.push_back(copy);

		//Paste
		Button* paste = new Button("Paste", main_font, 18, frame.get_width() - 290, frame.get_height() - 40, 100, 40);
		paste->set_on_click([&frame, this]() {
			lock.lock();
			bool pasted = plugin.paste_plugin(*clipboard, mgr);	//FIXME execute new without lock
			lock.unlock();
			if (pasted) {
				frame.change_view(this->back());
			}
		});
		controls.push_back(paste);
	}

	//Previous page
	Button* previous_page = new Button("<", main_font, 18, 00, frame.get_height() - 40, 60, 40);
	previous_page->set_on_click([&frame, this]() {
		frame.change_view(new PluginSelectView(plugin, plugins, lock, back, mgr, clipboard, std::max((ssize_t) page - 1, (ssize_t) 0)));
	});
	controls.push_back(previous_page);

	//Next page
	Button* next_page = new Button(">", main_font, 18, frame.get_width() - 70 - 60, frame.get_height() - 40, 60, 40);
	if (start + size < plugins.size()) {
		next_page->set_on_click([&frame, this]() {
			frame.change_view(new PluginSelectView(plugin, plugins, lock, back, mgr, clipboard, std::max((ssize_t) page + 1, (ssize_t) 0)));
		});
	}
	controls.push_back(next_page);

	//Back Button
	Button* back = new Button("Back", main_font, 18, frame.get_width() - 70, frame.get_height() - 40, 70, 40);
	back->set_on_click([&frame, this]() {
		frame.change_view(this->back());
	});
	back->rect.setFillColor(sf::Color::Yellow);
	controls.push_back(back);

	return {controls};
}
