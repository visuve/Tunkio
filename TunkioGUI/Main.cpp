#include "PCH.hpp"
#include "TunkioProgressDialog.hpp"

int APIENTRY wWinMain(
    _In_ HINSTANCE instance,
    _In_opt_ HINSTANCE previousInstance,
    _In_ LPWSTR commandLine,
    _In_ int showCommand)
{
    UNREFERENCED_PARAMETER(previousInstance);
    UNREFERENCED_PARAMETER(showCommand);

    Tunkio::ProgressDialog dialog(instance, commandLine);
    dialog.Show();

    return 0;
}