/*
 * LooperView.cpp
 *
 *  Created on: Apr 14, 2021
 *      Author: jojo
 */

#include "../view/LooperView.h"

#include "../view/resources.h"
#include "../view/SoundEngineView.h"

LooperView::LooperView() {

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

Scene LooperView::create(Frame &frame) {
	std::vector<Control*> controls;

	ActionHandler& handler = frame.cube.action_handler;

	//Background
	Pane* bg = new Pane(sf::Color(80, 80, 80), 0, 0, frame.get_width(), frame.get_height());
	controls.push_back(bg);

	SoundEngineDevice& sound_engine = frame.cube.engine;

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
		active->property.bind<bool>(sound_engine.looper.channels[i].play, handler);
		controls.push_back(active);
		//Bars
		DragBox<int>* bars = new DragBox<int>(4, 1, 16, main_font, 12, x + 5, y + 65, pane_width - 15, 30);
		bars->property.bind<unsigned int>(sound_engine.looper.channels[i].preset.bars, handler);
		controls.push_back(bars);
		//Record
		Button* record = new Button("Record", main_font, 12, x + 5, y + 100, pane_width - 15, 30);
		record->set_on_click([this, &handler, i , &sound_engine]() {
			if (record_index == (ssize_t) i) {
				record_index = -1;
			}
			else {
				record_index = i;
			}
			handler.queue_action(new SetValueAction<ssize_t, ssize_t>(sound_engine.looper.record_channel, record_index));
			update_record();
		});
		controls.push_back(record);
		records[i] = record;
		//Solo
		Button* solo = new Button("Solo", main_font, 12, x + 5, y + 135, pane_width - 15, 30);
		solo->set_on_click([this, &handler, i , &sound_engine]() {
			if (solo_index == (ssize_t) i) {
				solo_index = -1;
			}
			else {
				solo_index = i;
			}
			handler.queue_action(new SetValueAction<ssize_t, ssize_t>(sound_engine.looper.solo_channel, solo_index));
			update_solo();
		});
		controls.push_back(solo);
		solos[i] = solo;
		//Clear
		Button* clear = new Button("Clear", main_font, 12, x + 5, y + 170, pane_width - 15, 30);
		clear->set_on_click([this, &handler, i , &sound_engine]() {
			handler.queue_action(new SetValueAction<bool, bool>(sound_engine.looper.channels[i].reset, true));
		});
		controls.push_back(clear);
	}

	//Metronome
	CheckBox* metronome = new CheckBox(false, "Metronome", main_font, 18, 10, frame.get_height() - 45, 40, 40);
	metronome->property.bind(sound_engine.play_metronome, handler);
	controls.push_back(metronome);

	DragBox<int>* bpm = new DragBox<int>(120, 10, 480, main_font, 18, 200, frame.get_height() - 45, 100, 40);
	bpm->drag_mul = 0.00125;
	bpm->property.bind_function<unsigned int>(std::bind(&Metronome::get_bpm, &sound_engine.metronome), std::bind(&Metronome::set_bpm, &sound_engine.metronome, std::placeholders::_1), handler);
	controls.push_back(bpm);

	//Volume
	DragBox<double>* volume = new DragBox<double>(0, 0, 1, main_font, 18, 330, frame.get_height() - 45, 100, 40);
	volume->property.bind(sound_engine.volume, handler);
	controls.push_back(volume);

	//Looper
	CheckBox* looper = new CheckBox(false, "Looper", main_font, 18, 440, frame.get_height() - 45, 40, 40);
	looper->property.bind(sound_engine.looper.active, handler);
	controls.push_back(looper);

	//Exit Button
	Button* exit = new Button("Exit", main_font, 18, frame.get_width() - 70, frame.get_height() - 40, 70, 40);
	exit->set_on_click([&frame]() {
		frame.change_view(new SoundEngineView());
	});
	exit->rect.setFillColor(sf::Color::Yellow);
	controls.push_back(exit);

	this->handler = &handler;
	this->engine = &sound_engine;

	return {controls};
}

void LooperView::update_properties() {
	handler->queue_action(new GetValueAction<ssize_t, ssize_t>(engine->looper.record_channel, [this](ssize_t v) {
		record_index = v;
		update_record();
	}));
	handler->queue_action(new GetValueAction<ssize_t, ssize_t>(engine->looper.solo_channel, [this](ssize_t v) {
		solo_index = v;
		update_solo();
	}));
}
