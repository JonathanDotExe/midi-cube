/*
 * soundengine.h
 *
 *  Created on: 30 Sep 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_FRAMEWORK_CORE_PLUGINS_SOUNDENGINE_H_
#define MIDICUBE_FRAMEWORK_CORE_PLUGINS_SOUNDENGINE_H_

#include "../plugin.h"

class SoundEngine : public PluginInstance {

public:

	SoundEngine(PluginInfo i) : Plugin(i) {

	}

	virtual void process(const SampleInfo& info) = 0;

	virtual void recieve_midi(MidiMessage& message, const SampleInfo& info) {

	}

	virtual void press_note(SampleInfo& info, unsigned int real_note, unsigned int note, double velocity, size_t polyphony_limit) = 0;

	virtual void release_note(SampleInfo& info, unsigned int real_note) = 0;

	virtual ~SoundEngine() {

	};

};

template<typename V, size_t P>
class BaseSoundEngine : public SoundEngine {
protected:
	VoiceManager<V, P> note;

public:
	virtual bool midi_message(MidiMessage& msg, int transpose, SampleInfo& info, KeyboardEnvironment& env, size_t polyphony_limit);

	virtual void press_note(SampleInfo& info, unsigned int real_note, unsigned int note, double velocity, size_t polyphony_limit);

	virtual void release_note(SampleInfo& info, unsigned int note);

	EngineStatus process_sample(double& lsample, double& rsample, SampleInfo& info, KeyboardEnvironment& env);

	virtual void process_note_sample(double& lsample, double& rsample, SampleInfo& info, V& note, KeyboardEnvironment& env, size_t note_index) = 0;


	virtual void process_sample(double& lsample, double& rsample, SampleInfo& info, KeyboardEnvironment& env, EngineStatus& status) {

	};

	virtual bool control_change(unsigned int control, unsigned int value) {
		return false;
	};

	virtual bool note_finished(SampleInfo& info, V& note, KeyboardEnvironment& env, size_t note_index) {
		return !note.pressed || (env.sustain && note.release_time >= env.sustain_time);
	};

	virtual ~BaseSoundEngine() {

	};

};

//BaseSoundEngine
template<typename V, size_t P>
bool BaseSoundEngine<V, P>::midi_message(MidiMessage& message, int transpose, SampleInfo& info, KeyboardEnvironment& env, size_t polyphony_limit) {
	bool changed = false;
	switch (message.type) {
		case MessageType::NOTE_ON:
			press_note(info, message.note(), message.note() + transpose, message.velocity()/127.0, polyphony_limit);
			break;
		case MessageType::NOTE_OFF:
			release_note(info, message.note());
			break;
		case MessageType::CONTROL_CHANGE:
			changed = control_change(message.control(), message.value());
			break;
		default:
			break;
	}
	return changed;
}

template<typename V, size_t P>
void BaseSoundEngine<V, P>::press_note(SampleInfo& info, unsigned int real_note, unsigned int note, double velocity, size_t polyphony_limit) {
	this->note.press_note(info, real_note, note, velocity, polyphony_limit);
}

template<typename V, size_t P>
void BaseSoundEngine<V, P>::release_note(SampleInfo& info, unsigned int note) {
	this->note.release_note(info, note);
}

template<typename V, size_t P>
EngineStatus BaseSoundEngine<V, P>::process_sample(double& lsample, double& rsample, SampleInfo& info, KeyboardEnvironment& env) {
	EngineStatus status = {0, 0};
	//Notes
	for (size_t i = 0; i < P; ++i) {
		if (note.note[i].valid) {
			if (note_finished(info, note.note[i], env, i)) {
				note.note[i].valid = false;
			}
			else {
				++status.pressed_notes; //TODO might cause problems in the future
				note.note[i].phase_shift += (env.pitch_bend - 1) * info.time_step;
				process_note_sample(lsample, rsample, info, note.note[i], env, i);
				if (!status.pressed_notes || note.note[status.latest_note_index].start_time < note.note[i].start_time) {
					status.latest_note_index = i;
				}
			}
		}
	}
	//Static sample
	process_sample(lsample, rsample, info, env, status);

	return status;
}


#endif /* MIDICUBE_FRAMEWORK_CORE_PLUGINS_SOUNDENGINE_H_ */
