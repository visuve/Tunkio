#include "PCH.hpp"
#include <Windows.h>

void OnExitClicked()
{
    nana::API::exit();
}

void OnRunClicked()
{
    std::cout << "RUN!" << std::endl;
}

void OnAbortClicked()
{
    std::cout << "ABORT!" << std::endl;
}

class GUI
{
public:
    GUI(const std::string& cmdline) :
        form(nana::API::make_center(500, 300)),
        cmdParamsLabel(form, "Parameters given: " + cmdline.empty() ? "none." : cmdline),
        progressLabel(form, "Progress: not started."),
        progressBar(form),
        exitButton(form),
        abortButton(form),
        runButton(form),
        place(form)
    {
        form.caption(L"Tunkio GUI");

        progressBar.amount(100);
        progressBar.value(50);

        exitButton.caption(L"Exit");
        exitButton.events().click(OnExitClicked);

        abortButton.caption(L"Abort");
        abortButton.events().click(OnAbortClicked);

        runButton.caption(L"Run");
        runButton.events().click(OnRunClicked);

        place.div("vert margin=3% <labels vertical gap=10 arrange=[75,75,25]> <buttons weight=25 gap=10 >");
        place.field("labels") << cmdParamsLabel << progressLabel << progressBar;
        place.field("buttons") << exitButton << abortButton << runButton;

        place.collocate();

        form.show();
    }
private:
    nana::form form;
    nana::label cmdParamsLabel;
    nana::label progressLabel;
    nana::progress progressBar;
    nana::button exitButton;
    nana::button abortButton;
    nana::button runButton;
    nana::place place;
};

int WINAPI WinMain(HINSTANCE, HINSTANCE, char* cmdline, int)
{
    GUI gui(cmdline);
    nana::exec();

    return 0;
}