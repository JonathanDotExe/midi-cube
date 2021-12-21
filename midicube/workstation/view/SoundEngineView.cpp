/*
 * SoundEngineView.cpp
 *
 *  Created on: Dec 19, 2020
 *      Author: jojo
 */

#include "../view/SoundEngineView.h"

#include <iostream>
#include "../view/MasterEffectView.h"
#include "../view/ProgramView.h"
#include "../view/SceneView.h"
#include "../view/SoundEngineChannelView.h"
#include "../view/SourceView.h"

SoundEngineView::SoundEngineView(MidiCubeWorkstation& c) : ViewController(), cube(c) {

}

Scene SoundEngineView::create(ViewHost& frame) {
	std::vector<Control*> controls;
	ActionHandler& handler = frame.get_action_handler();

	//Background
	Pane* bg = new Pane(sf::Color(80, 80, 80), 0, 0, frame.get_width(), frame.get_height());
	controls.push_back(bg);

	this->engine = &cube.engine;

	SoundEngineDevice& sound_engine = cube.engine;
	//Sound engines

	//Channels
	int rows = 2;
	int cols = SOUND_ENGINE_MIDI_CHANNELS / rows;
	int pane_width = (frame.get_width() - 15) / cols;
	int pane_height = (frame.get_height() - 50 - 5) / rows;
	for (size_t i = 0; i < SOUND_ENGINE_MIDI_CHANNELS; ++i) {
		SoundEngineChannel& channel = sound_engine.channels[i];
		//Background pane
		int x = 10 + pane_width * (i % cols);
		int y = 10 + pane_height * (i / cols);
		Pane* pane = new Pane(sf::Color(120, 120, 120), x, y, pane_width - 5, pane_height - 5);

		controls.push_back(pane);

		//Title
		Label* title = new Label("Channel " + std::to_string(i + 1), main_font, 16, x + 5, y + 5);
		controls.push_back(title);

		//Active
		CheckBox* active = new CheckBox(false, "", main_font, 12, x + pane_width - 30, y + 5, 20, 20);
		active->property.bind_function<bool>(std::bind(&SoundEngineChannel::is_active, &channel), std::bind(&SoundEngineChannel::set_active, &channel, std::placeholders::_1), handler);
		controls.push_back(active);
		//Engine
		cube.lock.lock();
		std::string engine_name = channel.engine.get_plugin() ? channel.engine.get_plugin()->get_plugin().info.name : "None";
		cube.lock.unlock();

		Button* engine = new Button(engine_name, main_font, 12, x + 5, y + 30,  pane_width - 15, 30);
		engine->rect.setFillColor(sf::Color(0, 180, 255));
		controls.push_back(engine);
		engine_buttons[i] = engine;
		engine->set_on_click([this, &channel, i, &frame]() {
			MidiCubeWorkstation& cube = this->cube;
			frame.change_menu(VIEW_MENU(new SoundEngineChannelView(cube, channel, i), &cube, &channel, i));
		});

		//Volume
		Slider* volume = new Slider(0, 0, 1, main_font, x + (pane_width - 5)/2 - 20, y + 70, 40, 180);
		volume->property.bind(channel.volume, handler);
		controls.push_back(volume);
	}

	//Metronome
	CheckBox* metronome = new CheckBox(false, "Metronome", main_font, 18, 10, frame.get_height() - 45, 40, 40);
	metronome->property.bind(engine->play_metronome, handler);
	controls.push_back(metronome);

	DragBox<int>* bpm = new DragBox<int>(120, 10, 480, main_font, 18, 160, frame.get_height() - 45, 100, 40);
	bpm->drag_mul = 0.00125;
	bpm->property.bind_function<unsigned int>(std::bind(&Metronome::get_bpm, &engine->metronome), std::bind(&Metronome::set_bpm, &engine->metronome, std::placeholders::_1), handler);
	controls.push_back(bpm);

	//Volume
	DragBox<double>* volume = new DragBox<double>(0, 0, 1, main_font, 18, 270, frame.get_height() - 45, 100, 40);
	volume->property.bind(engine->volume, handler);
	controls.push_back(volume);

	//Effects
	Button* effects = new Button("Effects", main_font, 18, 370, frame.get_height() - 45, 100, 40);
	effects->set_on_click([&frame, this]() {
		MidiCubeWorkstation& cube = this->cube;
		frame.change_menu(VIEW_MENU(new MasterEffectView(cube), &cube));
	});
	controls.push_back(effects);

	//Copy
	Button* copy = new Button("Copy", main_font, 18, 470, frame.get_height() - 45, 100, 40);
	copy->set_on_click([&frame, this]() {
		cube.lock.lock();
		cube.copy_program();
		cube.lock.unlock();
	});
	controls.push_back(copy);

	//Paste
	Button* paste = new Button("Paste", main_font, 18, 570, frame.get_height() - 45, 100, 40);
	paste->set_on_click([&frame, this]() {
		cube.lock.lock();
		bool pasted = cube.paste_program();
		cube.lock.unlock();
		if (pasted) {
			frame.change_view(new SoundEngineView(cube)); //FIXME proper refresh
		}
	});
	controls.push_back(paste);

	//Sources
	Button* sources = new Button("Sources", main_font, 18, frame.get_width() - 375, frame.get_height() - 45, 100, 40);
	sources->set_on_click([&frame, this]() {
		MidiCubeWorkstation& cube = this->cube;
		frame.change_menu(VIEW_MENU(new SourceView(cube), &cube));
	});
	controls.push_back(sources);

	//Scene
	Button* scene = new Button("Scenes", main_font, 18, frame.get_width() - 275, frame.get_height() - 45, 100, 40);
	scene->set_on_click([&frame, this]() {
		MidiCubeWorkstation& cube = this->cube;
		frame.change_menu(VIEW_MENU(new SceneView(&cube.engine), &cube));
	});
	controls.push_back(scene);

	//Program Button
	Button* program = new Button("Programs", main_font, 18, frame.get_width() - 175, frame.get_height() - 45, 100, 40);
	program->set_on_click([&frame, this]() {
		cube.prog_mgr.lock();
		size_t bank = cube.prog_mgr.get_curr_bank_index();
		size_t page = cube.prog_mgr.get_curr_program_index()/(PROGRAM_VIEW_ROWS * PROGRAM_VIEW_COLS);
		cube.prog_mgr.unlock();
		MidiCubeWorkstation& cube = this->cube;
		frame.change_menu(VIEW_MENU(new ProgramView(cube, bank, page), &cube, bank, page));
	});
	controls.push_back(program);

	//Exit Button
	Button* exit = new Button("Exit", main_font, 18, frame.get_width() - 75, frame.get_height() - 45, 70, 40);
	exit->set_on_click([&frame]() {
		frame.close();
	});
	exit->rect.setFillColor(sf::Color::Yellow);
	controls.push_back(exit);


	return {controls};
}

SoundEngineView::~SoundEngineView() {

}
