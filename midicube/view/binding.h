/*
 * binding.h
 *
 *  Created on: Jul 25, 2021
 *      Author: jojo
 */

#include "../engine/control.h"
#include "../../soundengine/organ.h"
#include "../view/BooleanMidiBindingView.h"
#include "../view/resources.h"
#include "../view/TemplateMidiBindingView.h"

#ifndef MIDICUBE_GUI_VIEW_BINDING_H_
#define MIDICUBE_GUI_VIEW_BINDING_H_

class BindingGUIHandler {
private:
	Button* button = nullptr;
	bool edit = false;
	Frame* frame = nullptr;
	std::function<ViewController*()> view_factory;

public:

	BindingGUIHandler(std::function<ViewController*()> f) : view_factory(f) {

	}

	bool on_action(Control* control) {
		if (edit) {
			if (dynamic_cast<OrganSwitch*>(control)) {
				OrganSwitch* cast = dynamic_cast<OrganSwitch*>(control);
				BindableBooleanValue* val = dynamic_cast<BindableBooleanValue*> (cast->property.get_object());
				if (val) {
					frame->change_view(new BooleanMidiBindingView(*val, view_factory));
				}
			}
			else if (dynamic_cast<CheckBox*>(control)) {
				CheckBox* cast = dynamic_cast<CheckBox*>(control);
				BindableBooleanValue* val = dynamic_cast<BindableBooleanValue*> (cast->property.get_object());
				if (val) {
					frame->change_view(new BooleanMidiBindingView(*val, view_factory));
				}
			}
			else if (dynamic_cast<DragBox<double>*>(control)) {
				DragBox<double>* cast = dynamic_cast<DragBox<double>*>(control);
				BindableTemplateValue<double>* val = dynamic_cast<BindableTemplateValue<double>*> (cast->property.get_object());
				if (val) {
					frame->change_view(new TemplateMidiBindingView<double>(*val, view_factory));
				}
			}
			else if (dynamic_cast<DragBox<int>*>(control)) {
				DragBox<int>* cast = dynamic_cast<DragBox<int>*>(control);
				BindableTemplateValue<int>* val = dynamic_cast<BindableTemplateValue<int>*> (cast->property.get_object());
				if (val) {
					frame->change_view(new TemplateMidiBindingView<int>(*val, view_factory));
				}
			}
			else if (dynamic_cast<DragBox<unsigned int>*>(control)) {
				DragBox<unsigned int>* cast = dynamic_cast<DragBox<unsigned int>*>(control);
				BindableTemplateValue<unsigned int>* val = dynamic_cast<BindableTemplateValue<unsigned int>*> (cast->property.get_object());
				if (val) {
					frame->change_view(new TemplateMidiBindingView<unsigned int>(*val, view_factory));
				}
			}
			else if (dynamic_cast<Drawbar<ORGAN_DRAWBAR_MAX>*>(control)) {
				Drawbar<ORGAN_DRAWBAR_MAX>* cast = dynamic_cast<Drawbar<ORGAN_DRAWBAR_MAX>*>(control);
				BindableTemplateValue<unsigned int>* val = dynamic_cast<BindableTemplateValue<unsigned int>*> (cast->property.get_object());
				if (val) {
					frame->change_view(new TemplateMidiBindingView<unsigned int>(*val, view_factory));
				}
			}
			else if (dynamic_cast<Slider*>(control)) {
				Slider* cast = dynamic_cast<Slider*>(control);
				BindableTemplateValue<double>* val = dynamic_cast<BindableTemplateValue<double>*> (cast->property.get_object());
				if (val) {
					frame->change_view(new TemplateMidiBindingView<double>(*val, view_factory));
				}
			}
			else {
				return true;
			}
			return false;
		}

		return true;
	}

	Button* create_button(int x, int y, Frame* frame) {
		if (button) {
			throw "Button can only be created once!";
		}
		this->frame = frame;
		//Exit Button
		button = new Button("MIDI", main_font, 18, x, y, 100, 40);
		button->set_on_click([this]() {
			edit = !edit;
			if (edit) {
				button->rect.setFillColor(sf::Color::White);
			}
			else {
				button->rect.setFillColor(sf::Color::Yellow);
			}
			this->frame->request_redraw();
		});
		if (edit) {
			button->rect.setFillColor(sf::Color::White);
		}
		else {
			button->rect.setFillColor(sf::Color::Yellow);
		}

		return button;
	}
};

#endif
