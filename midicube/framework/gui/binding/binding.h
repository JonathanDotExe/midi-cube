/*
 * binding.h
 *
 *  Created on: Jul 25, 2021
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_VIEW_BINDING_H_
#define MIDICUBE_GUI_VIEW_BINDING_H_

#include "../control.h"
#include "../../core/plugin.h"
#include "BooleanMidiBindingView.h"
#include "TemplateMidiBindingView.h"

class BindingGUIHandler {
private:
	Button* button = nullptr;
	bool edit = false;
	ViewHost* frame = nullptr;
	sf::Font font;

public:

	BindingGUIHandler(sf::Font fo) : font(fo) {

	}

	bool on_action(Control* control) {
		sf::Font f = font;
		if (edit) {
			if (dynamic_cast<OrganSwitch*>(control)) {
				OrganSwitch* cast = dynamic_cast<OrganSwitch*>(control);
				BindableBooleanValue* val = dynamic_cast<BindableBooleanValue*> (cast->property.get_object());
				if (val) {
					frame->change_menu(VIEW_MENU(new BooleanMidiBindingView(*val, f), val, f));
				}
			}
			else if (dynamic_cast<CheckBox*>(control)) {
				CheckBox* cast = dynamic_cast<CheckBox*>(control);
				BindableBooleanValue* val = dynamic_cast<BindableBooleanValue*> (cast->property.get_object());
				if (val) {
					frame->change_menu(VIEW_MENU(new BooleanMidiBindingView(*val, f), val, f));
				}
			}
			else if (dynamic_cast<DragBox<double>*>(control)) {
				DragBox<double>* cast = dynamic_cast<DragBox<double>*>(control);
				BindableTemplateValue<double>* val = dynamic_cast<BindableTemplateValue<double>*> (cast->property.get_object());
				if (val) {
					frame->change_menu(VIEW_MENU(new TemplateMidiBindingView<double>(*val, f), val, f));
				}
			}
			else if (dynamic_cast<DragBox<int>*>(control)) {
				DragBox<int>* cast = dynamic_cast<DragBox<int>*>(control);
				BindableTemplateValue<int>* val = dynamic_cast<BindableTemplateValue<int>*> (cast->property.get_object());
				if (val) {
					frame->change_menu(VIEW_MENU(new TemplateMidiBindingView<int>(*val, f), val, f));
				}
			}
			else if (dynamic_cast<DragBox<unsigned int>*>(control)) {
				DragBox<unsigned int>* cast = dynamic_cast<DragBox<unsigned int>*>(control);
				BindableTemplateValue<unsigned int>* val = dynamic_cast<BindableTemplateValue<unsigned int>*> (cast->property.get_object());
				if (val) {
					frame->change_menu(VIEW_MENU(new TemplateMidiBindingView<unsigned int>(*val, f), val, f));
				}
			}
			else if (dynamic_cast<Drawbar*>(control)) {
				Drawbar* cast = dynamic_cast<Drawbar*>(control);
				BindableTemplateValue<unsigned int>* val = dynamic_cast<BindableTemplateValue<unsigned int>*> (cast->property.get_object());
				if (val) {
					frame->change_menu(VIEW_MENU(new TemplateMidiBindingView<unsigned int>(*val, f), val, f));
				}
			}
			else if (dynamic_cast<Slider*>(control)) {
				Slider* cast = dynamic_cast<Slider*>(control);
				BindableTemplateValue<double>* val = dynamic_cast<BindableTemplateValue<double>*> (cast->property.get_object());
				if (val) {
					frame->change_menu(VIEW_MENU(new TemplateMidiBindingView<double>(*val, f), val, f));
				}
			}
			else {
				return true;
			}
			return false;
		}

		return true;
	}

	Button* create_button(int x, int y, ViewHost* frame) {
		if (this->button) {
			throw "Button can only be created once!";
		}
		this->frame = frame;
		//Exit Button
		button = new Button("MIDI", font, 18, x, y, 100, 40);
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
