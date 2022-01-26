/*
 * EffectView.cpp
 *
 *  Created on: Apr 9, 2021
 *      Author: jojo
 */

#include "../view/EffectView.h"

#include "../effect/amplifier_simulation.h"
#include "../effect/bitcrusher.h"
#include "../effect/chorus.h"
#include "../effect/compressor.h"
#include "../effect/delay.h"
#include "../effect/equalizer.h"
#include "../effect/flanger.h"
#include "../effect/phaser.h"
#include "../effect/reverb.h"
#include "../effect/rotary_speaker.h"
#include "../effect/tremolo.h"
#include "../effect/vocoder.h"
#include "../effect/wahwah.h"
#include "../../plugins/resources.h"

EffectView::EffectView(PluginInstance* e) :
		effect(e), binder{main_font} {

}

EffectView::~EffectView() {

}

Scene EffectView::create(ViewHost &frame) {
	ActionHandler& handler = frame.get_action_handler();
	SpinLock& lock = effect->get_lock();
	lock.lock();
	std::vector<Control*> controls;

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
			boost->property.bind(amp->preset.post_gain, handler);
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
					"Polynomal", "Arctan", "Cubic", "Fuzz", "Soft Clip", "Tanh", "Sigmoid"}, main_font, 24, 0, tmp_x, tmp_y,
					180, 120);
			distortion_type->property.bind_cast(amp->preset.type, handler);
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
			tmp_x += 200;
		}

		tmp_y += 160;
		tmp_x -= 200 * 5;

		//Low Gain
		{
			Label *label = new Label("Low Gain", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, -1, 5, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			value->border = 0;
			value->property.bind(amp->preset.low_gain, handler);
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Mid Gain
		{
			Label *label = new Label("Mid Gain", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, -1, 5, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			value->border = 0;
			value->property.bind(amp->preset.mid_gain, handler);
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//High Gain
		{
			Label *label = new Label("High Gain", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, -1, 5, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			value->border = 0;
			value->property.bind(amp->preset.high_gain, handler);
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Lowshelf Boost
		{
			Label *label = new Label("Lowshelf Boost", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0, 3, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			value->border = 0;
			value->property.bind(amp->preset.lowshelf_boost, handler);
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Mix
		{
			Label *label = new Label("Mix", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *amp_mix = new DragBox<double>(0, 0, 1, main_font,
					24, tmp_x, tmp_y, 180, 120);
			amp_mix->property.bind(amp->preset.mix, handler);
			controls.push_back(amp_mix);

			tmp_y -= 25;
			tmp_x += 200;
		}

		tmp_y += 160;
		tmp_x -= 200 * 5;

		//Low Freq
		{
			Label *label = new Label("Low Freq", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 20, 400, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			value->property.bind(amp->preset.low_freq, handler);
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Mid Freq
		{
			Label *label = new Label("Mid Freq", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 200, 8000, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			value->property.bind(amp->preset.mid_freq, handler);
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//High Freq
		{
			Label *label = new Label("High Freq", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 1000, 20000, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			value->property.bind(amp->preset.high_freq, handler);
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Lowshelf Cutoff
		{
			Label *label = new Label("Lowshelf Cutoff", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 15, 160, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			value->border = 0;
			value->property.bind(amp->preset.lowshelf_cutoff, handler);
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
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
			Label *label = new Label("On", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			OrganSwitch *on = new OrganSwitch(false, main_font, tmp_x, tmp_y,
					80, 60, "On", "Off");
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

		//Rotary Stop
		{
			Label *label = new Label("Stop", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			OrganSwitch *rotary_speed = new OrganSwitch(false, main_font, tmp_x,
					tmp_y, 80, 60, "Stop", "Rotate");
			rotary_speed->property.bind(rotary->preset.stop, handler);
			controls.push_back(rotary_speed);

			tmp_y -= 25;
			tmp_x += 90;
		}

		//Drive
		{
			Label *label = new Label("Drive", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *rotary_stereo = new DragBox<double>(0, 0, 1,
					main_font, 16, tmp_x, tmp_y, 80, 60);
			rotary_stereo->property.bind(rotary->preset.drive, handler);
			controls.push_back(rotary_stereo);

			tmp_y -= 25;
			tmp_x += 90;
		}

		//Tone
		{
			Label *label = new Label("Tone", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *rotary_stereo = new DragBox<double>(0, 0, 1,
					main_font, 16, tmp_x, tmp_y, 80, 60);
			rotary_stereo->property.bind(rotary->preset.tone, handler);
			controls.push_back(rotary_stereo);

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

		//Rotary Max Delay
		{
			Label *label = new Label("Delay", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *rotary_stereo = new DragBox<double>(0, 0, 0.002,
				main_font, 16, tmp_x, tmp_y, 80, 60);
			rotary_stereo->property.bind(rotary->preset.max_delay, handler);
			rotary_stereo->drag_step = 2;
			controls.push_back(rotary_stereo);

			tmp_y -= 25;
			tmp_x += 90;
		}
		//Rotary Min Amplitude
		{
			Label *label = new Label("Amplitude", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *rotary_stereo = new DragBox<double>(0, 0, 1,
				main_font, 16, tmp_x, tmp_y, 80, 60);
			rotary_stereo->property.bind(rotary->preset.min_amplitude, handler);
			controls.push_back(rotary_stereo);

			tmp_y -= 25;
			tmp_x += 90;
		}
		tmp_y += 100;
		tmp_x = 10;

		//Mix
		{
			Label *label = new Label("Mix", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *rotary_stereo = new DragBox<double>(0, 0, 1,
				main_font, 16, tmp_x, tmp_y, 80, 60);
			rotary_stereo->property.bind(rotary->preset.mix, handler);
			controls.push_back(rotary_stereo);

			tmp_y -= 25;
			tmp_x += 90;
		}

		//Horn Slow Freq
		{
			Label *label = new Label("HS Freq", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0, 10,
				main_font, 16, tmp_x, tmp_y, 80, 60);
			value->property.bind(rotary->preset.horn_slow_frequency, handler);
			value->drag_step = 2;
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 90;
		}
		//Horn Fast Freq
		{
			Label *label = new Label("HF Freq", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0, 10,
				main_font, 16, tmp_x, tmp_y, 80, 60);
			value->property.bind(rotary->preset.horn_fast_frequency, handler);
			value->drag_step = 2;
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 90;
		}
		//Bass Slow Freq
		{
			Label *label = new Label("BS Freq", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0, 10,
				main_font, 16, tmp_x, tmp_y, 80, 60);
			value->property.bind(rotary->preset.bass_slow_frequency, handler);
			value->drag_step = 2;
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 90;
		}
		//Bass Fast Freq
		{
			Label *label = new Label("BF Freq", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0, 10,
				main_font, 16, tmp_x, tmp_y, 80, 60);
			value->property.bind(rotary->preset.bass_fast_frequency, handler);
			value->drag_step = 2;
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 90;
		}
		tmp_y += 100;
		tmp_x = 10;

		//Horn Slow Ramp
		{
			Label *label = new Label("HS Ramp", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0, 10,
				main_font, 16, tmp_x, tmp_y, 80, 60);
			value->property.bind(rotary->preset.horn_slow_ramp, handler);
			value->drag_step = 2;
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 90;
		}
		//Horn Fast Ramp
		{
			Label *label = new Label("HF Ramp", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0, 10,
				main_font, 16, tmp_x, tmp_y, 80, 60);
			value->property.bind(rotary->preset.horn_fast_ramp, handler);
			value->drag_step = 2;
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 90;
		}
		//Bass Slow Ramp
		{
			Label *label = new Label("BS Ramp", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0, 10,
				main_font, 16, tmp_x, tmp_y, 80, 60);
			value->property.bind(rotary->preset.bass_slow_ramp, handler);
			value->drag_step = 2;
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 90;
		}
		//Bass Fast Ramp
		{
			Label *label = new Label("BF Ramp", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0, 10,
				main_font, 16, tmp_x, tmp_y, 80, 60);
			value->property.bind(rotary->preset.bass_fast_ramp, handler);
			value->drag_step = 2;
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 90;
		}
		tmp_y += 100;
		tmp_x = 10;

		//Room Amount
		{
			Label *label = new Label("Room Amount", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0, 1,
				main_font, 16, tmp_x, tmp_y, 80, 60);
			value->property.bind(rotary->preset.room_amount, handler);
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 90;
		}

		//Room Size
		{
			Label *label = new Label("Room Size", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0, 1,
				main_font, 16, tmp_x, tmp_y, 80, 60);
			value->property.bind(rotary->preset.room_size, handler);
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 90;
		}
	}
	//Reverb
	else if (dynamic_cast<ReverbEffect*>(effect) != nullptr) {
		ReverbEffect* reverb = dynamic_cast<ReverbEffect*>(effect);

		//Background
		bg->rect.setFillColor(sf::Color(80, 200, 255));
		title->text.setString("Jo-Jo Reverb TM");

		//On
		{
			Label *label = new Label("On", main_font, 18, tmp_x, tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			OrganSwitch *on = new OrganSwitch(false, main_font, tmp_x,
					tmp_y, 180, 120);
			on->property.bind(reverb->preset.on, handler);
			controls.push_back(on);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Delay
		{
			Label *label = new Label("Delay", main_font, 18, tmp_x, tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *delay = new DragBox<double>(0, 0, 2, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			delay->property.bind(reverb->preset.delay, handler);
			controls.push_back(delay);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Decay
		{
			Label *label = new Label("Decay", main_font, 18, tmp_x, tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *decay = new DragBox<double>(0, 0, 1, main_font, 24,
				tmp_x, tmp_y, 180, 120);
			decay->property.bind(reverb->preset.decay, handler);
			controls.push_back(decay);

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
			mix->property.bind(reverb->preset.mix, handler);
			controls.push_back(mix);

			tmp_y -= 25;
			tmp_x += 200;
		}

		tmp_x = 10;
		tmp_y += 160;

		//Tone
		{
			Label *label = new Label("Tone", main_font, 18, tmp_x, tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *tone = new DragBox<double>(0, 0, 1, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			tone->property.bind(reverb->preset.tone, handler);
			controls.push_back(tone);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Resonance
		{
			Label *label = new Label("Resonance", main_font, 18, tmp_x, tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *resonance = new DragBox<double>(0, 0, 1, main_font, 24,
				tmp_x, tmp_y, 180, 120);
			resonance->property.bind(reverb->preset.resonance, handler);
			controls.push_back(resonance);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Stereo
		{
			Label *label = new Label("Stereo", main_font, 18, tmp_x, tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *stereo = new DragBox<double>(0, -1, 1, main_font, 24,
				tmp_x, tmp_y, 180, 120);
			stereo->property.bind(reverb->preset.stereo, handler);
			controls.push_back(stereo);

			tmp_y -= 25;
			tmp_x += 200;
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
			waveform->property.bind_cast(chorus->preset.vibrato_waveform, handler);
			controls.push_back(waveform);

			tmp_y -= 25;
			tmp_x += 200;
		}
	}
	//Bit Crusher
	else if (dynamic_cast<BitCrusherEffect*>(effect) != nullptr) {
		BitCrusherEffect* bitcrusher = dynamic_cast<BitCrusherEffect*>(effect);

		//Background
		bg->rect.setFillColor(sf::Color(0, 0, 0));
		title->text.setString("Bit Crusher Lo-Fi");
		title->text.setFillColor(sf::Color::Green);

		//On
		{
			Label *label = new Label("On", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::Green);
			controls.push_back(label);
			tmp_y += 25;

			OrganSwitch *on = new OrganSwitch(false, main_font, tmp_x,
					tmp_y, 180, 120);
			on->property.bind(bitcrusher->preset.on, handler);
			controls.push_back(on);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Bits
		{
			Label *label = new Label("Bits", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::Green);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<int> *rate = new DragBox<int>(16, 1, 32, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			rate->property.bind(bitcrusher->preset.bits, handler);
			controls.push_back(rate);

			tmp_y -= 25;
			tmp_x += 200;
		}
	}
	//Vocoder
	else if (dynamic_cast<VocoderEffect*>(effect) != nullptr) {
		VocoderEffect* vocoder = dynamic_cast<VocoderEffect*>(effect);

		//Background
		bg->rect.setFillColor(sf::Color(0x63, 0x0, 0x85));
		title->text.setString("Vocoder");
		title->text.setFillColor(sf::Color::White);

		//On
		{
			Label *label = new Label("On", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			OrganSwitch *on = new OrganSwitch(false, main_font, tmp_x,
					tmp_y, 180, 120);
			on->property.bind(vocoder->preset.on, handler);
			controls.push_back(on);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Modulator Mix
		{
			Label *label = new Label("Modulator Mix", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *rate = new DragBox<double>(0, 0, 1, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			rate->property.bind(vocoder->preset.modulator_mix, handler);
			controls.push_back(rate);

			tmp_y -= 25;
			tmp_x += 200;
		}
		//Carrier Mix
		{
			Label *label = new Label("Mix", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *rate = new DragBox<double>(0, 0, 1, main_font, 24,
				tmp_x, tmp_y, 180, 120);
			rate->property.bind(vocoder->preset.mix, handler);
			controls.push_back(rate);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Mod Highpass
		{
			Label *label = new Label("Modulator Highpass", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0, 21000, main_font, 24,
				tmp_x, tmp_y, 180, 120);
			value->property.bind(vocoder->preset.mod_highpass, handler);
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}
		tmp_x = 10;
		tmp_y += 160;

		//Gate
		{
			Label *label = new Label("Gate", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0, 1, main_font, 24,
				tmp_x, tmp_y, 180, 120);
			value->property.bind(vocoder->preset.gate, handler);
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}
		//Modulator Boost
		{
			Label *label = new Label("Modulator Boost", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0, 10, main_font, 24,
				tmp_x, tmp_y, 180, 120);
			value->property.bind(vocoder->preset.modulator_amplification, handler);
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Vocoder Boost
		{
			Label *label = new Label("Vocoder Boost", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0, 10, main_font, 24,
				tmp_x, tmp_y, 180, 120);
			value->property.bind(vocoder->preset.post_amplification, handler);
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Slope
		{
			Label *label = new Label("Envelope", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0, 0.2, main_font, 24,
				tmp_x, tmp_y, 180, 120);
			value->property.bind(vocoder->preset.slope, handler);
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Compression
		{
			Label *label = new Label("Normalization", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0, 1, main_font, 24,
				tmp_x, tmp_y, 180, 120);
			value->property.bind(vocoder->preset.normalization, handler);
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}

		tmp_x = 10;
		tmp_y += 160;

		//Formant
		{
			Label *label = new Label("Formant Mode", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			OrganSwitch *formant = new OrganSwitch(true, main_font, tmp_x,
				tmp_y, 180, 120);
			formant->property.bind(vocoder->preset.formant_mode, handler);
			controls.push_back(formant);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Min Freq
		{
			Label *label = new Label("Min Frequency", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0, 21000, main_font, 24,
				tmp_x, tmp_y, 180, 120);
			value->property.bind(vocoder->preset.min_freq, handler);
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Max Freq
		{
			Label *label = new Label("Max Frequency", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0, 21000, main_font, 24,
				tmp_x, tmp_y, 180, 120);
			value->property.bind(vocoder->preset.max_freq, handler);
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Resonance
		{
			Label *label = new Label("Resonance", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0, 1, main_font, 24,
				tmp_x, tmp_y, 180, 120);
			value->property.bind(vocoder->preset.resonance, handler);
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}
	}
	//Tremolo
	else if (dynamic_cast<TremoloEffect*>(effect) != nullptr) {
		TremoloEffect* tremolo = dynamic_cast<TremoloEffect*>(effect);

		//Background
		bg->rect.setFillColor(sf::Color(0, 180, 60));
		title->text.setString("Ultra-Tremolo L07");

		//Tremolo
		{
			Label *label = new Label("On", main_font, 18, tmp_x, tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			OrganSwitch *on = new OrganSwitch(false, main_font, tmp_x,
				tmp_y, 180, 120);
			on->property.bind(tremolo->preset.on, handler);
			controls.push_back(on);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Rate
		{
			Label *label = new Label("Rate", main_font, 18, tmp_x, tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *rate = new DragBox<double>(0, 0, 8, main_font, 24,
				tmp_x, tmp_y, 180, 120);
			rate->property.bind(tremolo->preset.rate, handler);
			controls.push_back(rate);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Depth
		{
			Label *label = new Label("Depth", main_font, 18, tmp_x,
				tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *depth = new DragBox<double>(0, 0, 1, main_font,
				24, tmp_x, tmp_y, 180, 120);
			depth->property.bind(tremolo->preset.depth, handler);
			controls.push_back(depth);

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
			waveform->property.bind_cast(tremolo->preset.waveform, handler);
			controls.push_back(waveform);

			tmp_y -= 25;
			tmp_x += 200;
		}
	}
	//Delay
	else if (dynamic_cast<DelayEffect*>(effect) != nullptr) {
		DelayEffect* delay = dynamic_cast<DelayEffect*>(effect);

		//Background
		bg->rect.setFillColor(sf::Color(255, 120, 120));
		title->text.setString("Delay T5");

		//Left
		Label *left = new Label("Left", main_font, 24, tmp_x, tmp_y);
		controls.push_back(left);
		tmp_y += 30;
		//On
		{
			Label *label = new Label("On", main_font, 18, tmp_x, tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			OrganSwitch *on = new OrganSwitch(false, main_font, tmp_x,
				tmp_y, 180, 120);
			on->property.bind(delay->preset.on, handler);
			controls.push_back(on);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Delay
		{
			Label *label = new Label("Delay", main_font, 18, tmp_x, tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0, 5, main_font, 24,
				tmp_x, tmp_y, 180, 120);
			value->property.bind(delay->preset.left_delay, handler);
			value->drag_step = 4;
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Init
		{
			Label *label = new Label("First Offset", main_font, 18, tmp_x, tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0, 5, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			value->property.bind(delay->preset.left_init_delay_offset, handler);
			value->drag_step = 4;
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Feedback
		{
			Label *label = new Label("Feedback", main_font, 18, tmp_x, tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0, 1, main_font, 24,
				tmp_x, tmp_y, 180, 120);
			value->property.bind(delay->preset.left_feedback, handler);
			value->drag_step = 4;
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Mix
		{
			Label *label = new Label("Mix", main_font, 18, tmp_x, tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0, 1, main_font, 24,
				tmp_x, tmp_y, 180, 120);
			value->property.bind(delay->preset.mix, handler);
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}

		tmp_y += 150;
		tmp_x -= 200 * 5;

		//Right
		Label *right = new Label("Right", main_font, 24, tmp_x, tmp_y);
		controls.push_back(right);
		tmp_y += 30;
		//Stereo
		{
			Label *label = new Label("Stereo", main_font, 18, tmp_x, tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			OrganSwitch *stereo = new OrganSwitch(false, main_font, tmp_x,
				tmp_y, 180, 120);
			stereo->property.bind(delay->preset.stereo, handler);
			controls.push_back(stereo);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Delay
		{
			Label *label = new Label("Delay", main_font, 18, tmp_x, tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0, 5, main_font, 24,
				tmp_x, tmp_y, 180, 120);
			value->property.bind(delay->preset.right_delay, handler);
			value->drag_step = 4;
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Init
		{
			Label *label = new Label("First Offset", main_font, 18, tmp_x, tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0, 5, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			value->property.bind(delay->preset.right_init_delay_offset, handler);
			value->drag_step = 4;
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Feedback
		{
			Label *label = new Label("Feedback", main_font, 18, tmp_x, tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0, 1, main_font, 24,
				tmp_x, tmp_y, 180, 120);
			value->property.bind(delay->preset.right_feedback, handler);
			value->drag_step = 4;
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}

	}
	//Flanger
	else if (dynamic_cast<FlangerEffect*>(effect) != nullptr) {
		FlangerEffect *flanger = dynamic_cast<FlangerEffect*>(effect);

		//Background
		bg->rect.setFillColor(sf::Color(0x63, 0x0, 0x85));
		title->text.setString("Orbit Flanger FC-6");

		//Flanger
		{
			Label *label = new Label("On", main_font, 18, tmp_x, tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			OrganSwitch *on = new OrganSwitch(false, main_font, tmp_x, tmp_y,
					180, 120);
			on->property.bind(flanger->preset.on, handler);
			controls.push_back(on);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Vibrato Rate
		{
			Label *label = new Label("Vibrato Rate", main_font, 18, tmp_x,
					tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *rate = new DragBox<double>(0, 0, 8, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			rate->property.bind(flanger->preset.vibrato_rate, handler);
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

			DragBox<double> *depth = new DragBox<double>(0, 0, 1, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			depth->property.bind(flanger->preset.vibrato_depth, handler);
			controls.push_back(depth);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Mix
		{
			Label *label = new Label("Mix", main_font, 18, tmp_x, tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *mix = new DragBox<double>(0, 0, 1, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			mix->property.bind(flanger->preset.mix, handler);
			controls.push_back(mix);

			tmp_y -= 25;
			tmp_x += 200;
		}
		tmp_x = 10;
		tmp_y += 160;

		//Delay
		{
			Label *label = new Label("Delay", main_font, 18, tmp_x, tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0.0, 0.02,
					main_font, 24, tmp_x, tmp_y, 180, 120);
			value->property.bind(flanger->preset.delay, handler);
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}
		//Waveform
		{
			Label *label = new Label("Waveform", main_font, 18, tmp_x, tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			std::vector<std::string> waveforms = { "Sine", "Saw Down", "Saw Up",
					"Square", "Triangle" };

			ComboBox *waveform = new ComboBox(4, waveforms, main_font, 24, 0,
					tmp_x, tmp_y, 180, 120);
			waveform->property.bind_cast(flanger->preset.vibrato_waveform, handler);
			controls.push_back(waveform);

			tmp_y -= 25;
			tmp_x += 200;
		}
		//Feeddack
		{
			Label *label = new Label("Feedback", main_font, 18, tmp_x, tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0.0, 1,
					main_font, 24, tmp_x, tmp_y, 180, 120);
			value->property.bind(flanger->preset.feedback, handler);
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}
	}
	//Phaser
	else if (dynamic_cast<PhaserEffect*>(effect) != nullptr) {
		PhaserEffect* phaser = dynamic_cast<PhaserEffect*>(effect);

		//Background
		bg->rect.setFillColor(sf::Color(0xE9, 0xBD, 0x72));
		title->text.setString("Vibe Phase V9");

		//Phaser
		{
			Label *label = new Label("On", main_font, 18, tmp_x, tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			OrganSwitch *on = new OrganSwitch(false, main_font, tmp_x,
					tmp_y, 180, 120);
			on->property.bind(phaser->preset.on, handler);
			controls.push_back(on);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Vibrato Rate
		{
			Label *label = new Label("Rate", main_font, 18, tmp_x, tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *rate = new DragBox<double>(0, 0, 8, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			rate->property.bind(phaser->preset.lfo_rate, handler);
			controls.push_back(rate);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Vibrato Depth
		{
			Label *label = new Label("Depth", main_font, 18, tmp_x,
					tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *depth = new DragBox<double>(0, 0, 1, main_font,
					24, tmp_x, tmp_y, 180, 120);
			depth->property.bind(phaser->preset.lfo_depth, handler);
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
			mix->property.bind(phaser->preset.mix, handler);
			controls.push_back(mix);

			tmp_y -= 25;
			tmp_x += 200;
		}
		tmp_x = 10;
		tmp_y += 160;

		//Delay
		{
			Label *label = new Label("Cutoff", main_font, 18, tmp_x,
				tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0.0, 1, main_font,
				24, tmp_x, tmp_y, 180, 120);
			value->property.bind(phaser->preset.center_cutoff, handler);
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
			waveform->property.bind_cast(phaser->preset.vibrato_waveform, handler);
			controls.push_back(waveform);

			tmp_y -= 25;
			tmp_x += 200;
		}
	}
	//WahWah
	else if (dynamic_cast<WahWahEffect*>(effect) != nullptr) {
		WahWahEffect* wahwah = dynamic_cast<WahWahEffect*>(effect);

		//Background
		bg->rect.setFillColor(sf::Color(0x53, 0x32, 0x00));
		title->text.setString("Funky Wah AW-5");

		//Wah Wah
		{
			Label *label = new Label("On", main_font, 18, tmp_x, tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			OrganSwitch *on = new OrganSwitch(false, main_font, tmp_x,
					tmp_y, 180, 120);
			on->property.bind(wahwah->preset.on, handler);
			controls.push_back(on);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Auto Wah
		{
			Label *label = new Label("Auto Wah", main_font, 18, tmp_x, tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			OrganSwitch *on = new OrganSwitch(false, main_font, tmp_x, tmp_y,
					180, 120);
			on->property.bind(wahwah->preset.auto_wah, handler);
			controls.push_back(on);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Cutoff
		{
			Label *label = new Label("Cutoff", main_font, 18, tmp_x, tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *rate = new DragBox<double>(0, 0, 1, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			rate->property.bind(wahwah->preset.cutoff, handler);
			controls.push_back(rate);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Resonance
		{
			Label *label = new Label("Resonance", main_font, 18, tmp_x,
					tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *depth = new DragBox<double>(0, 0, 1, main_font,
					24, tmp_x, tmp_y, 180, 120);
			depth->property.bind(wahwah->preset.resonance, handler);
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
			mix->property.bind(wahwah->preset.mix, handler);
			controls.push_back(mix);

			tmp_y -= 25;
			tmp_x += 200;
		}
		tmp_x = 10;
		tmp_y += 160;

		//Amount
		{
			Label *label = new Label("Amount", main_font, 18, tmp_x,
				tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0.0, 10, main_font,
				24, tmp_x, tmp_y, 180, 120);
			value->property.bind(wahwah->preset.amount, handler);
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}
	}
	//Equalizer
	else if (dynamic_cast<EqualizerEffect*>(effect) != nullptr) {
		EqualizerEffect *equalizer = dynamic_cast<EqualizerEffect*>(effect);

		//Background
		bg->rect.setFillColor(sf::Color::Black);
		title->text.setString("4-Band EQ");
		title->text.setFillColor(sf::Color::White);

		//On
		{
			Label *label = new Label("On", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			OrganSwitch *on = new OrganSwitch(false, main_font, tmp_x, tmp_y,
					180, 120);
			on->property.bind(equalizer->preset.on, handler);
			controls.push_back(on);

			tmp_y -= 25;
			tmp_x += 200;
		}

		DragBoxScale<double> db_scale;
		db_scale.value = [](double prog, double min, double max) {
			double db = (max - min) * prog + min;
			return db_to_amp(db);
		};
		db_scale.progress = [](double val, double min, double max) {
			double db = amp_to_db(val);
			return (db - min)/(max - min);
		};

		//Low Gain
		{
			Label *label = new Label("Low Gain", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, -1, 5, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			value->border = 0;
			value->property.bind(equalizer->preset.low_gain, handler);
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Low Mid Gain
		{
			Label *label = new Label("Low Mid Gain", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, -1, 5, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			value->border = 0;
			value->property.bind(equalizer->preset.low_mid_gain, handler);
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Mid Gain
		{
			Label *label = new Label("Mid Gain", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, -1, 5, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			value->border = 0;
			value->property.bind(equalizer->preset.mid_gain, handler);
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//High Gain
		{
			Label *label = new Label("High Gain", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, -1, 5, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			value->border = 0;
			value->property.bind(equalizer->preset.high_gain, handler);
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}

		tmp_y += 160;
		tmp_x -= 200 * 4;

		//Low Freq
		{
			Label *label = new Label("Low Freq", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 20, 400, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			value->property.bind(equalizer->preset.low_freq, handler);
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Low Mid Freq
		{
			Label *label = new Label("Low Mid Freq", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 100, 1000, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			value->property.bind(equalizer->preset.low_mid_freq, handler);
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Mid Freq
		{
			Label *label = new Label("Mid Freq", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 200, 8000, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			value->property.bind(equalizer->preset.mid_freq, handler);
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//High Freq
		{
			Label *label = new Label("High Freq", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 1000, 20000, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			value->property.bind(equalizer->preset.high_freq, handler);
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}
	}
	//Compressor
	else if (dynamic_cast<CompressorEffect*>(effect) != nullptr) {
		CompressorEffect *compressor = dynamic_cast<CompressorEffect*>(effect);

		//Background
		bg->rect.setFillColor(sf::Color::Black);
		title->text.setString("Power Compressor 7C");
		title->text.setFillColor(sf::Color::White);

		//On
		{
			Label *label = new Label("On", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			OrganSwitch *on = new OrganSwitch(false, main_font, tmp_x, tmp_y,
					180, 120);
			on->property.bind(compressor->preset.on, handler);
			controls.push_back(on);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Threshold
		{
			Label *label = new Label("Threshold", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, -50, 0, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			value->property.bind(compressor->preset.threshold, handler);
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Ratio
		{
			Label *label = new Label("Ratio", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(1, 1, 15, main_font, 24,
				tmp_x, tmp_y, 180, 120);
			value->property.bind(compressor->preset.ratio, handler);
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}

		tmp_y += 160;
		tmp_x -= 200 * 3;
		//Attack
		{
			Label *label = new Label("Attack", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0, 1, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			value->property.bind(compressor->preset.attack, handler);
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Release
		{
			Label *label = new Label("Release", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0, 1, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			value->property.bind(compressor->preset.release, handler);
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}
		//Makeup Gain
		{
			Label *label = new Label("Makeup Gain", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0, 5, main_font, 24,
				tmp_x, tmp_y, 180, 120);
			value->border = 0;
			value->property.bind(compressor->preset.makeup_gain, handler);
			controls.push_back(value);

			tmp_y -= 25;
			tmp_x += 200;
		}
	}

	controls.push_back(binder.create_button(frame.get_width() - 170, frame.get_height() - 40, &frame));
	//Back Button
	Button* back = new Button("Back", main_font, 18, frame.get_width() - 70, frame.get_height() - 40, 70, 40);
	back->rect.setFillColor(sf::Color::Yellow);
	back->set_on_click([&frame, this]() {
		frame.menu_back();
	});
	controls.push_back(back);
	lock.unlock();
	return {controls};
}

bool EffectView::on_action(Control *control) {
	return binder.on_action(control);
}
