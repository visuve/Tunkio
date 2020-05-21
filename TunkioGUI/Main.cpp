#include "PCH.hpp"

namespace
{
    constexpr int WindowWidth = 500;
    constexpr int WindowHeight = 255;

    constexpr int LabelMargin = 10;
    constexpr int LabelSpacing = 5;
    constexpr int LabelWidth = 480;
    constexpr int LabelHeight = 30;

    constexpr int ButtonWidth = 100;

    void OnRunButtonClicked(Fl_Widget*, void*)
    {
        std::cout << "RUN!" << std::endl;
    }

    void OnAbortButtonClicked(Fl_Widget*, void*)
    {
        std::cout << "ABORT!" << std::endl;
    }
}

int main(int argc, char* argv[]) 
{
    auto window = new Fl_Window(WindowWidth, WindowHeight, "Tunkio");

    {
        auto cmdParams = new Fl_Box(FL_EMBOSSED_FRAME, LabelMargin, LabelSpacing, LabelWidth, LabelHeight, "CMD PARAMS:");
        cmdParams->align(Fl_Align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE));

        auto dataWrittenLabel = new Fl_Box(FL_EMBOSSED_FRAME, LabelMargin, LabelHeight + LabelSpacing * 2, LabelWidth, LabelHeight, "WRITTEN:");
        dataWrittenLabel->align(Fl_Align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE));

        auto speedLabel = new Fl_Box(FL_EMBOSSED_FRAME, LabelMargin, LabelHeight * 2 + LabelSpacing * 3, LabelWidth, LabelHeight, "SPEED:");
        speedLabel->align(Fl_Align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE));

        auto avgSpeedLabel = new Fl_Box(FL_EMBOSSED_FRAME, LabelMargin, LabelHeight * 3 + LabelSpacing * 4, LabelWidth, LabelHeight, "AVG. SPEED:");
        avgSpeedLabel->align(Fl_Align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE));

        auto timeLeftLabel = new Fl_Box(FL_EMBOSSED_FRAME, LabelMargin, LabelHeight * 4 + LabelSpacing * 5, LabelWidth, LabelHeight, "TIME LEFT:");
        timeLeftLabel->align(Fl_Align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE));

        auto progress = new Fl_Progress(LabelMargin, LabelHeight * 5 + LabelSpacing * 6, LabelWidth, LabelHeight);
        progress->minimum(0);
        progress->maximum(100);
        progress->value(50.0f);
        progress->label("50%");
        progress->color(FL_DARK3);
        progress->selection_color(FL_DARK_BLUE);
        progress->labelcolor(FL_BLACK);


        auto runButton = new Fl_Button(WindowWidth - ButtonWidth - LabelMargin, LabelHeight * 6 + LabelSpacing * 7, ButtonWidth, LabelHeight, "Run");
        runButton->callback(OnRunButtonClicked);

        auto abortButton = new Fl_Button(WindowWidth - ButtonWidth - LabelMargin - LabelSpacing - ButtonWidth, LabelHeight * 6 + LabelSpacing * 7, 100, LabelHeight, "Abort");
        abortButton->callback(OnAbortButtonClicked);


    }

    window->end();
    window->show(argc, argv);

    return Fl::run();
}