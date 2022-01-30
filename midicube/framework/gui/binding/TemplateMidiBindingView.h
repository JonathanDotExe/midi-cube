/*
 * MidiBindingView.h
 *
 *  Created on: Jul 24, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_TEMPLATEMIDIBINDINGVIEW_H_
#define MIDICUBE_GUI_VIEW_TEMPLATEMIDIBINDINGVIEW_H_

#include "../core.h"
#include "../control.h"

template<typename T>
class TemplateMidiBindingView : public ViewController {
private:
	BindableTemplateValue<T>& value;
	sf::Font font;

public:
	TemplateMidiBindingView(BindableTemplateValue<T>& val, sf::Font fo) : value(val), font(fo) {

	}

	virtual ~TemplateMidiBindingView() {

	}

	virtual Scene create(ViewHost &frame) {
		ActionHandler& handler = frame.get_action_handler();
		std::vector<Control*> controls;

		{

			//Background
			Pane* bg = new Pane(sf::Color(80, 80, 80), 0, 0, frame.get_width(), frame.get_height());
			controls.push_back(bg);

			Pane* pane = new Pane(sf::Color(120, 120, 120), 5, 5, frame.get_width() - 10, frame.get_height() - 50);
			controls.push_back(pane);

			int width = 90 * 6;
			std::vector<DragBox<int>*> boxes;
			size_t index = 0;
			//CC
			{
				Label* title = new Label("CC", font, 12, frame.get_width()/2 - width/2 + 90 * index, 200);
				controls.push_back(title);

				DragBox<unsigned int>* value = new DragBox<unsigned int>(128, 0, 128, font, 16, frame.get_width()/2 - width/2 + 90 * index, 225, 80, 40);
				value->property.bind(this->value.cc, handler);
				controls.push_back(value);
			}
			++index;
			//Bank
			{
				Label* title = new Label("Bank", font, 12, frame.get_width()/2 - width/2 + 90 * index, 200);
				controls.push_back(title);

				DragBox<unsigned int>* value = new DragBox<unsigned int>(0, 0, 16, font, 16, frame.get_width()/2 - width/2 + 90 * index, 225, 80, 40);
				value->property.bind(this->value.bank, handler);
				controls.push_back(value);
			}
			++index;
			//Type
			{
				Label* title = new Label("Type", font, 12, frame.get_width()/2 - width/2 + 90 * index, 200);
				controls.push_back(title);

				ComboBox* value = new ComboBox(0, {"CC", "Slider", "Knob", "Button", "Scene", "Mod", "Breath", "Foot SW.", "Expression", "Sustain", "Sostenuto", "Soft"}, font, 16, 0, frame.get_width()/2 - width/2 + 90 * index, 225, 80, 40);
				value->property.bind_cast(this->value.type, handler);
				controls.push_back(value);
			}
			++index;
			//Persistent
			{
				Label* title = new Label("Persistent", font, 12, frame.get_width()/2 - width/2 + 90 * index, 200);
				controls.push_back(title);

				OrganSwitch* value = new OrganSwitch(true, font, frame.get_width()/2 - width/2 + 90 * index, 225, 80, 40);
				value->property.bind(this->value.persistent, handler);
				controls.push_back(value);
			}
			++index;
			//Min
			{
				Label* title = new Label("Min", font, 12, frame.get_width()/2 - width/2 + 90 * index, 200);
				controls.push_back(title);

				DragBox<T>* value = new DragBox<T>(0, this->value.total_min, this->value.total_max, font, 16, frame.get_width()/2 - width/2 + 90 * index, 225, 80, 40);
				value->property.bind(this->value.binding_min, handler);
				controls.push_back(value);
			}
			++index;
			//Max
			{
				Label* title = new Label("Max", font, 12, frame.get_width()/2 - width/2 + 90 * index, 200);
				controls.push_back(title);

				DragBox<T>* value = new DragBox<T>(0, this->value.total_min, this->value.total_max, font, 16, frame.get_width()/2 - width/2 + 90 * index, 225, 80, 40);
				value->property.bind(this->value.binding_max, handler);
				controls.push_back(value);
			}
			++index;


			//Back Button
			Button* back = new Button("Back", font, 18, frame.get_width() - 100, frame.get_height() - 40, 100, 40);
			back->set_on_click([&frame]() {
				//Change view
				frame.menu_back();
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
