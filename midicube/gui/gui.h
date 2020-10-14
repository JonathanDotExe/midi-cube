/*
 * gui.h
 *
 *  Created on: 8 Jul 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_GUI_H_
#define MIDICUBE_GUI_GUI_H_

#include "model.h"
#include "../util.h"
#include "../soundengine/organ.h"
#include "../soundengine/synthesizer.h"
#include "../soundengine/soundengine.h"
#include <type_traits>

#include "engine/core.h"
#include "engine/container.h"
#include "engine/control.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 600

class Dialog {
public:
	virtual bool draw(float x, float y) = 0;
	virtual float width() = 0;
	virtual float height() = 0;
	virtual ~Dialog() {

	}
};

class MainMenuView : public ViewController {

public:

	Node* init(Frame* frame);

};

class SoundEngineMenuView : public ViewController {

private:
	SoundEngineDevice* engine;

public:

	SoundEngineMenuView(SoundEngineDevice* engine);

	Node* init(Frame* frame);

};

/*
struct DeviceDragInfo {
	int last_x;
	int last_y;
	bool dragging;
	std::string device;
	int start_x;
	int start_y;
};

struct BindingDragInfo {
	bool dragging;
	bool dialog;
	std::string input_device;
	std::string output_device;

	bool input_channel_editmode = false;
	bool output_channel_editmode = false;
	bool start_note_editmode = false;
	bool end_note_editmode = false;
	bool octave_editmode = false;

	int input_channel = -1;
	int output_channel = -1;
	int start_note = 0;
	int end_note = 127;
	int octave = 0;

	bool transfer_channel_aftertouch = true;
	bool transfer_pitch_bend = true;
	bool transfer_cc = true;
	bool transfer_prog_change = true;
	bool transfer_other = true;
};

class DevicesMenuView : public View {

private:
	DeviceDragInfo device_drag;
	BindingDragInfo binding_drag;
	bool drag_binding = false;
	double press_time = -2;

public:

	DevicesMenuView();

	View* draw();

};


class SoundEngineDeviceMenuView : public View {

private:
	SoundEngineDevice* device;
	std::string options;
	bool bpm_editmode = false;

public:

	SoundEngineDeviceMenuView(SoundEngineDevice* device);

	View* draw();

};

View* create_view_for_device(AudioDevice* device);

class B3OrganEngineMenuView : public View {
private:
	B3OrganData* data;
	bool edit_midi = false;
	std::array<std::string, ORGAN_DRAWBAR_COUNT> drawbar_titles = {"16'", "5 1/3'", "8'", "4'", "2 2/3'", "2'", "1 3/5'", "1 1/3'", "1"};

	std::array<bool, ORGAN_DRAWBAR_COUNT> drawbar_editmode = {false};
	bool rotary_editmode = false;
	bool rotary_speed_editmode = false;

	bool percussion_editmode = false;
	bool percussion_third_harmonic_editmode = false;
	bool percussion_fast_decay_editmode = false;
	bool percussion_soft_editmode = false;

	bool overdrive_editmode = false;
	bool overdrive_gain_editmode = false;
	bool overdrive_clip_editmode = false;

	void draw_drawbar (int x, int y, int width, int height, size_t index);

public:
	B3OrganEngineMenuView(B3OrganData* device);
	View* draw();

};

class SynthesizerEngineMenuView : public View {
private:
	SynthesizerData* data;
	Dialog* comp_dialog = nullptr;
	Dialog* slot_dialog = nullptr;
	int current_preset = 1;
	void set_comp_dialog(Dialog* d);
	void set_slot_dialog(Dialog* d);

public:
	SynthesizerEngineMenuView(SynthesizerData* data);
	View* draw();
	~SynthesizerEngineMenuView();
};

class OscilatorDialog : public Dialog {
private:
	OscilatorComponent* osc = nullptr;
public:
	OscilatorDialog(OscilatorComponent* osc);
	bool draw(float x, float y);
	float width();
	float height();
};

const FixedScale ATTACK_SCALE(0.0005, {{0.35, 1}, {0.5, 2}, {0.6, 3}, {0.7, 4}, {0.75, 5}, {0.9, 10}}, 15);
const FixedScale DECAY_SCALE(0.0, {{0.35, 1}, {0.5, 2}, {0.6, 3}, {0.65, 4}, {0.8, 10}}, 50);
const FixedScale RELEASE_SCALE(0.003, {{0.35, 1}, {0.5, 2}, {0.6, 3}, {0.65, 4}, {0.8, 10}}, 50);

class AmpEnvelopeDialog : public Dialog {
private:
	AmpEnvelopeComponent* amp = nullptr;
public:
	AmpEnvelopeDialog(AmpEnvelopeComponent* amp);
	bool draw(float x, float y);
	float width();
	float height();
};

class ModEnvelopeDialog : public Dialog {
private:
	ModEnvelopeComponent* env = nullptr;
public:
	ModEnvelopeDialog(ModEnvelopeComponent* env);
	bool draw(float x, float y);
	float width();
	float height();
};


const FixedScale FILTER_CUTOFF_SCALE(14, {{0.2, 200}, {0.3, 400}, {0.4, 800}, {0.7, 2500}, {0.8, 5000}, {0.9, 10000}}, 21000);
template <typename T>
class FilterDialog : public Dialog {
private:
	T* filter = nullptr;
public:
	FilterDialog(T* filter);

	bool draw(float x, float y);

	float width();

	float height();
};

const FixedScale LFO_FREQ_SCALE(0, {{0.2, 0.2}, {0.3, 0.4}, {0.4, 1}, {0.6, 3}, {0.7, 6}, {0.8, 20}, {0.9, 100}}, 512);

class LFODialog: public Dialog {
private:
	LFOComponent* lfo = nullptr;
public:
	LFODialog(LFOComponent* lfo);
	bool draw(float x, float y);
	float width();
	float height();
};

class ControlChangeDialog: public Dialog {
private:
	ControlChangeComponent* cc = nullptr;
	bool cc_editmode = false;
public:
	ControlChangeDialog(ControlChangeComponent* cc);
	bool draw(float x, float y);
	float width();
	float height();
};

class VelocityDialog : public Dialog {
private:
	VelocityComponent* vel = nullptr;
public:
	VelocityDialog(VelocityComponent* vel);
	bool draw(float x, float y);
	float width();
	float height();
};

class ComponentSlotDialog : public Dialog {
private:
	ComponentSlot* slot = nullptr;
	ComponentPropertyBinding binding;
	size_t binding_index = 0;
	bool edit_binding = false;
public:
	ComponentSlotDialog(ComponentSlot* slot);
	bool draw(float x, float y);
	float width();
	float height();
};

View* create_view_for_engine(std::string engine_name, SoundEngine* engine);

Dialog* create_dialog_for_component(std::string component_name, SynthComponent* data);*/

#endif /* MIDICUBE_GUI_GUI_H_ */
