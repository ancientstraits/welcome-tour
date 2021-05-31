#include <gtkmm.h>
#include <glibmm.h>

#include <iostream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <filesystem>
std::tuple<int, std::string> exec(const char *cmd)
{
    std::array<char, 128> buffer;
    std::string result;
    FILE *pipe = popen(cmd, "r");
    if (!pipe)
    {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
    {
        result += buffer.data();
    }

    if (result[result.size() - 1] == '\n')
        result = result.substr(0, result.size() - 1);
    int status = pclose(pipe);
    return {status, result};
}

namespace WelcomeTour
{
    class Window;

    class Page
    {
    public:
        virtual void initialize() = 0;
        virtual bool can_leave() = 0;
    };

    class WelcomePage : public Page, public Gtk::Box
    {
        Glib::RefPtr<Gtk::Builder> const &ui;

    public:
        WelcomePage(BaseObjectType *obj, Glib::RefPtr<Gtk::Builder> const &refbuilder)
            : Gtk::Box(obj), ui(refbuilder)
        {
        }

        void initialize()
        {
            std::cout << "Initializing welcome page" << std::endl;
        }

        bool can_leave()
        {
            return true;
        }
    };

    class InterfacePage : public Page, public Gtk::Box
    {
        Glib::RefPtr<Gtk::Builder> const &ui;

    public:
        InterfacePage(BaseObjectType *obj, Glib::RefPtr<Gtk::Builder> const &refbuilder)
            : Gtk::Box(obj), ui(refbuilder)
        {
        }

        void initialize()
        {
        }

        bool can_leave()
        {
            return true;
        }
    };

    class AppsPage : public Page, public Gtk::Box
    {
        Glib::RefPtr<Gtk::Builder> const &ui;
        Gtk::LinkButton *apps_link_btn,
            *appimage_org_link,
            *appimage_hub_link;

    public:
        AppsPage(BaseObjectType *obj, Glib::RefPtr<Gtk::Builder> const &refbuilder)
            : Gtk::Box(obj), ui(refbuilder)
        {
            ui->get_widget("apps_link_btn", apps_link_btn);
            ui->get_widget("appimage_org_link", appimage_org_link);
            ui->get_widget("appimage_hub_link", appimage_hub_link);

            apps_link_btn->set_label("Bazaar");
            appimage_org_link->set_label("AppImage Home");
            appimage_hub_link->set_label("AppImage Hub");
        }

        void initialize()
        {
        }

        bool can_leave()
        {
            return true;
        }
    };

    class UpdatesPage : public Page, public Gtk::Box
    {
        Glib::RefPtr<Gtk::Builder> const &ui;

    public:
        UpdatesPage(BaseObjectType *obj, Glib::RefPtr<Gtk::Builder> const &refbuilder)
            : Gtk::Box(obj), ui(refbuilder)
        {
        }

        void initialize()
        {
        }

        bool can_leave()
        {
            return true;
        }
    };

    class HelpPage : public Page, public Gtk::Box
    {
        Glib::RefPtr<Gtk::Builder> const &ui;
        Gtk::LinkButton *discord_link,
            *youtube_link,
            *community_link,

            *doc_link,
            *support_link,
            *home_link,

            *updates_link,
            *blog_link,
            *doc_rlxos;

    public:
        HelpPage(BaseObjectType *obj, Glib::RefPtr<Gtk::Builder> const &refbuilder)
            : Gtk::Box(obj), ui(refbuilder)
        {
            ui->get_widget("discord_link", discord_link);
            discord_link->set_label("Discord");

            ui->get_widget("youtube_link", youtube_link);
            youtube_link->set_label("Youtube");

            ui->get_widget("community_link", community_link);
            community_link->set_label("Forum");

            ui->get_widget("doc_link", doc_link);
            doc_link->set_label("Documentation");

            ui->get_widget("support_link", support_link);
            support_link->set_label("Support");

            ui->get_widget("home_link", home_link);
            home_link->set_label("Website");

            ui->get_widget("updates_link", updates_link);
            updates_link->set_label("Upates");

            ui->get_widget("blog_rlxos", blog_link);
            blog_link->set_label("Blog");

            ui->get_widget("doc_rlxos", doc_rlxos);
            doc_rlxos->set_label("Docs");
        }

        void initialize()
        {
        }

        bool can_leave()
        {
            return true;
        }
    };

    class EndPage : public Page, public Gtk::Box
    {
        Glib::RefPtr<Gtk::Builder> const &ui;
        Gtk::Button *enjoy_btn;

    public:
        EndPage(BaseObjectType *obj, Glib::RefPtr<Gtk::Builder> const &refbuilder)
            : Gtk::Box(obj), ui(refbuilder)
        {
            ui->get_widget("enjoy_btn", enjoy_btn);
            enjoy_btn->signal_clicked().connect(sigc::mem_fun(*this, &EndPage::EndNow));
        }

        void EndNow()
        {
            std::string HOMEDIR = std::getenv("HOME");
            system(("touch " + HOMEDIR + "/.config/welcome-tour").c_str());
            Gtk::Main::quit();
        }

        void initialize()
        {
        }

        bool can_leave()
        {
            return true;
        }
    };

    class Stack : public Gtk::Stack
    {
        Glib::RefPtr<Gtk::Builder> const &ui;
        std::map<std::string, Page *> pages;
        std::vector<std::string> pages_id;

        Gtk::Button *back_btn, *next_btn;

        int current_index = 0;

    public:
        Stack(BaseObjectType *obj, Glib::RefPtr<Gtk::Builder> const &refbuilder)
            : Gtk::Stack(obj), ui(refbuilder)
        {
            ui->get_widget("back_btn", back_btn);
            ui->get_widget("next_btn", next_btn);

            next_btn->signal_clicked().connect(sigc::mem_fun(*this, &Stack::nextPage));
            back_btn->signal_clicked().connect(sigc::mem_fun(*this, &Stack::backPage));
        }

        void nextPage()
        {
            if (!pages[pages_id[current_index]]->can_leave())
                return;
            if (!((current_index + 1) >= pages_id.size()))
            {
                current_index++;
                set_visible_child(pages_id[current_index]);
                pages[pages_id[current_index]]->initialize();
            }

            updateButtons();
        }

        void backPage()
        {
            if ((current_index) > 0)
            {
                current_index--;
                set_visible_child(pages_id[current_index]);
                //pages[pages_id[current_index]]->initialize();
            }

            updateButtons();
        }

        void addPage(std::string i, Page *p)
        {
            pages.insert(make_pair(i, p));
            pages_id.push_back(i);
        }

        void enableBack(bool enable = true)
        {
            back_btn->set_sensitive(enable);
        }

        void enableNext(bool enable = true)
        {
            next_btn->set_sensitive(enable);
        }

        void updateButtons()
        {
            enableNext((current_index + 1) < pages_id.size());
            enableBack(current_index > 0);
        }
    };

    class Window : public Gtk::Window
    {
        Glib::RefPtr<Gtk::Builder> const &ui;

    public:
        Stack *stack;

        Window(BaseObjectType *winobj, Glib::RefPtr<Gtk::Builder> const &refbuilder)
            : Gtk::Window(winobj), ui(refbuilder)
        {
            set_position(Gtk::WIN_POS_CENTER_ALWAYS);
        }

        static Window *create()
        {
            Window *win;
            WelcomePage *welcome_page;
            InterfacePage *interface_page;
            AppsPage *apps_page;
            UpdatesPage *updates_page;
            HelpPage *help_page;
            EndPage *end_page;
            Stack *stack;
            Gtk::LinkButton *updates_link;

            auto ui = Gtk::Builder::create_from_resource("/dev/rlxos/welcome-tour/ui.glade");
            ui->get_widget_derived("main_window", win);

            ui->get_widget("updates_page_link", updates_link);
            updates_link->set_label("Learn More");

            ui->get_widget_derived("stack", stack);
            ui->get_widget_derived("welcome_page", welcome_page);
            ui->get_widget_derived("interface_page", interface_page);
            ui->get_widget_derived("apps_page", apps_page);
            ui->get_widget_derived("updates_page", updates_page);
            ui->get_widget_derived("help_page", help_page);
            ui->get_widget_derived("end_page", end_page);

            stack->addPage("welcome_page", welcome_page);
            stack->addPage("interface_page", interface_page);
            stack->addPage("apps_page", apps_page);
            stack->addPage("updates_page", updates_page);
            stack->addPage("help_page", help_page);
            stack->addPage("end_page", end_page);

            win->stack = stack;

            return win;
        }
    };

}

int main(int ac, char **av)
{
    std::string HOMEDIR = std::getenv("HOME");
    if (std::filesystem::exists(HOMEDIR + "/.config/welcome-tour") && std::getenv("WITH_AUTOSTART") != nullptr)
        return 0;

    Gtk::Main gtk_(ac, av);
    WelcomeTour::Window *window = WelcomeTour::Window::create();
    window->stack->updateButtons();
    window->set_position(Gtk::WIN_POS_CENTER_ALWAYS);
    Gtk::Main::run(*window);
    return 0;
}