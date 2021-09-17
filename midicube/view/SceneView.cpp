/*
 * SceneView.cpp
 *
 *  Created on: Feb 28, 2021
 *      Author: jojo
 */

#include "../view/SceneView.h"

#include "../view/SoundEngineView.h"

SceneView::SceneView() {

}

Scene SceneView::create(Frame &frame) {
	std::vector<Control*> controls;

	//Background
	Pane* bg = new Pane(sf::Color(80, 80, 80), 0, 0, frame.get_width(), frame.get_height());
	controls.push_back(bg);

	this->engine = &frame.cube.engine;
	this->handler = &frame.cube.action_handler;

	//Scenes
	int rows = 2;
	int cols = SOUND_ENGINE_SCENE_AMOUNT/ rows;
	int pane_width = (frame.get_width() - 15) / cols;
	int pane_height = (frame.get_height() - 50 - 5) / rows;
	for (size_t i = 0; i < SOUND_ENGINE_SCENE_AMOUNT; ++i) {
		//Background pane
		int x = 10 + pane_width * (i % cols);
		int y = 10 + pane_height * (i / cols);
		//Engine
		Button* scene = new Button("Scene " + std::to_string(i + 1), main_font, 32, x, y,  pane_width - 5, pane_height - 5);
		scene->set_on_click([&frame, this, i]() {
			//Change scene
			handler->queue_action(new SetValueAction<size_t, size_t>(this->engine->scene, i));
			this->update_properties();
		});

		scenes[i] = scene;

		controls.push_back(scene);
	}

	//Exit Button
	Button* exit = new Button("Exit", main_font, 18, frame.get_width() - 70, frame.get_height() - 40, 70, 40);
	exit->set_on_click([&frame]() {
		frame.change_view(new SoundEngineView());
	});
	exit->rect.setFillColor(sf::Color::Yellow);
	controls.push_back(exit);


	return {controls};
}


SceneView::~SceneView() {

}

void SceneView::update_properties() {
	handler->queue_action(new GetValueAction<size_t, size_t>(this->engine->scene, [this](size_t s) {
		for (size_t i = 0; i < SOUND_ENGINE_SCENE_AMOUNT; ++i) {
			if (s == i) {
				scenes[i]->rect.setFillColor(sf::Color(0, 180, 255));
			}
			else {
				scenes[i]->rect.setFillColor(sf::Color(200, 200, 200));
			}
		}
	}));
}
