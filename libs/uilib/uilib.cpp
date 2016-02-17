#include "uilib.h"

#include <tuple>

#include <gtk/gtk.h>

#include "gtkutil/dialog.h"
#include "gtkutil/filechooser.h"
#include "gtkutil/messagebox.h"
#include "gtkutil/window.h"

namespace ui {

    void init(int argc, char *argv[])
    {
        gtk_disable_setlocale();
        gtk_init(&argc, &argv);
    }

    void main()
    {
        gtk_main();
    }

    Widget root;

    alert_response Widget::alert(std::string text, std::string title, alert_type type, alert_icon icon)
    {
        auto ret = gtk_MessageBox(*this, text.c_str(),
                                  title.c_str(),
                                  type == alert_type::OK ? eMB_OK :
                                  type == alert_type::OKCANCEL ? eMB_OKCANCEL :
                                  type == alert_type::YESNO ? eMB_YESNO :
                                  type == alert_type::YESNOCANCEL ? eMB_YESNOCANCEL :
                                  type == alert_type::NOYES ? eMB_NOYES :
                                  eMB_OK,
                                  icon == alert_icon::DEFAULT ? eMB_ICONDEFAULT :
                                  icon == alert_icon::ERROR ? eMB_ICONERROR :
                                  icon == alert_icon::WARNING ? eMB_ICONWARNING :
                                  icon == alert_icon::QUESTION ? eMB_ICONQUESTION :
                                  icon == alert_icon::ASTERISK ? eMB_ICONASTERISK :
                                  eMB_ICONDEFAULT
        );
        return
                ret == eIDOK ? alert_response::OK :
                ret == eIDCANCEL ? alert_response::CANCEL :
                ret == eIDYES ? alert_response::YES :
                ret == eIDNO ? alert_response::NO :
                alert_response::OK;
    }

    const char *Widget::file_dialog(bool open, const char *title, const char *path,
                                    const char *pattern, bool want_load, bool want_import,
                                    bool want_save)
    {
        return ::file_dialog(*this, open, title, path, pattern, want_load, want_import, want_save);
    }

    Window Window::create_dialog_window(const char *title, void func(), void *data, int default_w, int default_h)
    {
        return Window(::create_dialog_window(*this, title, func, data, default_w, default_h));
    }

    Window Window::create_modal_dialog_window(const char *title, ui_modal &dialog, int default_w, int default_h)
    {
        return Window(::create_modal_dialog_window(*this, title, dialog, default_w, default_h));
    }

    Window Window::create_floating_window(const char *title)
    {
        return Window(::create_floating_window(title, *this));
    }

    std::uint64_t Window::on_key_press(bool (*f)(Widget widget, ui_evkey *event, void *extra), void *extra)
    {
        auto pass = std::make_tuple(f, extra);
        auto func = [](ui_widget *widget, GdkEventKey *event, void *pass_) -> bool {
            using pass_t = decltype(pass);
            auto &args = *(pass_t *) pass_;
            auto func = std::get<0>(args);
            auto pass = std::get<1>(args);
            return func(Widget(widget), event, pass);
        };
        return g_signal_connect(G_OBJECT(*this), "key-press-event", (GCallback) +func, &pass);
    }

    Label::Label(const char *label) : Label(GTK_LABEL(gtk_label_new(label)))
    { }

}