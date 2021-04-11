/*
 * EffectView.cpp
 *
 *  Created on: Apr 9, 2021
 *      Author: jojo
 */

#include "EffectView.h"
#include "SoundEngineView.h"
#include "resources.h"

#include "../../effect/amplifier_simulation.h"
#include "../../effect/bitcrusher.h"
#include "../../effect/chorus.h"
#include "../../effect/rotary_speaker.h"
#include "../../effect/vocoder.h"

EffectView::EffectView(Effect *e) :
		effect(e) {

}

EffectView::~EffectView() {

}

Scene EffectView::create(Frame &frame) {
	std::vector<Control*> controls;

	ActionHandler &handler = frame.cube.action_handler;

	//Background
	Pane *bg = new Pane(sf::Color(80, 80, 80), 0, 0, frame.get_width(),
			frame.get_height());
	controls.push_back(bg);

	//Title
	Label *title = new Label("Effect", main_font, 72, 10, 10);
	title->text.setFillColor(sf::Color::White);
	controls.push_back(title);

	int tmp_x = 10;
	int tmp_y = 120;

	//Effects
	//Amplifier
	if (dynamic_cast<AmplifierSimulationEffect*>(effect) != nullptr) {
		AmplifierSimulationEffect *amp =
				dynamic_cast<AmplifierSimulationEffect*>(effect);

		//Background
		bg->rect.setFillColor(sf::Color(0x53, 0x32, 0x00));
		title->text.setString("Super Tube M44");

		//Amplifier
		{
			Label *label = new Label("On", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			OrganSwitch *amplifier = new OrganSwitch(false, main_font, tmp_x,
					tmp_y, 180, 120);
			amplifier->property.bind(amp->preset.on, handler);
			controls.push_back(amplifier);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Amplifier Boost
		{
			Label *label = new Label("Boost", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *boost = new DragBox<double>(0, 0, 1, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			boost->property.bind(amp->preset.boost, handler);
			controls.push_back(boost);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Distortion Amount
		{
			Label *overdrive_label = new Label("Drive", main_font, 18, tmp_x,
					tmp_y);
			overdrive_label->text.setFillColor(sf::Color::White);
			controls.push_back(overdrive_label);
			tmp_y += 25;

			DragBox<double> *overdrive = new DragBox<double>(0, 0, 1, main_font,
					24, tmp_x, tmp_y, 180, 120);
			overdrive->property.bind(amp->preset.drive, handler);
			controls.push_back(overdrive);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Distortion Type
		{
			Label *distortion_type_label = new Label("Distortion Type",
					main_font, 18, tmp_x, tmp_y);
			distortion_type_label->text.setFillColor(sf::Color::White);
			controls.push_back(distortion_type_label);
			tmp_y += 25;

			ComboBox *distortion_type = new ComboBox(0, { "Digital",
					"Polynomal", "Arctan" }, main_font, 24, 0, tmp_x, tmp_y,
					180, 120);
			distortion_type->property.bind(amp->preset.type, handler);
			controls.push_back(distortion_type);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Amp Tone
		{
			Label *label = new Label("Tone", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *amp_tone = new DragBox<double>(0, 0, 1, main_font,
					24, tmp_x, tmp_y, 180, 120);
			amp_tone->property.bind(amp->preset.tone, handler);
			controls.push_back(amp_tone);
			tmp_y -= 25;
		}
	}
	//Rotary Speaker
	else if (dynamic_cast<RotarySpeakerEffect*>(effect) != nullptr) {
		RotarySpeakerEffect *rotary = dynamic_cast<RotarySpeakerEffect*>(effect);

		//Background
		bg->rect.setFillColor(sf::Color(0x53, 0x32, 0x00));
		title->text.setString("Vintage Rotary L120");

		//Rotary
		{
			Label *label = new Label("Mode", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			OrganSwitch *on = new OrganSwitch(false, main_font, tmp_x, tmp_y,
					80, 60, "Rotate", "Stop");
			on->property.bind(rotary->preset.on, handler);
			controls.push_back(on);

			tmp_y -= 25;
			tmp_x += 90;
		}
		//Rotary Speed
		{
			Label *label = new Label("Speed", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			OrganSwitch *rotary_speed = new OrganSwitch(false, main_font, tmp_x,
					tmp_y, 80, 60, "Fast", "Slow");
			rotary_speed->property.bind(rotary->preset.fast, handler);
			controls.push_back(rotary_speed);

			tmp_y -= 25;
			tmp_x += 90;
		}

		//Rotary Stereo Mix
		{
			Label *label = new Label("Stereo", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *rotary_stereo = new DragBox<double>(0, 0, 1,
					main_font, 16, tmp_x, tmp_y, 80, 60);
			rotary_stereo->property.bind(rotary->preset.stereo_mix, handler);
			controls.push_back(rotary_stereo);

			tmp_y -= 25;
			tmp_x += 90;
		}

		//Rotary Type
		{
			Label *label = new Label("Type", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			OrganSwitch *rotary_type = new OrganSwitch(false, main_font, tmp_x,
					tmp_y, 80, 60, "1", "2");
			rotary_type->property.bind(rotary->preset.type, handler);
			controls.push_back(rotary_type);

			tmp_y -= 25;
			tmp_x += 90;
		}
	}

	//Chorus
	else if (dynamic_cast<ChorusEffect*>(effect) != nullptr) {
		ChorusEffect* chorus = dynamic_cast<ChorusEffect*>(effect);

		//Background
		bg->rect.setFillColor(sf::Color(0, 180, 255));
		title->text.setString("Mega Chorus MC-5");

		//Chorus
		{
			Label *label = new Label("On", main_font, 18, tmp_x, tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			OrganSwitch *on = new OrganSwitch(false, main_font, tmp_x,
					tmp_y, 180, 120);
			on->property.bind(chorus->preset.on, handler);
			controls.push_back(on);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Vibrato Rate
		{
			Label *label = new Label("Vibrato Rate", main_font, 18, tmp_x, tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *rate = new DragBox<double>(0, 0, 8, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			rate->property.bind(chorus->preset.vibrato_rate, handler);
			controls.push_back(rate);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Vibrato Depth
		{
			Label *label = new Label("Vibrato Depth", main_font, 18, tmp_x,
					tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *depth = new DragBox<double>(0, 0, 1, main_font,
					24, tmp_x, tmp_y, 180, 120);
			depth->property.bind(chorus->preset.vibrato_depth, handler);
			controls.push_back(depth);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Mix
		{
			Label *label = new Label("Mix", main_font, 18, tmp_x,
				tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *mix = new DragBox<double>(0, 0, 1, main_font,
				24, tmp_x, tmp_y, 180, 120);
			mix->property.bind(chorus->preset.mix, handler);
			controls.push_back(mix);

			tmp_y -= 25;
			tmp_x += 200;
		}
		tmp_x = 10;
		tmp_y += 160;

		//Delay
		{
			Label *label = new Label("Delay", main_font, 18, tmp_x,
				tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0.0, 0.03, main_font,
				24, tmp_x, tmp_y, 180, 120);
			value->property.bind(chorus->preset.delay, handler);
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}
		//Waveform
		{
			Label *label = new Label("Waveform", main_font, 18, tmp_x,
				tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			std::vector<std::string> waveforms = {"Sine", "Saw Down", "Saw Up", "Square", "Triangle"};

			ComboBox* waveform = new ComboBox(4, waveforms, main_font, 24, 0, tmp_x , tmp_y, 180, 120);
			waveform->property.bind(chorus->preset.vibrato_waveform, handler);
			controls.push_back(waveform);

			tmp_y -= 25;
			tmp_x += 200;
		}
	}

	//Exit Button
	Button *exit = new Button("Back", main_font, 18, frame.get_width() - 70,
			frame.get_height() - 40, 70, 40);
	exit->set_on_click([&frame]() {
		frame.change_view(new SoundEngineView());
	});
	exit->rect.setFillColor(sf::Color::Yellow);
	controls.push_back(exit);

	return {controls};
}
