/*
 * SceneView.cpp
 *
 *  Created on: Feb 28, 2021
 *      Author: jojo
 */

#include "SceneView.h"
#include "SoundEngineView.h"

SceneView::SceneView() {

}

void SceneView::property_change(PropertyChange change) {
}

Scene SceneView::create(Frame &frame) {
	std::vector<Control*> controls;
	std::vector<PropertyHolder*> holders;

	//Background
	Pane* bg = new Pane(sf::Color(80, 80, 80), 0, 0, frame.get_width(), frame.get_height());
	controls.push_back(bg);

	this->engine = &frame.cube.engine;

	//Scenes
	int rows = 2;
	int cols = SOUND_ENGINE_SCENE_AMOUNT/ rows;
	int pane_width = (frame.get_width() - 15) / cols;
	int pane_height = (frame.get_height() - 50 - 5) / rows;
	std::array<Button*, SOUND_ENGINE_SCENE_AMOUNT> scenes;
	for (size_t i = 0; i < SOUND_ENGINE_SCENE_AMOUNT; ++i) {
		//Background pane
		int x = 10 + pane_width * (i % cols);
		int y = 10 + pane_height * (i / cols);
		//Engine
		Button* scene = new Button("Scene " + std::to_string(i + 1), main_font, 12, x, y,  pane_width - 5, pane_height - 5);
		if (this->engine->scene == i) {
			scene->rect.setFillColor(sf::Color(0, 180, 255));
		}
		else {
			scene->rect.setFillColor(sf::Color(200, 200, 200));
		}
		scene->set_on_click([&frame, scenes, this, i]() {
			//Change scene
			scenes[this->engine->scene]->rect.setFillColor(sf::Color(200, 200, 200));
			this->engine->scene = i;
			scenes[i]->rect.setFillColor(sf::Color(0, 180, 255));
		});

		scenes[i] = scene;

		controls.push_back(scene);
	}

	//Exit Button
	Button* exit = new Button("Exit", main_font, 32, frame.get_width() - 70, frame.get_height() - 40, 70, 40);
	exit->set_on_click([&frame]() {
		frame.change_view(new SoundEngineView());
	});
	exit->rect.setFillColor(sf::Color::Yellow);
	controls.push_back(exit);


	return {controls, holders};
}


SceneView::~SceneView() {

}
