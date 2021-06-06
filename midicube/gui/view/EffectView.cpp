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
#include "../../effect/compressor.h"
#include "../../effect/delay.h"
#include "../../effect/equalizer.h"
#include "../../effect/flanger.h"
#include "../../effect/phaser.h"
#include "../../effect/reverb.h"
#include "../../effect/rotary_speaker.h"
#include "../../effect/tremolo.h"
#include "../../effect/vocoder.h"
#include "../../effect/wahwah.h"

EffectView::EffectView(Effect *e, std::function<ViewController* ()> b) :
		effect(e), back(b) {

}

EffectView::~EffectView() {

}

void create_cc_control(MidiControlHandler& m, ActionHandler& handler, void* field, int x, int y, int width, int height, std::vector<Control*>& controls, std::vector<Control*>& show_midi) {
	DragBox<int>* midi = new DragBox<int>(0, 0, 128, main_font, 16, x, y, width, height);
	midi->property.bind(m.get_binding(field)->cc, handler);
	controls.push_back(midi);
	show_midi.push_back(midi);
}

Scene EffectView::create(Frame &frame) {
	std::vector<Control*> controls;

	ActionHandler &handler = frame.cube.action_handler;

	std::vector<Control*> hide_midi = {};
	std::vector<Control*> show_midi = {};

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
			hide_midi.push_back(amplifier);

			create_cc_control(effect->cc, handler, &amp->preset.on, tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(boost);

			create_cc_control(effect->cc, handler, &amp->preset.post_gain, tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(overdrive);

			create_cc_control(effect->cc, handler, &amp->preset.drive, tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(amp_tone);

			create_cc_control(effect->cc, handler, &amp->preset.tone, tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(on);

			create_cc_control(effect->cc, handler, &rotary->preset.on, tmp_x, tmp_y, 80, 60, controls, show_midi);

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
			hide_midi.push_back(rotary_speed);

			create_cc_control(effect->cc, handler, &rotary->preset.fast, tmp_x, tmp_y, 80, 60, controls, show_midi);

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
		tmp_y += 100;
		tmp_x = 10;

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
			hide_midi.push_back(on);

			create_cc_control(effect->cc, handler, &reverb->preset.on, tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(delay);

			create_cc_control(effect->cc, handler, &reverb->preset.delay, tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(decay);

			create_cc_control(effect->cc, handler, &reverb->preset.decay, tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(mix);

			create_cc_control(effect->cc, handler, &reverb->preset.mix, tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(tone);

			create_cc_control(effect->cc, handler, &reverb->preset.tone, tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(resonance);

			create_cc_control(effect->cc, handler, &reverb->preset.resonance, tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(stereo);

			create_cc_control(effect->cc, handler, &reverb->preset.stereo, tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(on);

			create_cc_control(effect->cc, handler, &chorus->preset.on, tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(rate);

			create_cc_control(effect->cc, handler, &chorus->preset.vibrato_rate, tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(depth);

			create_cc_control(effect->cc, handler, &chorus->preset.vibrato_depth, tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(mix);

			create_cc_control(effect->cc, handler, &chorus->preset.mix, tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(value);

			create_cc_control(effect->cc, handler, &chorus->preset.delay, tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(on);

			create_cc_control(effect->cc, handler, &bitcrusher->preset.on, tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(on);

			create_cc_control(effect->cc, handler, &tremolo->preset.on, tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(rate);

			create_cc_control(effect->cc, handler, &tremolo->preset.rate, tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(depth);

			create_cc_control(effect->cc, handler, &tremolo->preset.depth, tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			waveform->property.bind(tremolo->preset.waveform, handler);
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
			hide_midi.push_back(on);

			create_cc_control(effect->cc, handler, &delay->preset.on, tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(value);

			create_cc_control(effect->cc, handler, &delay->preset.left_delay, tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(value);

			create_cc_control(effect->cc, handler, &delay->preset.left_init_delay_offset,
					tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(value);

			create_cc_control(effect->cc, handler, &delay->preset.left_feedback, tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(value);

			create_cc_control(effect->cc, handler, &delay->preset.mix, tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(stereo);

			create_cc_control(effect->cc, handler, &delay->preset.stereo, tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(value);

			create_cc_control(effect->cc, handler, &delay->preset.right_delay, tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(value);

			create_cc_control(effect->cc, handler, &delay->preset.right_init_delay_offset,
					tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(value);

			create_cc_control(effect->cc, handler, &delay->preset.right_feedback, tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(on);

			create_cc_control(effect->cc, handler, &flanger->preset.on, tmp_x,
					tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(rate);

			create_cc_control(effect->cc, handler, &flanger->preset.vibrato_rate,
					tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(depth);

			create_cc_control(effect->cc, handler,
					&flanger->preset.vibrato_depth, tmp_x, tmp_y, 180, 120,
					controls, show_midi);

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
			hide_midi.push_back(mix);

			create_cc_control(effect->cc, handler, &flanger->preset.mix, tmp_x,
					tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(value);

			create_cc_control(effect->cc, handler, &flanger->preset.delay, tmp_x,
					tmp_y, 180, 120, controls, show_midi);

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
			waveform->property.bind(flanger->preset.vibrato_waveform, handler);
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
			hide_midi.push_back(value);

			create_cc_control(effect->cc, handler, &flanger->preset.feedback, tmp_x,
					tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(on);

			create_cc_control(effect->cc, handler, &phaser->preset.on, tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(rate);

			create_cc_control(effect->cc, handler, &phaser->preset.lfo_rate, tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(depth);

			create_cc_control(effect->cc, handler, &phaser->preset.lfo_depth, tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(mix);

			create_cc_control(effect->cc, handler, &phaser->preset.mix, tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(value);

			create_cc_control(effect->cc, handler, &phaser->preset.center_cutoff, tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			waveform->property.bind(phaser->preset.vibrato_waveform, handler);
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
			hide_midi.push_back(on);

			create_cc_control(effect->cc, handler, &wahwah->preset.on, tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(on);

			create_cc_control(effect->cc, handler, &wahwah->preset.auto_wah, tmp_x,
					tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(rate);

			create_cc_control(effect->cc, handler, &wahwah->preset.cutoff, tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(depth);

			create_cc_control(effect->cc, handler, &wahwah->preset.resonance, tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(mix);

			create_cc_control(effect->cc, handler, &wahwah->preset.mix, tmp_x, tmp_y, 180, 120, controls, show_midi);

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

			DragBox<double> *value = new DragBox<double>(0, 0.0, 1, main_font,
				24, tmp_x, tmp_y, 180, 120);
			value->property.bind(wahwah->preset.amount, handler);
			controls.push_back(value);
			hide_midi.push_back(value);

			create_cc_control(effect->cc, handler, &wahwah->preset.amount, tmp_x, tmp_y, 180, 120, controls, show_midi);

			tmp_y -= 25;
			tmp_x += 200;
		}
		//Pedal
		{
			Label *label = new Label("Pedal", main_font, 18, tmp_x,
				tmp_y);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0.0, 1, main_font,
				24, tmp_x, tmp_y, 180, 120);
			value->property.bind(wahwah->pedal, handler);
			controls.push_back(value);
			hide_midi.push_back(value);

			create_cc_control(effect->cc, handler, &wahwah->pedal, tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(on);

			create_cc_control(effect->cc, handler, &equalizer->preset.on, tmp_x,
					tmp_y, 180, 120, controls, show_midi);

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

			DragBox<double> *value = new DragBox<double>(0, -15, 15, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			value->border = 0;
			value->property.bind(equalizer->preset.low_gain, handler);
			controls.push_back(value);
			hide_midi.push_back(value);

			create_cc_control(effect->cc, handler, &equalizer->preset.low_gain,
					tmp_x, tmp_y, 180, 120, controls, show_midi);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Low Mid Gain
		{
			Label *label = new Label("Low Mid Gain", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, -15, 15, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			value->border = 0;
			value->property.bind(equalizer->preset.low_mid_gain, handler);
			controls.push_back(value);
			hide_midi.push_back(value);

			create_cc_control(effect->cc, handler, &equalizer->preset.low_mid_gain,
					tmp_x, tmp_y, 180, 120, controls, show_midi);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Mid Gain
		{
			Label *label = new Label("Mid Gain", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, -15, 15, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			value->border = 0;
			value->property.bind(equalizer->preset.mid_gain, handler);
			controls.push_back(value);
			hide_midi.push_back(value);

			create_cc_control(effect->cc, handler, &equalizer->preset.mid_gain,
					tmp_x, tmp_y, 180, 120, controls, show_midi);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//High Gain
		{
			Label *label = new Label("High Gain", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, -15, 15, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			value->border = 0;
			value->property.bind(equalizer->preset.high_gain, handler);
			controls.push_back(value);
			hide_midi.push_back(value);

			create_cc_control(effect->cc, handler, &equalizer->preset.high_gain,
					tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(value);

			create_cc_control(effect->cc, handler, &equalizer->preset.low_freq,
					tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(value);

			create_cc_control(effect->cc, handler, &equalizer->preset.low_mid_freq,
					tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(value);

			create_cc_control(effect->cc, handler, &equalizer->preset.mid_freq,
					tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(value);

			create_cc_control(effect->cc, handler, &equalizer->preset.high_freq,
					tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(on);

			create_cc_control(effect->cc, handler, &compressor->preset.on, tmp_x,
					tmp_y, 180, 120, controls, show_midi);

			tmp_y -= 25;
			tmp_x += 200;
		}

		//Threshold
		{
			Label *label = new Label("Threshold", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0, 2, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			value->property.bind(compressor->preset.threshold, handler);
			controls.push_back(value);
			hide_midi.push_back(value);

			create_cc_control(effect->cc, handler, &compressor->preset.threshold,
					tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(value);

			create_cc_control(effect->cc, handler, &compressor->preset.ratio,
				tmp_x, tmp_y, 180, 120, controls, show_midi);

			tmp_y -= 25;
			tmp_x += 200;
		}

		tmp_y += 160;
		tmp_x -= 200 * 3;
		//Attack
		{
			Label *label = new Label("Mid Gain", main_font, 18, tmp_x, tmp_y);
			label->text.setFillColor(sf::Color::White);
			controls.push_back(label);
			tmp_y += 25;

			DragBox<double> *value = new DragBox<double>(0, 0, 1, main_font, 24,
					tmp_x, tmp_y, 180, 120);
			value->property.bind(compressor->preset.attack, handler);
			controls.push_back(value);
			hide_midi.push_back(value);

			create_cc_control(effect->cc, handler, &compressor->preset.attack,
					tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(value);

			create_cc_control(effect->cc, handler, &compressor->preset.release,
					tmp_x, tmp_y, 180, 120, controls, show_midi);

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
			hide_midi.push_back(value);

			create_cc_control(effect->cc, handler, &compressor->preset.makeup_gain,
				tmp_x, tmp_y, 180, 120, controls, show_midi);
			tmp_y -= 25;
			tmp_x += 200;
		}
	}

	//Edit MIDI Button
	Button* edit_midi = new Button("Edit MIDI", main_font, 18, frame.get_width() - 320, frame.get_height() - 40, 100, 40);
	edit_midi->rect.setFillColor(sf::Color::Yellow);
	edit_midi->set_on_click([show_midi, hide_midi, this]() {
		this->edit_midi = !this->edit_midi;
		for (Control* control : show_midi) {
			control->set_visible(this->edit_midi);
		}
		for (Control* control : hide_midi) {
			control->set_visible(!this->edit_midi);
		}
	});
	controls.push_back(edit_midi);

	//Exit Button
	Button *exit = new Button("Back", main_font, 18, frame.get_width() - 70,
			frame.get_height() - 40, 70, 40);
	exit->set_on_click([&frame, this]() {
		frame.change_view(this->back());
	});
	exit->rect.setFillColor(sf::Color::Yellow);
	controls.push_back(exit);

	//Hide midi controls
	for (Control* control : show_midi) {
		control->set_visible(this->edit_midi);
	}
	for (Control* control : hide_midi) {
		control->set_visible(!this->edit_midi);
	}

	return {controls};
}
