/*
 * soundengine.h
 *
 *  Created on: 30 Sep 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_FRAMEWORK_CORE_PLUGINS_SOUNDENGINE_H_
#define MIDICUBE_FRAMEWORK_CORE_PLUGINS_SOUNDENGINE_H_

#include "../plugin.h"
#include "../../util/voice.h"

struct EngineStatus {
	size_t pressed_notes = 0;
	size_t latest_note_index = 0;
};

template<typename V, size_t P>
class SoundEngine : public PluginInstance {
private:
	EngineStatus status;

protected:
	VoiceManager<V, P> voice_mgr;

public:

	SoundEngine(PluginHost& h, Plugin& p) : PluginInstance(h, p) {

	}

	virtual void process(const SampleInfo& info) {
		EngineStatus status = {0, 0};
		const KeyboardEnvironment& env = get_host().get_environment();
		//Notes
		for (size_t i = 0; i < P; ++i) {
			if (voice_mgr.note[i].valid) {
				if (note_finished(info, voice_mgr.note[i], i)) {
					voice_mgr.note[i].valid = false;
				}
				else {
					++status.pressed_notes; //TODO might cause problems in the future
					voice_mgr.note[i].phase_shift += (env.pitch_bend - 1) * info.time_step;
					process_note_sample(info, voice_mgr.note[i]);
					if (!status.pressed_notes || voice_mgr.note[status.latest_note_index].start_time < voice_mgr.note[i].start_time) {
						status.latest_note_index = i;
					}
				}
			}
		}
		this->status = status;
		//Static sample
		process_sample(info);
	}

	virtual void process_sample(const SampleInfo& info) {

	}

	virtual void process_note_sample(const SampleInfo& info, V& voice, size_t note_index) = 0;

	virtual void recieve_midi(MidiMessage& message, const SampleInfo& info) {
		switch (message.type) {
		case MessageType::NOTE_ON:
			this->press_note(info, message.note(), message.velocity()/127.0);
			break;
		case MessageType::NOTE_OFF:
			this->release_note(info, message.note(), message.velocity()/127.0);
			break;
		case MessageType::CONTROL_CHANGE:
			this->control_change(message.control(), message.value());
			break;
		default:
			break;
		}
	}

	virtual void press_note(const SampleInfo& info, unsigned int note, double velocity) {
		voice_mgr.press_note(info, note, note, velocity, 0); //FIXME parameters
	}

	virtual void release_note(const SampleInfo& info, unsigned int note, double velocity) {
		voice_mgr.release_note(info, note); //FIXME parameters
	}

	virtual void control_change(unsigned int cc, unsigned int value) {

	}

	virtual bool note_finished(const SampleInfo& info, V& note, size_t note_index) {
		const KeyboardEnvironment& env = get_host().get_environment();
		return !note.pressed || (env.sustain && note.release_time >= env.sustain_time);
	};

	virtual ~SoundEngine() {

	};

};



#endif /* MIDICUBE_FRAMEWORK_CORE_PLUGINS_SOUNDENGINE_H_ */
