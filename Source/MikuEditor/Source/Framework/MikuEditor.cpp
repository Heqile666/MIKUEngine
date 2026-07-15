#include "Framework/MikuEditor.h"

#include "Miku/Engine/Application.h"
#include "Miku/Engine/Log.h"

namespace MIKU
{
    // 编辑器应用壳：目前仅继承引擎 Application，后续可扩展编辑器 UI / 面板
    class MikuEditorApp : public Application
    {
    public:
        MikuEditorApp() = default;
        ~MikuEditorApp() override = default;
    };
}

int MikuEditorMain(int argc, char** argv)
{
    MIKU::Log::Init();
    MIKU_CORE_WARN("MikuEditor starting...");

    MIKU::Application* app = new MIKU::MikuEditorApp();
    app->Run();
    delete app;

    return 0;
}
