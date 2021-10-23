/*
 * control.h
 *
 *  Created on: Dec 19, 2020
 *      Author: jojo
 */

#ifndef MIDICUBE_GUI_ENGINE_CONTROL_H_
#define MIDICUBE_GUI_ENGINE_CONTROL_H_

#include "core.h"
#include "util.h"
#include "math.h"
#include <functional>

class Label : public Control {
public:
	sf::Text text;

	Label(std::string text, sf::Font& font, int text_size = 12, int x = 0, int y = 0, int width = 0, int height = 0) : Control (x, y, width, height) {
		this->text.setFont(font);
		this->text.setString(text);
		this->text.setCharacterSize(text_size);
		this->text.setFillColor(sf::Color::Black);

		update_position(x, y, width, height);
	}

	virtual void update_position(int x, int y, int width, int height);

	virtual void draw(sf::RenderWindow& window, bool selected);

	virtual ~Label() {

	}

};

class Pane : public Control {

public:
	sf::RectangleShape rect;

	Pane(sf::Color color, int x = 0, int y = 0, int width = 0, int height = 0) : Control (x, y, width, height) {
		rect.setFillColor(color);

		update_position(x, y, width, height);
	}

	virtual void update_position(int x, int y, int width, int height);

	virtual void draw(sf::RenderWindow& window, bool selected);

	virtual ~Pane() {

	}

};

class Button : public Control {
private:
	std::function<void(void)> on_click = nullptr;

public:
	sf::RectangleShape rect;
	sf::Text text;

	Button(std::string text, sf::Font& font, int text_size = 12, int x = 0, int y = 0, int width = 0, int height = 0) : Control (x, y, width, height) {
		this->text.setFont(font);
		this->text.setString(text);
		this->text.setCharacterSize(text_size);
		this->text.setFillColor(sf::Color::Black);
		rect.setFillColor(sf::Color(220, 220, 220));

		update_position(x, y, width, height);
	}

	virtual void update_position(int x, int y, int width, int height);

	virtual void draw(sf::RenderWindow& window, bool selected);

	virtual void on_mouse_action();

	void update_text(std::string text);

	SELECTABLE

	virtual ~Button() {

	}

	void set_on_click(const std::function<void(void)> &onClick = nullptr) {
		on_click = onClick;
	}
};

class Slider : public Control {

private:
	double progress;
	double min;
	double max;

	double slider_width;
	double button_height;

public:
	sf::RectangleShape slider_rect;
	sf::RectangleShape button_rect;
	sf::RectangleShape context_rect;
	sf::Text text;

	PropertyBinding<double> property;

	Slider(double value, double min, double max, sf::Font& font, int x = 0, int y = 0, int width = 0, int height = 0, double slider_width = 0.7, double button_height = 0.15) : Control (x, y, width, height) {
		this->min = min;
		this->max = max;
		this->progress = (value - min)/(max - min);

		this->slider_width = slider_width;
		this->button_height = button_height;

		this->slider_rect.setFillColor(sf::Color(50, 50, 50));
		this->button_rect.setFillColor(sf::Color(220, 220, 220));

		this->text.setFont(font);
		this->text.setFillColor(sf::Color::Black);
		this->text.setCharacterSize(12);

		this->context_rect.setFillColor(sf::Color::White);
		this->context_rect.setOutlineColor(sf::Color::Black);

		update_position(x, y, width, height);
	}

	virtual void update_position(int x, int y, int width, int height);

	virtual void draw(sf::RenderWindow& window, bool selected);

	virtual void on_mouse_drag(int x, int y, int x_motion, int y_motion);

	virtual void update_properties();

	SELECTABLE

	virtual ~Slider() {

	}

};

template <typename T>
struct DragBoxScale {
	std::function<T (double, T, T)> value;
	std::function<double (T, T, T)> progress;
};

template <typename T>
class DragBox : public Control {

private:
	double progress;
	bool hit_border = false;
	T min;
	T max;
	int temp_drag = 0;

public:
	sf::RectangleShape rect;
	sf::Text text;
	double drag_mul = 0.0025;
	T border;
	int drag_step = 2;
	std::function<std::string (T t)> to_string = [](T t) {
		return std::to_string(t);
	};
	DragBoxScale<T> scale;
	PropertyBinding<T> property;

	DragBox(T value, T min, T max, sf::Font& font, int text_size = 12, int x = 0, int y = 0, int width = 0, int height = 0, DragBoxScale<T> scale = {
			[](double progress, T min, T max) {
				return progress * (max - min) + min;
			},
			[](T value, T min, T max) {
				return ((double) value - min)/(max - min);
			}
		}) : Control (x, y, width, height) {
		this->scale = scale;
		this->min = min;
		this->max = max;
		border = min;
		this->progress = this->scale.progress(value, min, max);

		this->rect.setFillColor(sf::Color(220, 220, 220));

		this->text.setFont(font);
		this->text.setFillColor(sf::Color::Black);
		this->text.setCharacterSize(text_size);

		update_position(x, y, width, height);
	}

	virtual void update_position(int x, int y, int width, int height) {
		Control::update_position(x, y, width, height);
		rect.setPosition(x + get_host()->get_x_offset(), y + get_host()->get_y_offset());
		rect.setSize(sf::Vector2<float>(width, height));
		T value = scale.value(progress, min, max);
		text.setString(to_string(value));
		center_text(text, x + get_host()->get_x_offset(), y + get_host()->get_y_offset(), width, height);
	}

	virtual void draw(sf::RenderWindow& window, bool selected) {
		window.draw(rect);
		window.draw(text);
	}

	virtual Control* on_mouse_pressed(int x, int y, sf::Mouse::Button button) {
		hit_border = false;
		temp_drag = 0;
		return Control::on_mouse_pressed(x, y, button);
	}

	virtual void on_mouse_drag(int x, int y, int x_motion, int y_motion) {
		if (!hit_border) {
			temp_drag += x_motion;
			if (abs(temp_drag) >= drag_step) {
				T old_val = scale.value(progress, min, max);
				progress += drag_mul * ((int) temp_drag/drag_step);
				temp_drag = temp_drag%drag_step;

				if (progress < 0) {
					progress = 0;
				}
				else if (progress > 1) {
					progress = 1;
				}

				T value = scale.value(progress, min, max);
				if ((old_val > border && value < border) || (old_val < border && value > border)) {
					value = border;
					progress = scale.progress(value, min, max);
					hit_border = true;
				}
				if (old_val != value) {
					if (property.is_bound()) {
						property.set(value);
					}
					update_position(get_x(), get_y(), get_width(), get_height());
				}
			}
		}
	}

	T get_value() {
		return scale.value(progress, min, max);
	}

	SELECTABLE

	virtual ~DragBox() {

	}

protected:
	virtual void update_properties() {
		if (property.is_bound()) {
			property.get([this](T v) {
				progress = fmin(fmax(scale.progress(v, min, max), 0), 1);
				update_position(get_x(), get_y(), get_width(), get_height());
			});

		}
	}

};

class CheckBox : public Control {
private:
	bool checked = false;

public:
	sf::RectangleShape rect;
	sf::RectangleShape inner_rect;
	sf::Text text;
	PropertyBinding<double> property;

	CheckBox(bool checked, std::string text, sf::Font& font, int text_size = 12, int x = 0, int y = 0, int width = 0, int height = 0) : Control (x, y, width, height) {
		this->checked = checked;

		this->text.setFont(font);
		this->text.setString(text);
		this->text.setCharacterSize(text_size);
		this->text.setFillColor(sf::Color::Black);

		rect.setFillColor(sf::Color(220, 220, 220));
		inner_rect.setFillColor(sf::Color(50, 50, 50));

		update_position(x, y, width, height);
	}

	virtual void update_position(int x, int y, int width, int height);

	virtual void draw(sf::RenderWindow& window, bool selected);

	virtual void on_mouse_action();

	virtual void update_properties();

	SELECTABLE

	virtual ~CheckBox() {

	}

};

class ComboBox : public Control {
private:
	int start_val = 0;
	int index = 0;
	std::vector<std::string> values;

public:
	sf::RectangleShape rect;
	sf::Text text;
	PropertyBinding<int> property;

	ComboBox(int value, std::vector<std::string> values, sf::Font& font, int text_size = 12, int start_val = 0, int x = 0, int y = 0, int width = 0, int height = 0) : Control (x, y, width, height) {
		this->start_val = start_val;
		this->index = fmax(0, value - start_val);
		this->values = values;

		this->text.setFont(font);
		this->text.setCharacterSize(text_size);
		this->text.setFillColor(sf::Color::Black);
		rect.setFillColor(sf::Color(220, 220, 220));

		update_position(x, y, width, height);
	}

	int get_value() {
		return index + start_val;
	}

	virtual void update_position(int x, int y, int width, int height);

	virtual void draw(sf::RenderWindow& window, bool selected);

	virtual void on_mouse_action();

	virtual void update_properties();

	SELECTABLE

	virtual ~ComboBox() {

	}
};

class Drawbar : public Control {

private:
	double progress;

	double slider_width;
	int button_height;
	int max;

public:
	sf::RectangleShape slider_rect;
	sf::RectangleShape button_rect;
	sf::Text title_text;
	sf::Text text;
	PropertyBinding<unsigned int> property;

	Drawbar(int value, int max, sf::Font& font, std::string title, int x = 0, int y = 0, int width = 0, int height = 0, sf::Color button_color = sf::Color::White, double slider_width = 0.7, int button_height = 60) : Control (x, y, width, height) {
		this->progress = (double) value/max;

		this->slider_width = slider_width;
		this->button_height = button_height;

		this->slider_rect.setFillColor(sf::Color::Black);
		this->slider_rect.setOutlineColor(sf::Color::White);
		this->slider_rect.setOutlineThickness(2);
		this->button_rect.setFillColor(button_color);
		this->button_rect.setOutlineColor(sf::Color::Black);
		this->button_rect.setOutlineThickness(2);

		this->title_text.setFont(font);
		this->title_text.setFillColor(sf::Color::Black);
		this->title_text.setCharacterSize(24);
		this->title_text.setString(title);

		this->text.setFont(font);
		this->text.setFillColor(sf::Color::Black);
		this->text.setCharacterSize(24);

		update_position(x, y, width, height);
	}

	virtual void update_position(int x, int y, int width, int height) {
		Control::update_position(x, y, width, height);
		int value = max * progress;
		double real_prog = (double) value/max;
		//Slider
		double range = height - button_height;
		slider_rect.setPosition(x  + get_host()->get_x_offset() + width/2 - width * slider_width / 2, y + get_host()->get_y_offset());
		slider_rect.setSize(sf::Vector2<float>(width * slider_width, real_prog * range - 2));
		//Button
		button_rect.setPosition(x + get_host()->get_x_offset(), y + get_host()->get_y_offset() + real_prog * range);
		button_rect.setSize(sf::Vector2<float>(width, button_height));

		//Value Text
		text.setString(std::to_string(value));
		center_text_top(text, x + get_host()->get_x_offset(), y + get_host()->get_y_offset() + height + 5, width, height);

		//Title Text
		center_text_top(title_text, x + get_host()->get_x_offset(), y + get_host()->get_y_offset() - 40, width, 40);
	}

	virtual void draw(sf::RenderWindow& window, bool selected) {
		window.draw(slider_rect);
		window.draw(button_rect);
		window.draw(text);
		window.draw(title_text);
	}

	virtual void on_mouse_drag(int x, int y, int x_motion, int y_motion) {
		int old_val = max * progress;
		progress += (double)y_motion/get_height();

		if (progress < 0) {
			progress = 0;
		}
		else if (progress > 1) {
			progress = 1;
		}

		int value = max * progress;
		if (old_val != value) {
			if (property.is_bound()) {
				property.set(value);
			}
			update_position(this->get_x(), this->get_y(), get_width(), get_height());
		}
	}

	virtual void update_properties() {
		if (property.is_bound()) {
			property.get([this](int v) {
				progress = (double) v / max;
				update_position(this->get_x(), this->get_y(), get_width(), get_height());
			});
		}
	}

	SELECTABLE

	virtual ~Drawbar() {

	}

};

class OrganSwitch : public Control {
private:
	bool checked = false;

public:
	sf::RectangleShape activated_rect;
	sf::RectangleShape deactivated_rect;
	sf::Text on_text;
	sf::Text off_text;
	PropertyBinding<bool> property;

	OrganSwitch(bool checked, sf::Font& font, int x = 0, int y = 0, int width = 0, int height = 0, std::string on_text="On", std::string off_text="Off", int switch_text_size = 12) : Control (x, y, width, height) {
		this->checked = checked;

		activated_rect.setFillColor(sf::Color::White);
		activated_rect.setOutlineColor(sf::Color::Black);
		activated_rect.setOutlineThickness(2);
		deactivated_rect.setFillColor(sf::Color(180, 180, 140));
		deactivated_rect.setOutlineColor(sf::Color::Black);
		deactivated_rect.setOutlineThickness(2);

		this->on_text.setFont(font);
		this->on_text.setString(on_text);
		this->on_text.setCharacterSize(switch_text_size);
		this->on_text.setFillColor(sf::Color::Black);

		this->off_text.setFont(font);
		this->off_text.setString(off_text);
		this->off_text.setCharacterSize(switch_text_size);
		this->off_text.setFillColor(sf::Color::Black);

		update_position(x, y, width, height);
	}

	virtual void update_position(int x, int y, int width, int height);

	virtual void draw(sf::RenderWindow& window, bool selected);

	virtual void on_mouse_action();

	SELECTABLE

	virtual void update_properties();

	virtual ~OrganSwitch() {

	}

};

class ViewContainer : public Control, public ViewHost {
public:

	ViewContainer(int x = 0, int y = 0, int width = 0, int height = 0) : Control (x, y, width, height), ViewHost() {

	}

	virtual int get_width() const;
	virtual void change_view(ViewController *view);
	virtual int get_y_offset() const;
	virtual int get_x_offset() const;
	virtual std::vector<Control*, std::allocator<Control*> > get_controls();
	virtual void request_redraw();
	virtual void add_control(Control *control);
	virtual int get_height() const;
	virtual void close();
	virtual void on_mouse_released(int x, int y, sf::Mouse::Button button);
	virtual void on_mouse_action();
	virtual bool selectable() const;
	virtual void update_position(int x, int y, int width, int height);
	virtual Control* on_mouse_pressed(int x, int y, sf::Mouse::Button button);
	virtual void update_properties();
	virtual void init(ViewHost *host);
	virtual void draw(sf::RenderWindow &window, bool selected);
	virtual void on_mouse_drag(int x, int y, int x_motion, int y_motion);
};


#endif /* MIDICUBE_GUI_ENGINE_CONTROL_H_ */
