/*
 * Copyright © 2018 Christian Persch
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <memory>
#include <optional>
#include <string>
#include <variant>

#include "vteterminal.h"
#include "vtepty.h"

#include "vteinternal.hh"

#include "fwd.hh"
#include "refptr.hh"

namespace vte {

namespace terminal {

class Terminal;

} // namespace terminal

namespace platform {

class Widget {
public:
        friend class vte::terminal::Terminal;

        Widget(VteTerminal* t) noexcept;
        ~Widget() noexcept;

        Widget(Widget const&) = delete;
        Widget(Widget&&) = delete;
        Widget& operator= (Widget const&) = delete;
        Widget& operator= (Widget&&) = delete;

        GObject* object() const noexcept { return reinterpret_cast<GObject*>(m_widget); }
        GtkWidget* gtk() const noexcept { return m_widget; }
        VteTerminal* vte() const noexcept { return reinterpret_cast<VteTerminal*>(m_widget); }

        inline constexpr vte::terminal::Terminal* terminal() const noexcept { return m_terminal; }

        void constructed() noexcept { m_terminal->widget_constructed(); }
        void dispose() noexcept;
        void realize() noexcept;
        void unrealize() noexcept;
        void map() noexcept;
        void unmap() noexcept;
        void style_updated() noexcept { m_terminal->widget_style_updated(); }
        void draw(cairo_t *cr) noexcept { m_terminal->widget_draw(cr); }
        void get_preferred_width(int *minimum_width,
                                 int *natural_width) const noexcept { m_terminal->widget_get_preferred_width(minimum_width, natural_width); }
        void get_preferred_height(int *minimum_height,
                                  int *natural_height) const noexcept { m_terminal->widget_get_preferred_height(minimum_height, natural_height); }
        void size_allocate(GtkAllocation *allocation) noexcept;

        void focus_in(GdkEventFocus *event) noexcept { m_terminal->widget_focus_in(event); }
        void focus_out(GdkEventFocus *event) noexcept { m_terminal->widget_focus_out(event); }
        bool key_press(GdkEventKey *event) noexcept { return m_terminal->widget_key_press(event); }
        bool key_release(GdkEventKey *event) noexcept { return m_terminal->widget_key_release(event); }
        bool button_press(GdkEventButton *event) noexcept { return m_terminal->widget_button_press(event); }
        bool button_release(GdkEventButton *event) noexcept { return m_terminal->widget_button_release(event); }
        void enter(GdkEventCrossing *event) noexcept { m_terminal->widget_enter(event); }
        void leave(GdkEventCrossing *event) noexcept { m_terminal->widget_leave(event); }
        void scroll(GdkEventScroll *event) noexcept { m_terminal->widget_scroll(event); }
        bool motion_notify(GdkEventMotion *event) noexcept { return m_terminal->widget_motion_notify(event); }

        void paste(GdkAtom board) noexcept { m_terminal->widget_paste(board); }
        void copy(VteSelection sel,
                  VteFormat format) noexcept { m_terminal->widget_copy(sel, format); }
        void paste_received(char const* text) noexcept { m_terminal->widget_paste_received(text); }
        void clipboard_cleared(GtkClipboard *clipboard) noexcept { m_terminal->widget_clipboard_cleared(clipboard); }
        void clipboard_requested(GtkClipboard *target_clipboard,
                                 GtkSelectionData *data,
                                 guint info) noexcept { m_terminal->widget_clipboard_requested(target_clipboard, data, info); }

        void screen_changed (GdkScreen *previous_screen) noexcept;
        void settings_changed() noexcept;

        void beep() noexcept;

        void set_hadjustment(vte::glib::RefPtr<GtkAdjustment>&& adjustment) noexcept { m_hadjustment = std::move(adjustment); }
        void set_vadjustment(vte::glib::RefPtr<GtkAdjustment>&& adjustment) { terminal()->widget_set_vadjustment(std::move(adjustment)); }
        auto hadjustment() noexcept { return m_hadjustment.get(); }
        auto vadjustment() noexcept { return terminal()->vadjustment(); }
        void set_hscroll_policy(GtkScrollablePolicy policy) noexcept { m_hscroll_policy = policy; }
        void set_vscroll_policy(GtkScrollablePolicy policy) noexcept { m_vscroll_policy = policy; }
        auto hscroll_policy() const noexcept { return m_hscroll_policy; }
        auto vscroll_policy() const noexcept { return m_vscroll_policy; }

        bool set_cursor_blink_mode(VteCursorBlinkMode mode) { return terminal()->set_cursor_blink_mode(vte::terminal::Terminal::CursorBlinkMode(mode)); }
        auto cursor_blink_mode() const noexcept { return VteCursorBlinkMode(terminal()->cursor_blink_mode()); }

        bool set_cursor_shape(VteCursorShape shape) { return terminal()->set_cursor_shape(vte::terminal::Terminal::CursorShape(shape)); }
        auto cursor_shape() const noexcept { return VteCursorShape(terminal()->cursor_shape()); }

        bool set_backspace_binding(VteEraseBinding mode) { return terminal()->set_backspace_binding(vte::terminal::Terminal::EraseMode(mode)); }
        auto backspace_binding() const noexcept { return VteEraseBinding(terminal()->backspace_binding()); }

        bool set_delete_binding(VteEraseBinding mode) { return terminal()->set_delete_binding(vte::terminal::Terminal::EraseMode(mode)); }
        auto delete_binding() const noexcept { return VteEraseBinding(terminal()->delete_binding()); }

        bool set_text_blink_mode(VteTextBlinkMode mode) { return terminal()->set_text_blink_mode(vte::terminal::Terminal::TextBlinkMode(mode)); }
        auto text_blink_mode() const noexcept { return VteTextBlinkMode(terminal()->text_blink_mode()); }

        bool set_word_char_exceptions(std::optional<std::string_view> stropt);
        auto word_char_exceptions() const noexcept { return m_word_char_exceptions ? m_word_char_exceptions.value().c_str() : nullptr; }

        char const* encoding() const noexcept { return m_terminal->encoding(); }

        void emit_child_exited(int status) noexcept;
        void emit_eof() noexcept;

        bool set_pty(VtePty* pty) noexcept;
        inline auto pty() const noexcept { return m_pty.get(); }

        void feed(std::string_view const& str) { terminal()->feed(str); }
        void feed_child(std::string_view const& str) { terminal()->feed_child(str); }
        void feed_child_binary(std::string_view const& str) { terminal()->feed_child_binary(str); }

        bool should_emit_signal(int id) noexcept;

protected:

        enum class CursorType {
                eDefault,
                eInvisible,
                eMousing,
                eHyperlink
        };

        GdkWindow* event_window() const noexcept { return m_event_window; }

        bool realized() const noexcept
        {
                return gtk_widget_get_realized(m_widget);
        }

        vte::glib::RefPtr<GdkCursor> create_cursor(GdkCursorType cursor_type) const noexcept;

        void set_cursor(CursorType type) noexcept;
        void set_cursor(GdkCursor* cursor) noexcept;
        void set_cursor(Cursor const& cursor) noexcept;

        bool im_filter_keypress(GdkEventKey* event) noexcept;

        void im_focus_in() noexcept;
        void im_focus_out() noexcept;

        void im_reset() noexcept
        {
                if (m_im_context)
                        gtk_im_context_reset(m_im_context.get());
        }

        void im_set_cursor_location(cairo_rectangle_int_t const* rect) noexcept;

        void unset_pty() noexcept;

public: // FIXMEchpe
        void im_preedit_changed() noexcept;

private:
        GtkWidget* m_widget;

        vte::terminal::Terminal* m_terminal;

        /* Event window */
        GdkWindow *m_event_window;

        /* Cursors */
        vte::glib::RefPtr<GdkCursor> m_default_cursor;
        vte::glib::RefPtr<GdkCursor> m_invisible_cursor;
        vte::glib::RefPtr<GdkCursor> m_mousing_cursor;
        vte::glib::RefPtr<GdkCursor> m_hyperlink_cursor;

        /* Input method */
        vte::glib::RefPtr<GtkIMContext> m_im_context;

        /* PTY */
        vte::glib::RefPtr<VtePty> m_pty;

        /* Misc */
        std::optional<std::string> m_word_char_exceptions{};

        vte::glib::RefPtr<GtkAdjustment> m_hadjustment{};
        uint32_t m_hscroll_policy : 1;
        uint32_t m_vscroll_policy : 1;
};

} // namespace platform

} // namespace vte
