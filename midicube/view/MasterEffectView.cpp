/*
 * MasterEffectView.cpp
 *
 *  Created on: Apr 11, 2021
 *      Author: jojo
 */

#include "../view/MasterEffectView.h"

#include "../view/EffectView.h"
#include "../view/SoundEngineView.h"

MasterEffectView::MasterEffectView() {
	// TODO Auto-generated constructor stub

}

MasterEffectView::~MasterEffectView() {
	// TODO Auto-generated destructor stub
}

Scene MasterEffectView::create(Frame &frame) {
	std::vector<Control*> controls;

	ActionHandler& handler = frame.cube.action_handler;

	//Background
	Pane* bg = new Pane(sf::Color(80, 80, 80), 0, 0, frame.get_width(), frame.get_height());
	controls.push_back(bg);

	SoundEngineDevice& sound_engine = frame.cube.engine;

	//Channels
	int rows = 2;
	int cols = SOUND_ENGINE_MASTER_EFFECT_AMOUNT / rows;
	int pane_width = (frame.get_width() - 15) / cols;
	int pane_height = (frame.get_height() - 50 - 5) / rows;
	for (size_t i = 0; i < SOUND_ENGINE_MASTER_EFFECT_AMOUNT; ++i) {
		MasterEffect& effect = sound_engine.effects[i];
		//Background pane
		int x = 10 + pane_width * (i % cols);
		int y = 10 + pane_height * (i / cols);
		Pane* pane = new Pane(sf::Color(120, 120, 120), x, y, pane_width - 5, pane_height - 5);

		controls.push_back(pane);

		//Effects
		std::vector<std::string> effect_names;
		effect_names.push_back("None");
		for (EffectBuilder* effect : frame.cube.engine.get_effect_builders()) {
			effect_names.push_back(effect->get_name());
		}

		//Title
		Label* title = new Label("Effect " + std::to_string(i), main_font, 16, x + 5, y + 5);
		controls.push_back(title);

		//Effect
		ComboBox* e = new ComboBox(0, effect_names, main_font, 12, -1, x + 5, y + 30, pane_width - 15, 30);
		e->rect.setFillColor(sf::Color(128, 255, 255));
		e->property.bind_function<ssize_t>(std::bind(&MasterEffect::get_effect_index, &effect), std::bind(&MasterEffect::set_effect_index, &effect, std::placeholders::_1), frame.cube.lock);
		controls.push_back(e);
		//Edit
		Button* edit_effect = new Button("Edit", main_font, 12, x + 5, y + 65, pane_width - 15, 30);
		edit_effect->set_on_click([this, &frame, i, &effect]() {
			frame.cube.lock.lock();
			Effect* eff = effect.get_effect();
			if (eff) {
				frame.change_view(new EffectView(eff, []() { return new MasterEffectView(); }));
			}
			frame.cube.lock.unlock();
		});
		controls.push_back(edit_effect);

		//Master Effect Send
		{
			Label* octave_label = new Label("Master Send", main_font, 12, x + 5, y + 105);
			controls.push_back(octave_label);

			DragBox<int>* master_send = new DragBox<int>(0, -1, SOUND_ENGINE_MASTER_EFFECT_AMOUNT - 1, main_font, 12, x + 5, y + 125, pane_width - 15, 30);
			master_send->property.bind(effect.next_effect, handler);
			controls.push_back(master_send);
		}
	}

	//Back Button
	Button* exit = new Button("Back", main_font, 18, frame.get_width() - 70, frame.get_height() - 40, 70, 40);
	exit->set_on_click([&frame, this]() {
		frame.change_view(new SoundEngineView());
	});
	exit->rect.setFillColor(sf::Color::Yellow);
	controls.push_back(exit);


	return {controls};
}
