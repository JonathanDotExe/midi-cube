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
#include "../soundengine/sampler.h"

class SamplerView: public ViewController {
	private:
	Sampler& sampler;
public:
	SamplerView(Sampler& s);
	virtual Scene create(ViewHost &frame);
	virtual ~SamplerView();
};

#endif /* MIDICUBE_GUI_VIEW_SAMPLERVIEW_H_ */
