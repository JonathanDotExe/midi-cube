/*
 * effect.cpp
 *
 *  Created on: Mar 30, 2021
 *      Author: jojo
 */

#include "effect.h"
#include "../soundengine/soundengine.h"

void Effect::init(SoundEngineDevice &engine) {
	cc.init(&engine.binding_handler);
}
