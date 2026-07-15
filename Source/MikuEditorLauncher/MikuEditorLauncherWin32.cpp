// ============================================================
//  MikuEditorLauncher —— 极薄可执行入口（对齐 Horizon 的 Launcher）
//  仅负责调用编辑器入口 MikuEditorMain
// ============================================================
#include "Editor/MikuEditorModule.h"

int main(int argc, char** argv)
{
    return MikuEditorMain(argc, argv);
}
