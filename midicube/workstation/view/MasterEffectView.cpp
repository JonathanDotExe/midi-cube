/*
 * MasterEffectView.cpp
 *
 *  Created on: Apr 11, 2021
 *      Author: jojo
 */

#include "MasterEffectView.h"
#include "SoundEngineView.h"
#include "PluginSelectView.h"

MasterEffectView::MasterEffectView(MidiCubeWorkstation& c): cube(c) {


}

MasterEffectView::~MasterEffectView() {

}

Scene MasterEffectView::create(ViewHost &frame) {
	std::vector<Control*> controls;

	//Background
	Pane* bg = new Pane(sf::Color(80, 80, 80), 0, 0, frame.get_width(), frame.get_height());
	controls.push_back(bg);

	SoundEngineDevice& sound_engine = cube.engine;

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

		//Title
		Label* title = new Label("Effect " + std::to_string(i), main_font, 16, x + 5, y + 5);
		controls.push_back(title);

		//Effect
		//TODO Effect selector
		/*ComboBox* e = new ComboBox(0, effect_names, main_font, 12, -1, x + 5, y + 30, pane_width - 15, 30);
		e->rect.setFillColor(sf::Color(128, 255, 255));
		e->property.bind_function<ssize_t>(std::bind(&MasterEffect::get_effect_index, &effect), std::bind(&MasterEffect::set_effect_index, &effect, std::placeholders::_1), frame.cube.lock);
		controls.push_back(e);*/
		cube.lock.lock();
		std::string effect_name = effect.effect.get_plugin() ? effect.effect.get_plugin()->get_plugin().info.name : "None";
		cube.lock.unlock();
		Button* e = new Button(effect_name, main_font, 12, x + 5, y + 30, pane_width - 15, 30);
		e->rect.setFillColor(sf::Color(128, 255, 255));
		e->set_on_click([this, &frame, &effect]() {
			MidiCubeWorkstation& c = cube;
			frame.change_menu(VIEW_MENU(new PluginSelectView(effect.effect, c.plugin_mgr.get_plugins(PluginType::PLUGIN_TYPE_EFFECT), c.lock, c.plugin_mgr, &c.clipboard), &c, &effect));
		});
		controls.push_back(e);
		//Edit
		Button* edit_effect = new Button("Edit", main_font, 12, x + 5, y + 65, pane_width - 15, 30);
		edit_effect->set_on_click([this, i, &effect, &frame]() {
			cube.lock.lock();
			PluginInstance* eff = effect.effect.get_plugin();
			if (eff) {
				frame.change_menu(eff->create_menu());
			}
			cube.lock.unlock();
		});
		controls.push_back(edit_effect);

		//Master Effect Send
		{
			Label* octave_label = new Label("Master Send", main_font, 12, x + 5, y + 105);
			controls.push_back(octave_label);

			DragBox<int>* master_send = new DragBox<int>(0, -1, SOUND_ENGINE_MASTER_EFFECT_AMOUNT - 1, main_font, 12, x + 5, y + 125, pane_width - 15, 30);
			master_send->property.bind(effect.next_effect, cube.lock);
			controls.push_back(master_send);
		}
	}

	//Back Button
	Button* exit = new Button("Back", main_font, 18, frame.get_width() - 70, frame.get_height() - 40, 70, 40);
	exit->set_on_click([&frame, this]() {
		frame.menu_back();
	});
	exit->rect.setFillColor(sf::Color::Yellow);
	controls.push_back(exit);


	return {controls};
}
