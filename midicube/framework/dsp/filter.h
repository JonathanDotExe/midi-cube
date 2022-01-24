 /*
 * filter.h
 *
 *  Created on: 26 Aug 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_FILTER_H_
#define MIDICUBE_FILTER_H_
#include <array>
#include <type_traits>
#include "synthesis.h"
#include <cmath>

#define FILTER_MIN 14
#define FILTER_MAX 44100
#define FILTER_POLES 8
const double FILTER_PROGRESS_START = log(FILTER_MIN)/log(FILTER_MAX);
const double FILTER_PROGRESS_REST = 1 - FILTER_PROGRESS_START;

inline double scale_cutoff(double prog) {
	return pow(FILTER_MAX, FILTER_PROGRESS_START + FILTER_PROGRESS_REST * prog);
}

inline double invert_scale_cutoff(double cutoff) {
	return (log(cutoff)/log(FILTER_MAX) - FILTER_PROGRESS_START)/FILTER_PROGRESS_REST;
}

inline double cutoff_to_rc(double cutoff) {
	return 1.0/(2 * M_PI * cutoff);
}

inline double rc_to_cutoff_factor(double rc, double time_step) {
	return time_step/(rc+time_step);
}

inline double rc_to_highpass_cutoff_factor(double rc, double time_step) {
	return rc/(rc+time_step);
}

inline double cutoff_to_factor(double cutoff, double time_step) {
	return rc_to_cutoff_factor(cutoff_to_rc(cutoff), time_step);
}

inline double cutoff_to_highpass_factor(double cutoff, double time_step) {
	return rc_to_highpass_cutoff_factor(cutoff_to_rc(cutoff), time_step);
}

inline double factor_to_cutoff(double cutoff, double time_step) {
	return 1/(2*M_PI*(time_step/cutoff-time_step));
}

enum FilterType {
	LP_12, LP_24, HP_12, HP_24, BP_12, BP_24, LP_6, HP_6, BP_6, LP_12_BP, LP_24_BP, LP_18, HP_18, BP_18
};

struct FilterData {
	FilterType type;
	double cutoff = 44100; //in Hz
	double resonance = 0;
};

class FilterPole {
public:
	double pole = 0;
	double last_pole = 0;

	inline void update_lp(double in, double factor);
	inline void update_hp(double in, double factor);
};

inline void FilterPole::update_lp(double in, double factor) {
	pole += factor * (in - pole);
}

inline void FilterPole::update_hp(double in, double factor) {
	pole = factor * (in - last_pole + pole);
}

class Filter {
private:
	std::array<FilterPole, FILTER_POLES> poles;

	inline void do_lowpass(double in, double c, double res, size_t poles, double time_step, size_t offset = 0) {
		double cutoff = cutoff_to_factor(c, time_step);
		double feedback = res + res/(1 - cutoff);

		double last = in + feedback * (this->poles[0].pole - this->poles[1].pole);
		for (size_t i = 0; i < poles; ++i) {
			size_t p = i + offset;
			this->poles[p].update_lp(last, cutoff);
			last = this->poles[p].pole;
		}
	}
	inline void do_highpass(double in, double c, double res, size_t poles, double time_step, size_t offset = 0) {
		double cutoff = cutoff_to_factor(c, time_step);
		double feedback = res + res/(1 - cutoff);

		double last = in + feedback * (this->poles[0].pole - this->poles[1].pole);
		for (size_t i = 0; i < poles; ++i) {
			size_t p = i + offset;
			this->poles[p].update_hp(last, cutoff);
			last = this->poles[p].pole;
		}
	}
public:
	double apply (FilterData& data, double sample, double time_step);
};

#endif /* MIDICUBE_FILTER_H_ */
