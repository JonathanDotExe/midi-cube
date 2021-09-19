/*
 * audio.h
 *
 *  Created on: Sep 19, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_FRAMEWORK_CORE_AUDIO_H_
#define MIDICUBE_FRAMEWORK_CORE_AUDIO_H_


struct SampleInfo {
	double time;
	double time_step;
	unsigned int sample_rate;
	unsigned int sample_time;
	double input_sample;
};


#endif /* MIDICUBE_FRAMEWORK_CORE_AUDIO_H_ */
