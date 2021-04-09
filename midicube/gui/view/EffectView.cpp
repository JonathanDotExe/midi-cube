/*
 * EffectView.cpp
 *
 *  Created on: Apr 9, 2021
 *      Author: jojo
 */

#include "EffectView.h"
#include "resources.h"

#include "../../effect/amplifier_simulation.h"
#include "../../effect/bitcrusher.h"
#include "../../effect/chorus.h"
#include "../../effect/rotary_speaker.h"
#include "../../effect/vocoder.h"

EffectView::EffectView(Effect* e) : effect(e) {

}

EffectView::~EffectView() {

}

Scene EffectView::create(Frame &frame) {
	std::vector<Control*> controls;

	ActionHandler& handler = frame.cube.action_handler;

	//Background
	Pane* bg = new Pane(sf::Color(80, 80, 80), 0, 0, frame.get_width(), frame.get_height());
	controls.push_back(bg);

	//Title
	Label* title = new Label("Effect", main_font, 48, 10, 10);
	controls.push_back(title);

	int tmp_x = 10;
	int tmp_y = 70;

	//Effects
	//Amplifier
	if (dynamic_cast<AmplifierSimulationEffect*>(effect) != nullptr) {
		AmplifierSimulationEffect* amp = dynamic_cast<AmplifierSimulationEffect*>(effect);

		//Amplifier
		{
			Label* label = new Label("On", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			OrganSwitch* amplifier = new OrganSwitch(false, main_font, tmp_x, tmp_y, 180, 120);
			amplifier->property.bind(amp->preset.on, handler);
			controls.push_back(amplifier);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Amplifier Boost
		{
			Label* label = new Label("Boost", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);

			DragBox<double>* boost = new DragBox<double>(0, 0, 1, main_font, 16, tmp_x, tmp_y, 180, 120);
			boost->property.bind(amp->preset.boost, handler);
			controls.push_back(boost);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Distortion Amount
		{
			Label* overdrive_label = new Label("Drive", main_font, 18, tmp_x, tmp_y);
			overdrive_label->text.setFillColor(sf::Color::White);
			controls.push_back(overdrive_label);
			tmp_y += 25;

			DragBox<double>* overdrive = new DragBox<double>(0, 0, 1, main_font, 16, tmp_x, tmp_y, 180, 120);
			overdrive->property.bind(amp->preset.drive, handler);
			controls.push_back(overdrive);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Distortion Type
		{
			Label* distortion_type_label = new Label("Distortion Type", main_font, 18, tmp_x, tmp_y);
			distortion_type_label->text.setFillColor(sf::Color::White);
			controls.push_back(distortion_type_label);
			tmp_y += 25;

			ComboBox* distortion_type = new ComboBox(0, {"Digital", "Polynomal", "Arctan"}, main_font, 16, 0, tmp_x, tmp_y, 180, 120);
			distortion_type->property.bind(amp->preset.type, handler);
			controls.push_back(distortion_type);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Amp Tone
		{
			Label* label = new Label("Tone", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double>* amp_tone = new DragBox<double>(0, 0, 1, main_font, 16, tmp_x, tmp_y, 80, 60);
			amp_tone->property.bind(amp->preset.tone, handler);
			controls.push_back(amp_tone);
			tmp_y -= 25;
		}
	}

	//Exit Button
	Button* exit = new Button("Exit", main_font, 18, frame.get_width() - 70, frame.get_height() - 40, 70, 40);
	exit->set_on_click([&frame]() {
		frame.close();
	});
	exit->rect.setFillColor(sf::Color::Yellow);
	controls.push_back(exit);


	return {controls};
}
