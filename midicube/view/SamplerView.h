/*
 * SamplerView.h
 *
 *  Created on: Apr 2, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_SAMPLERVIEW_H_
#define MIDICUBE_GUI_VIEW_SAMPLERVIEW_H_

#include "../framework/gui/core.h"
#include "../framework/gui/control.h"
#include "../../soundengine/sampler.h"

class SamplerView: public ViewController {
	private:
	Sampler& sampler;
	SoundEngineChannel& channel;
	int channel_index;
public:
	SamplerView(Sampler& s, SoundEngineChannel& c, int channel_index);
	virtual Scene create(Frame &frame);
	virtual ~SamplerView();
};

#endif /* MIDICUBE_GUI_VIEW_SAMPLERVIEW_H_ */
