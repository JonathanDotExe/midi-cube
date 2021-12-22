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
		//Notes
		for (size_t i = 0; i < P; ++i) {
			if (voice_mgr.note[i].valid) {
				if (note_finished(info, voice_mgr.note[i], i)) {
					voice_mgr.note[i].valid = false;
				}
				else {
					++status.pressed_notes;
					voice_mgr.note[i].phase_shift += (host_environment.pitch_bend - 1) * info.time_step;
					process_note_sample(info, voice_mgr.note[i], i);
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

	virtual bool keep_active() {
		return status.pressed_notes != 0;
	}

	virtual void process_note_sample(const SampleInfo& info, V& voice, size_t note_index) = 0;

	virtual void recieve_midi(const MidiMessage& message, const SampleInfo& info) {
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
		voice_mgr.press_note(info, note, note + host.get_transpose(), velocity, 0);
	}

	virtual void release_note(const SampleInfo& info, unsigned int note, double velocity) {
		voice_mgr.release_note(info, note);
	}

	virtual void control_change(unsigned int cc, unsigned int value) {

	}

	virtual bool note_finished(const SampleInfo& info, V& note, size_t note_index) {
		return !note.pressed || (host_environment.sustain && note.release_time >= host_environment.sustain_time);
	};

	virtual ~SoundEngine() {

	}
	const EngineStatus& get_status() const {
		return status;
	}

};



#endif /* MIDICUBE_FRAMEWORK_CORE_PLUGINS_SOUNDENGINE_H_ */
