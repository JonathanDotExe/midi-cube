/*
 * MidiBindingView.h
 *
 *  Created on: Jul 24, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_BOOLEANMIDIBINDINGVIEW_H_
#define MIDICUBE_GUI_VIEW_BOOLEANMIDIBINDINGVIEW_H_

#include "../engine/core.h"
#include "../engine/control.h"
#include "resources.h"
#include "../../midicube.h"
#include "../../soundengine/soundengine.h"
#include "SoundEngineView.h"

class BooleanMidiBindingView : public ViewController {
private:
	BindableBooleanValue& value;
	std::function<ViewController*()> view_factory;

public:
	BooleanMidiBindingView(BindableBooleanValue& val, std::function<ViewController*()> f) : value(val), view_factory(f) {

	}

	virtual ~BooleanMidiBindingView() {

	}

	virtual Scene create(Frame &frame) {
		std::vector<Control*> controls;
		ActionHandler& handler = frame.cube.action_handler;

		{

			//Background
			Pane* bg = new Pane(sf::Color(80, 80, 80), 0, 0, frame.get_width(), frame.get_height());
			controls.push_back(bg);

			Pane* pane = new Pane(sf::Color(120, 120, 120), 5, 5, frame.get_width() - 10, frame.get_height() - 50);
			controls.push_back(pane);

			int width = 90 * 2;
			std::vector<DragBox<int>*> boxes;
			size_t index = 0;
			//CC
			{
				Label* title = new Label("CC", main_font, 12, frame.get_width()/2 - width/2 + 90 * index, 200);
				controls.push_back(title);

				DragBox<unsigned int>* value = new DragBox<unsigned int>(128, 0, 128, main_font, 16, frame.get_width()/2 - width/2 + 90 * index, 225, 80, 40);
				value->property.bind(this->value.cc, handler);
				controls.push_back(value);
			}
			++index;
			//Persistent
			{
				Label* title = new Label("Persistent", main_font, 12, frame.get_width()/2 - width/2 + 90 * index, 200);
				controls.push_back(title);

				OrganSwitch* value = new OrganSwitch(true, main_font, frame.get_width()/2 - width/2 + 90 * index, 225, 80, 40);
				value->property.bind(this->value.persistent, handler);
				controls.push_back(value);
			}
			++index;


			//Back Button
			Button* back = new Button("Back", main_font, 18, frame.get_width() - 100, frame.get_height() - 40, 100, 40);
			back->set_on_click([&frame, boxes, this]() {
				//Change view
				frame.change_view(view_factory());
			});
			back->rect.setFillColor(sf::Color::Yellow);
			controls.push_back(back);
		}

		return {controls};
	}

	virtual void update_properties() {

	}
};

#endif /* MIDICUBE_GUI_VIEW_BOOLEANMIDIBINDINGVIEW_H_ */