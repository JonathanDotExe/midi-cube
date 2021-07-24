/*
 * MidiBindingView.h
 *
 *  Created on: Jul 24, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_TEMPLATEMIDIBINDINGVIEW_H_
#define MIDICUBE_GUI_VIEW_TEMPLATEMIDIBINDINGVIEW_H_

#include "../engine/core.h"
#include "../engine/control.h"
#include "resources.h"
#include "../../midicube.h"
#include "../../soundengine/soundengine.h"

template<typename T>
class TemplateMidiBindingView : public ViewController {
private:
	BindableTemplateValue<T>& value;

public:
	TemplateMidiBindingView(BindableTemplateValue<T>& val) : value(val) {

	}

	virtual ~TemplateMidiBindingView() {

	}

	virtual Scene create(Frame &frame) {
		std::vector<Control*> controls;

		{

			//Background
			Pane* bg = new Pane(sf::Color(80, 80, 80), 0, 0, frame.get_width(), frame.get_height());
			controls.push_back(bg);

			Pane* pane = new Pane(sf::Color(120, 120, 120), 5, 5, frame.get_width() - 10, frame.get_height() - 50);
			controls.push_back(pane);


			//Rename
			int width = PROGRAM_NAME_LENGTH * 45;
			std::vector<DragBox<int>*> boxes;
			for (size_t i = 0; i < PROGRAM_NAME_LENGTH; ++i) {
				size_t index = 0;
				if (i < name.size()) {
					char ch = name.at(i);
					auto pos = std::find(chars.begin(), chars.end(), ch);
					if (pos != chars.end()) {
						index = pos - chars.begin();
					}
				}

				DragBox<int>* box = new DragBox<int>(index, 0, chars.size() - 1, main_font, 30, 0, 0, 0, 0);
				box->drag_step = 4;
				box->to_string = [this](int i) {
					return std::string{chars.at(i)};
				};
				box->update_position(frame.get_width()/2 - width/2 + 45 * i, 150, 40, 40);
				boxes.push_back(box);
				controls.push_back(box);
			}

			//Back Button
			Button* back = new Button("Back", main_font, 18, frame.get_width() - 100, frame.get_height() - 40, 100, 40);
			back->set_on_click([&frame, boxes, this]() {
				//Change view
				frame.change_view(new SoundEngineView());
			});
			back->rect.setFillColor(sf::Color::Yellow);
			controls.push_back(back);
		}

		return {controls};
	}

	virtual void update_properties() {

	}
};

#endif /* MIDICUBE_GUI_VIEW_TEMPLATEMIDIBINDINGVIEW_H_ */
