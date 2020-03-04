#include "window.h"
#include "core/os/keyboard.h"
#include "scene/resources/dynamic_font.h"

void Window::set_title(const String &p_title) {
	title = p_title;
	if (window_id == DisplayServer::INVALID_WINDOW_ID)
		return;
	DisplayServer::get_singleton()->window_set_title(p_title, window_id);
}
String Window::get_title() const {
	return title;
}

void Window::set_current_screen(int p_screen) {
	current_screen = p_screen;
	if (window_id == DisplayServer::INVALID_WINDOW_ID)
		return;
	DisplayServer::get_singleton()->window_set_current_screen(p_screen, window_id);
}
int Window::get_current_screen() const {
	if (window_id != DisplayServer::INVALID_WINDOW_ID) {
		current_screen = DisplayServer::get_singleton()->window_get_current_screen(window_id);
	}
	return current_screen;
}

void Window::set_position(const Point2i &p_position) {

	position = p_position;
	if (window_id == DisplayServer::INVALID_WINDOW_ID)
		return;
	DisplayServer::get_singleton()->window_set_position(p_position, window_id);
}
Point2i Window::get_position() const {
	if (window_id != DisplayServer::INVALID_WINDOW_ID) {
		position = DisplayServer::get_singleton()->window_get_position(window_id);
	}
	return position;
}

void Window::set_size(const Size2i &p_size) {
	size = p_size;
	if (window_id == DisplayServer::INVALID_WINDOW_ID) {
		DisplayServer::get_singleton()->window_set_size(p_size, window_id);
	}
	_update_size();
}
Size2i Window::get_size() const {
	if (window_id != DisplayServer::INVALID_WINDOW_ID) {
		size = DisplayServer::get_singleton()->window_get_size(window_id);
	}
	return size;
}

Size2i Window::get_real_size() const {

	if (window_id != DisplayServer::INVALID_WINDOW_ID) {
		return DisplayServer::get_singleton()->window_get_real_size(window_id);
	}
	return size;
}

void Window::set_max_size(const Size2i &p_max_size) {
	max_size = p_max_size;
	if (window_id == DisplayServer::INVALID_WINDOW_ID)
		return;
	DisplayServer::get_singleton()->window_set_max_size(p_max_size, window_id);
}
Size2i Window::get_max_size() const {
	if (window_id != DisplayServer::INVALID_WINDOW_ID) {
		max_size = DisplayServer::get_singleton()->window_get_max_size(window_id);
	}
	return max_size;
}

void Window::set_min_size(const Size2i &p_min_size) {
	min_size = p_min_size;
	if (window_id == DisplayServer::INVALID_WINDOW_ID)
		return;
	DisplayServer::get_singleton()->window_set_min_size(p_min_size, window_id);
}
Size2i Window::get_min_size() const {
	if (window_id != DisplayServer::INVALID_WINDOW_ID) {
		min_size = DisplayServer::get_singleton()->window_get_min_size(window_id);
	}
	return min_size;
}

void Window::set_mode(Mode p_mode) {

	mode = p_mode;
	if (window_id == DisplayServer::INVALID_WINDOW_ID)
		return;
	DisplayServer::get_singleton()->window_set_mode(DisplayServer::WindowMode(p_mode), window_id);
}

Window::Mode Window::get_mode() const {

	if (window_id != DisplayServer::INVALID_WINDOW_ID) {
		mode = (Mode)DisplayServer::get_singleton()->window_get_mode(window_id);
	}
	return mode;
}

void Window::set_flag(Flags p_flag, bool p_enabled) {
	ERR_FAIL_INDEX(p_flag, FLAG_MAX);
	flags[p_flag] = p_enabled;
	if (window_id == DisplayServer::INVALID_WINDOW_ID)
		return;
	DisplayServer::get_singleton()->window_set_flag(DisplayServer::WindowFlags(p_flag), p_enabled, window_id);
}

bool Window::get_flag(Flags p_flag) const {
	ERR_FAIL_INDEX_V(p_flag, FLAG_MAX, false);
	if (window_id != DisplayServer::INVALID_WINDOW_ID) {
		flags[p_flag] = DisplayServer::get_singleton()->window_get_flag(DisplayServer::WindowFlags(p_flag), window_id);
	}
	return flags[p_flag];
}

bool Window::is_maximize_allowed() const {
	if (window_id != DisplayServer::INVALID_WINDOW_ID) {
		return DisplayServer::get_singleton()->window_is_maximize_allowed(window_id);
	}
	return true;
}

void Window::request_attention() {
	if (window_id != DisplayServer::INVALID_WINDOW_ID) {
		DisplayServer::get_singleton()->window_request_attention(window_id);
	}
}
void Window::move_to_foreground() {
	if (window_id != DisplayServer::INVALID_WINDOW_ID) {
		DisplayServer::get_singleton()->window_move_to_foreground(window_id);
	}
}

bool Window::can_draw() const {
	if (!is_inside_tree()) {
		return false;
	}
	if (window_id != DisplayServer::INVALID_WINDOW_ID) {
		return DisplayServer::get_singleton()->window_can_draw(window_id);
	}

	return true;
}

void Window::set_ime_active(bool p_active) {
	if (window_id != DisplayServer::INVALID_WINDOW_ID) {
		DisplayServer::get_singleton()->window_set_ime_active(p_active, window_id);
	}
}
void Window::set_ime_position(const Point2i &p_pos) {
	if (window_id != DisplayServer::INVALID_WINDOW_ID) {
		DisplayServer::get_singleton()->window_set_ime_position(p_pos, window_id);
	}
}

bool Window::is_embedded() const {
	ERR_FAIL_COND_V(!is_inside_tree(), false);
	if (get_parent_viewport()) {
		return get_parent_viewport()->is_embedding_subwindows();
	} else {
		return false;
	}
}

void Window::_make_window() {
	ERR_FAIL_COND(window_id != DisplayServer::INVALID_WINDOW_ID);

	uint32_t f = 0;
	for (int i = 0; i < FLAG_MAX; i++) {
		if (flags[i]) {
			f |= (1 << i);
		}
	}
	window_id = DisplayServer::get_singleton()->create_sub_window(DisplayServer::WindowMode(mode), f, Rect2i(position, size));
	ERR_FAIL_COND(window_id == DisplayServer::INVALID_WINDOW_ID);
	DisplayServer::get_singleton()->window_set_current_screen(current_screen, window_id);
	DisplayServer::get_singleton()->window_set_max_size(max_size, window_id);
	DisplayServer::get_singleton()->window_set_min_size(min_size, window_id);
	DisplayServer::get_singleton()->window_set_title(title, window_id);

	_update_size();

	if (transient_parent && transient_parent->window_id != DisplayServer::INVALID_WINDOW_ID) {
		DisplayServer::get_singleton()->window_set_transient(window_id, transient_parent->window_id);
	}

	for (Set<Window *>::Element *E = transient_children.front(); E; E = E->next()) {
		if (E->get()->window_id != DisplayServer::INVALID_WINDOW_ID) {
			DisplayServer::get_singleton()->window_set_transient(E->get()->window_id, transient_parent->window_id);
		}
	}
}
void Window::_update_from_window() {

	ERR_FAIL_COND(window_id == DisplayServer::INVALID_WINDOW_ID);
	mode = (Mode)DisplayServer::get_singleton()->window_get_mode(window_id);
	for (int i = 0; i < FLAG_MAX; i++) {
		flags[i] = DisplayServer::get_singleton()->window_get_flag(DisplayServer::WindowFlags(i), window_id);
	}
	position = DisplayServer::get_singleton()->window_get_position(window_id);
	size = DisplayServer::get_singleton()->window_get_size(window_id);
	max_size = DisplayServer::get_singleton()->window_get_max_size(window_id);
	min_size = DisplayServer::get_singleton()->window_get_min_size(window_id);
}

void Window::_clear_window() {
	ERR_FAIL_COND(window_id == DisplayServer::INVALID_WINDOW_ID);

	if (transient_parent && transient_parent->window_id != DisplayServer::INVALID_WINDOW_ID) {
		DisplayServer::get_singleton()->window_set_transient(window_id, DisplayServer::INVALID_WINDOW_ID);
	}

	for (Set<Window *>::Element *E = transient_children.front(); E; E = E->next()) {
		if (E->get()->window_id != DisplayServer::INVALID_WINDOW_ID) {
			DisplayServer::get_singleton()->window_set_transient(E->get()->window_id, DisplayServer::INVALID_WINDOW_ID);
		}
	}

	_update_from_window();
	DisplayServer::get_singleton()->delete_sub_window(window_id);
	window_id = DisplayServer::INVALID_WINDOW_ID;
	_update_size();
}

void Window::_resize_callback(const Size2i &p_callback) {
	size = p_callback;
	_update_size();
}

void Window::_propagate_window_notification(Node *p_node, int p_notification) {
	p_node->notification(p_notification);
	for (int i = 0; i < p_node->get_child_count(); i++) {
		Node *child = p_node->get_child(i);
		Window *window = Object::cast_to<Window>(child);
		if (window) {
			break;
		}
		_propagate_window_notification(child, p_notification);
	}
}

void Window::_event_callback(DisplayServer::WindowEvent p_event) {

	switch (p_event) {
		case DisplayServer::WINDOW_EVENT_MOUSE_ENTER: {
			_propagate_window_notification(this, NOTIFICATION_WM_MOUSE_ENTER);
			emit_signal("mouse_entered");
		} break;
		case DisplayServer::WINDOW_EVENT_MOUSE_EXIT: {
			_propagate_window_notification(this, NOTIFICATION_WM_MOUSE_EXIT);
			emit_signal("mouse_exited");
		} break;
		case DisplayServer::WINDOW_EVENT_FOCUS_IN: {
			_propagate_window_notification(this, NOTIFICATION_WM_FOCUS_IN);
			emit_signal("focus_entered");
		} break;
		case DisplayServer::WINDOW_EVENT_FOCUS_OUT: {
			_propagate_window_notification(this, NOTIFICATION_WM_FOCUS_OUT);
			emit_signal("focus_exited");
		} break;
		case DisplayServer::WINDOW_EVENT_CLOSE_REQUEST: {
			_propagate_window_notification(this, NOTIFICATION_WM_CLOSE_REQUEST);
			emit_signal("close_requested");
		} break;
		case DisplayServer::WINDOW_EVENT_GO_BACK_REQUEST: {
			_propagate_window_notification(this, NOTIFICATION_WM_GO_BACK_REQUEST);
			emit_signal("go_back_requested");
		} break;
	}
}

void Window::set_visible(bool p_visible) {
	if (visible == p_visible) {
		return;
	}

	if (!is_inside_tree()) {
		return;
	}

	bool subwindow = get_parent() && get_parent()->get_viewport()->is_embedding_subwindows();

	visible = p_visible;

	if (!subwindow) {
		if (p_visible && window_id != DisplayServer::INVALID_WINDOW_ID) {
			_clear_window();
		}
		if (!p_visible && window_id == DisplayServer::INVALID_WINDOW_ID) {
			_make_window();
		}
	} else {
		_update_size();
	}
}

void Window::_clear_transient() {
	if (transient_parent) {
		if (transient_parent->window_id != DisplayServer::INVALID_WINDOW_ID && window_id != DisplayServer::INVALID_WINDOW_ID) {
			DisplayServer::get_singleton()->window_set_transient(window_id, DisplayServer::INVALID_WINDOW_ID);
		}
		transient_parent->transient_children.erase(this);
		transient_parent = nullptr;
	}
}

void Window::_make_transient() {
	if (!get_parent()) {
		//main window, can't be transient
		return;
	}
	//find transient parent
	Viewport *vp = get_parent()->get_viewport();
	Window *window = nullptr;
	while (vp) {
		window = Object::cast_to<Window>(vp);
		if (window) {
			break;
		}
		if (!vp->get_parent()) {
			break;
		}

		vp = vp->get_parent()->get_viewport();
	}

	if (window) {
		transient_parent = window;
		window->transient_children.insert(this);
	}

	//see if we can make transient
	if (transient_parent->window_id != DisplayServer::INVALID_WINDOW_ID && window_id != DisplayServer::INVALID_WINDOW_ID) {
		DisplayServer::get_singleton()->window_set_transient(window_id, transient_parent->window_id);
	}
}

void Window::set_transient(bool p_transient) {
	if (transient == p_transient) {
		return;
	}

	transient = p_transient;

	if (!is_inside_tree()) {
		return;
	}

	if (transient) {
		_make_transient();
	} else {
		_clear_transient();
	}
}
bool Window::is_transient() const {
	return transient;
}

bool Window::is_visible() const {
	return visible;
}

void Window::_update_size() {

	Size2i final_size;
	Size2i final_size_override;
	Rect2i attach_to_screen_rect(Point2i(), size);
	Transform2D stretch_transform;
	float font_oversampling = 1.0;

	if (content_scale_mode == CONTENT_SCALE_MODE_DISABLED || content_scale_size.x == 0 || content_scale_size.y == 0) {

		stretch_transform = Transform2D();
		final_size = size;

	} else {

		//actual screen video mode
		Size2 video_mode = size;
		Size2 desired_res = content_scale_size;

		Size2 viewport_size;
		Size2 screen_size;

		float viewport_aspect = desired_res.aspect();
		float video_mode_aspect = video_mode.aspect();

		if (content_scale_aspect == CONTENT_SCALE_ASPECT_IGNORE || Math::is_equal_approx(viewport_aspect, video_mode_aspect)) {
			//same aspect or ignore aspect
			viewport_size = desired_res;
			screen_size = video_mode;
		} else if (viewport_aspect < video_mode_aspect) {
			// screen ratio is smaller vertically

			if (content_scale_aspect == CONTENT_SCALE_ASPECT_KEEP_HEIGHT || content_scale_aspect == CONTENT_SCALE_ASPECT_EXPAND) {

				//will stretch horizontally
				viewport_size.x = desired_res.y * video_mode_aspect;
				viewport_size.y = desired_res.y;
				screen_size = video_mode;

			} else {
				//will need black bars
				viewport_size = desired_res;
				screen_size.x = video_mode.y * viewport_aspect;
				screen_size.y = video_mode.y;
			}
		} else {
			//screen ratio is smaller horizontally
			if (content_scale_aspect == CONTENT_SCALE_ASPECT_KEEP_WIDTH || content_scale_aspect == CONTENT_SCALE_ASPECT_EXPAND) {

				//will stretch horizontally
				viewport_size.x = desired_res.x;
				viewport_size.y = desired_res.x / video_mode_aspect;
				screen_size = video_mode;

			} else {
				//will need black bars
				viewport_size = desired_res;
				screen_size.x = video_mode.x;
				screen_size.y = video_mode.x / viewport_aspect;
			}
		}

		screen_size = screen_size.floor();
		viewport_size = viewport_size.floor();

		Size2 margin;
		Size2 offset;
		//black bars and margin
		if (content_scale_aspect != CONTENT_SCALE_ASPECT_EXPAND && screen_size.x < video_mode.x) {
			margin.x = Math::round((video_mode.x - screen_size.x) / 2.0);
			//VisualServer::get_singleton()->black_bars_set_margins(margin.x, 0, margin.x, 0);
			offset.x = Math::round(margin.x * viewport_size.y / screen_size.y);
		} else if (content_scale_aspect != CONTENT_SCALE_ASPECT_EXPAND && screen_size.y < video_mode.y) {
			margin.y = Math::round((video_mode.y - screen_size.y) / 2.0);
			//VisualServer::get_singleton()->black_bars_set_margins(0, margin.y, 0, margin.y);
			offset.y = Math::round(margin.y * viewport_size.x / screen_size.x);
		} else {
			//VisualServer::get_singleton()->black_bars_set_margins(0, 0, 0, 0);
		}

		switch (content_scale_mode) {
			case CONTENT_SCALE_MODE_DISABLED: {
				// Already handled above
				//_update_font_oversampling(1.0);
			} break;
			case CONTENT_SCALE_MODE_OBJECTS: {

				final_size = screen_size;
				final_size_override = viewport_size;
				attach_to_screen_rect = Rect2(margin, screen_size);
				font_oversampling = screen_size.x / viewport_size.x;
			} break;
			case CONTENT_SCALE_MODE_PIXELS: {

				final_size = viewport_size;
				attach_to_screen_rect = Rect2(margin, screen_size);

			} break;
		}

		Size2 scale = size / (Vector2(final_size) + margin * 2);
		stretch_transform.scale(scale);
		stretch_transform.elements[2] = margin * scale;
	}

	bool allocate = is_inside_tree() && visible && (window_id != DisplayServer::INVALID_WINDOW_ID || (get_parent() && get_parent()->get_viewport()->is_embedding_subwindows()));

	_set_size(final_size, final_size_override, attach_to_screen_rect, stretch_transform, allocate);

	if (window_id != DisplayServer::INVALID_WINDOW_ID) {
		VisualServer::get_singleton()->viewport_attach_to_screen(get_viewport_rid(), attach_to_screen_rect, window_id);
	} else {
		VisualServer::get_singleton()->viewport_attach_to_screen(get_viewport_rid(), Rect2i(), DisplayServer::INVALID_WINDOW_ID);
	}

	if (window_id == DisplayServer::MAIN_WINDOW_ID) {

		if (!use_font_oversampling) {
			font_oversampling = 1.0;
		}
		if (DynamicFontAtSize::font_oversampling != font_oversampling) {

			DynamicFontAtSize::font_oversampling = font_oversampling;
			DynamicFont::update_oversampling();
		}
	}
}

void Window::_update_window_callbacks() {
	DisplayServer::get_singleton()->window_set_resize_callback(callable_mp(this, &Window::_resize_callback), window_id);
	DisplayServer::get_singleton()->window_set_window_event_callback(callable_mp(this, &Window::_event_callback), window_id);
	DisplayServer::get_singleton()->window_set_input_event_callback(callable_mp(this, &Window::_window_input), window_id);
	DisplayServer::get_singleton()->window_set_input_text_callback(callable_mp(this, &Window::_window_input_text), window_id);
	DisplayServer::get_singleton()->window_set_drop_files_callback(callable_mp(this, &Window::_window_drop_files), window_id);
}
void Window::_notification(int p_what) {
	if (p_what == NOTIFICATION_ENTER_TREE) {
		if (is_embedded()) {
			//create as embedded
			_update_size();
		} else {
			if (get_parent() == nullptr) {
				//it's the root window!
				window_id = DisplayServer::MAIN_WINDOW_ID;
				_update_from_window();
				_update_size();
				_update_window_callbacks();
			} else {
				//create
				_make_window();
				_update_window_callbacks();
			}
		}

		if (transient) {
			_make_transient();
		}
	}

	if (p_what == NOTIFICATION_EXIT_TREE) {

		if (transient) {
			_clear_transient();
		}

		if (!is_embedded() && window_id != DisplayServer::INVALID_WINDOW_ID) {

			if (window_id == DisplayServer::MAIN_WINDOW_ID) {

				_update_window_callbacks();
			} else {
				_clear_window();
			}
		} else {
			_update_size();
		}
	}
}

void Window::set_content_scale_size(const Size2i &p_size) {
	ERR_FAIL_COND(p_size.x < 0);
	ERR_FAIL_COND(p_size.y < 0);
	content_scale_size = p_size;
	_update_size();
}

Size2i Window::get_content_scale_size() const {
	return content_scale_size;
}

void Window::set_content_scale_mode(ContentScaleMode p_mode) {
	content_scale_mode = p_mode;
	_update_size();
}
Window::ContentScaleMode Window::get_content_scale_mode() const {
	return content_scale_mode;
}

void Window::set_content_scale_aspect(ContentScaleAspect p_aspect) {
	content_scale_aspect = p_aspect;
	_update_size();
}
Window::ContentScaleAspect Window::get_content_scale_aspect() const {
	return content_scale_aspect;
}

void Window::set_use_font_oversampling(bool p_oversampling) {
	if (is_inside_tree() && window_id != DisplayServer::MAIN_WINDOW_ID) {
		ERR_FAIL_MSG("Only the root window can set and use font oversampling.");
	}
	use_font_oversampling = p_oversampling;
	_update_size();
}
bool Window::is_using_font_oversampling() const {
	return use_font_oversampling;
}

DisplayServer::WindowID Window::get_window_id() const {
	return window_id;
}

void Window::_window_input(const Ref<InputEvent> &p_ev) {

	if (Engine::get_singleton()->is_editor_hint() && (Object::cast_to<InputEventJoypadButton>(p_ev.ptr()) || Object::cast_to<InputEventJoypadMotion>(*p_ev)))
		return; //avoid joy input on editor

	if (ScriptDebugger::get_singleton() && ScriptDebugger::get_singleton()->is_remote()) {
		//quit from game window using F8
		Ref<InputEventKey> k = p_ev;
		if (k.is_valid() && k->is_pressed() && !k->is_echo() && k->get_scancode() == KEY_F8) {
			ScriptDebugger::get_singleton()->request_quit();
		}
	}

	input(p_ev);
	if (!is_input_handled()) {
		unhandled_input(p_ev);
	}
}
void Window::_window_input_text(const String &p_text) {
	input_text(p_text);
}
void Window::_window_drop_files(const Vector<String> &p_files) {
	emit_signal("files_dropped", p_files);
}

void Window::_bind_methods() {

	ClassDB::bind_method(D_METHOD("set_title", "title"), &Window::set_title);
	ClassDB::bind_method(D_METHOD("get_title"), &Window::get_title);

	ClassDB::bind_method(D_METHOD("set_current_screen", "index"), &Window::set_current_screen);
	ClassDB::bind_method(D_METHOD("get_current_screen"), &Window::get_current_screen);

	ClassDB::bind_method(D_METHOD("set_position", "position"), &Window::set_position);
	ClassDB::bind_method(D_METHOD("get_position"), &Window::get_position);

	ClassDB::bind_method(D_METHOD("set_size", "size"), &Window::set_size);
	ClassDB::bind_method(D_METHOD("get_size"), &Window::get_size);

	ClassDB::bind_method(D_METHOD("get_real_size"), &Window::get_real_size);

	ClassDB::bind_method(D_METHOD("set_max_size", "max_size"), &Window::set_max_size);
	ClassDB::bind_method(D_METHOD("get_max_size"), &Window::get_max_size);

	ClassDB::bind_method(D_METHOD("set_min_size", "min_size"), &Window::set_min_size);
	ClassDB::bind_method(D_METHOD("get_min_size"), &Window::get_min_size);

	ClassDB::bind_method(D_METHOD("set_mode", "mode"), &Window::set_mode);
	ClassDB::bind_method(D_METHOD("get_mode"), &Window::get_mode);

	ClassDB::bind_method(D_METHOD("set_flag", "flag", "enabled"), &Window::set_flag);
	ClassDB::bind_method(D_METHOD("get_flag", "flag"), &Window::get_flag);

	ClassDB::bind_method(D_METHOD("is_maximize_allowed"), &Window::is_maximize_allowed);

	ClassDB::bind_method(D_METHOD("request_attention"), &Window::request_attention);

	ClassDB::bind_method(D_METHOD("move_to_foreground"), &Window::move_to_foreground);

	ClassDB::bind_method(D_METHOD("set_visible", "visible"), &Window::set_visible);
	ClassDB::bind_method(D_METHOD("is_visible"), &Window::is_visible);

	ClassDB::bind_method(D_METHOD("set_transient", "transient"), &Window::set_transient);
	ClassDB::bind_method(D_METHOD("is_transient"), &Window::is_transient);

	ClassDB::bind_method(D_METHOD("can_draw"), &Window::is_transient);

	ClassDB::bind_method(D_METHOD("set_ime_active"), &Window::set_ime_active);
	ClassDB::bind_method(D_METHOD("set_ime_position"), &Window::set_ime_position);

	ClassDB::bind_method(D_METHOD("is_embedded"), &Window::is_embedded);

	ClassDB::bind_method(D_METHOD("set_content_scale_size", "size"), &Window::set_content_scale_size);
	ClassDB::bind_method(D_METHOD("get_content_scale_size"), &Window::get_content_scale_size);

	ClassDB::bind_method(D_METHOD("set_content_scale_mode", "mode"), &Window::set_content_scale_mode);
	ClassDB::bind_method(D_METHOD("get_content_scale_mode"), &Window::get_content_scale_mode);

	ClassDB::bind_method(D_METHOD("set_content_scale_aspect", "aspect"), &Window::set_content_scale_aspect);
	ClassDB::bind_method(D_METHOD("get_content_scale_aspect"), &Window::get_content_scale_aspect);

	ClassDB::bind_method(D_METHOD("set_use_font_oversampling", "enable"), &Window::set_use_font_oversampling);
	ClassDB::bind_method(D_METHOD("is_using_font_oversampling"), &Window::is_using_font_oversampling);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "title"), "set_title", "get_title");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2I, "position"), "set_position", "get_position");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2I, "size"), "set_size", "get_size");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "mode", PROPERTY_HINT_ENUM, "Windowed,Minimized,Maximized,FullScreen"), "set_mode", "get_mode");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "current_screen"), "set_current_screen", "get_current_screen");
	ADD_GROUP("Flags", "");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "visible"), "set_visible", "is_visible");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "transient"), "set_transient", "is_transient");
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "unresizable"), "set_flag", "get_flag", FLAG_RESIZE_DISABLED);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "borderless"), "set_flag", "get_flag", FLAG_BORDERLESS);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "always_on_top"), "set_flag", "get_flag", FLAG_ALWAYS_ON_TOP);
	ADD_PROPERTYI(PropertyInfo(Variant::BOOL, "transparent"), "set_flag", "get_flag", FLAG_TRANSPARENT);
	ADD_GROUP("Limits", "");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2I, "min_size"), "set_min_size", "get_min_size");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2I, "max_size"), "set_max_size", "get_max_size");
	ADD_GROUP("Content Scale", "content_scale_");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2I, "content_scale_size"), "set_content_scale_size", "get_content_scale_size");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "content_scale_mode", PROPERTY_HINT_ENUM, "Disabled,Object,Pixels"), "set_content_scale_mode", "get_content_scale_mode");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "content_scale_aspect", PROPERTY_HINT_ENUM, "Ignore,Keep,KeepWidth,KeepHeight,Expand"), "set_content_scale_aspect", "get_content_scale_aspect");

	ADD_SIGNAL(MethodInfo("files_dropped"));
	ADD_SIGNAL(MethodInfo("mouse_entered"));
	ADD_SIGNAL(MethodInfo("mouse_exited"));
	ADD_SIGNAL(MethodInfo("focus_entered"));
	ADD_SIGNAL(MethodInfo("focus_exited"));
	ADD_SIGNAL(MethodInfo("close_requested"));
	ADD_SIGNAL(MethodInfo("go_back_requested"));
}

Window::Window() {
	for (int i = 0; i < FLAG_MAX; i++) {
		flags[i] = false;
	}
	content_scale_mode = CONTENT_SCALE_MODE_DISABLED;
	content_scale_aspect = CONTENT_SCALE_ASPECT_IGNORE;
}
Window::~Window() {
}