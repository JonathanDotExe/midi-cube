/*
 * LooperView.cpp
 *
 *  Created on: Apr 14, 2021
 *      Author: jojo
 */

#include "../view/LooperView.h"
#include "../../plugins/resources.h"

LooperView::LooperView(Looper& l) : looper(l){

}

LooperView::~LooperView() {

}

void LooperView::update_record() {
	ssize_t i = 0;
	for (Button* button : records) {
		if (i == record_index) {
			button->rect.setFillColor(sf::Color::Red);
		}
		else {
			button->rect.setFillColor(sf::Color(220, 220, 220));
		}
		++i;
	}
}

void LooperView::update_solo() {
	ssize_t i = 0;
	for (Button* button : solos) {
		if (i == solo_index) {
			button->rect.setFillColor(sf::Color::Blue);
		}
		else {
			button->rect.setFillColor(sf::Color(220, 220, 220));
		}
		++i;
	}
}

Scene LooperView::create(ViewHost &frame) {
	std::vector<Control*> controls;
	SpinLock& lock = looper.get_lock();
	ActionHandler& handler = frame.get_action_handler();

	//Background
	Pane* bg = new Pane(sf::Color(80, 80, 80), 0, 0, frame.get_width(), frame.get_height());
	controls.push_back(bg);

	//Channels
	int rows = 2;
	int cols = LOOPER_CHANNELS;
	int pane_width = (frame.get_width() - 15) / cols;
	int pane_height = (frame.get_height() - 50 - 5) / rows;
	for (size_t i = 0; i < LOOPER_CHANNELS; ++i) {
		//Background pane
		int x = 10 + pane_width * (i % cols);
		int y = 10 + pane_height * (i / cols);
		Pane* pane = new Pane(sf::Color(120, 120, 120), x, y, pane_width - 5, pane_height - 5);

		controls.push_back(pane);

		//Title
		Label* title = new Label("Channel " + std::to_string(i + 1), main_font, 16, x + 5, y + 5);
		controls.push_back(title);

		//Play
		CheckBox* active = new CheckBox(false, "Play", main_font, 12, x + 5, y + 30, 30, 30);
		active->property.bind<bool>(looper.channels[i].play, handler);
		controls.push_back(active);
		//Bars
		DragBox<int>* bars = new DragBox<int>(4, 1, 16, main_font, 12, x + 5, y + 65, pane_width - 15, 30);
		bars->property.bind<unsigned int>(looper.channels[i].preset.bars, handler);
		controls.push_back(bars);
		//Record
		Button* record = new Button("Record", main_font, 12, x + 5, y + 100, pane_width - 15, 30);
		record->set_on_click([this, &lock, i]() {
			if (record_index == (ssize_t) i) {
				record_index = -1;
			}
			else {
				record_index = i;
			}
			lock.lock();
			looper.record_channel = record_index;
			lock.unlock();
			update_record();
		});
		controls.push_back(record);
		records[i] = record;
		//Solo
		Button* solo = new Button("Solo", main_font, 12, x + 5, y + 135, pane_width - 15, 30);
		solo->set_on_click([this, &lock, i ]() {
			if (solo_index == (ssize_t) i) {
				solo_index = -1;
			}
			else {
				solo_index = i;
			}
			lock.lock();
			looper.record_channel = record_index;
			lock.unlock();
			update_solo();
		});
		controls.push_back(solo);
		solos[i] = solo;
		//Clear
		Button* clear = new Button("Clear", main_font, 12, x + 5, y + 170, pane_width - 15, 30);
		clear->set_on_click([this, &lock, i]() {
			lock.lock();
			looper.channels[i].reset = true;
			lock.unlock();
		});
		controls.push_back(clear);
	}

	//Looper
	CheckBox* looper = new CheckBox(false, "Looper", main_font, 18, 10, frame.get_height() - 45, 40, 40);
	looper->property.bind(this->looper.active, handler);
	controls.push_back(looper);

	return {controls};
}

void LooperView::update_properties() {
	SpinLock& lock = looper.get_lock();
	lock.lock();
	record_index = looper.record_channel;
	solo_index = looper.solo_channel;
	lock.lock();
	update_record();
	update_solo();
}
