#pragma once
#include <imgui.h>

namespace Anito3D {

    class AnitoImGuiStyle {
    public:
        static void applyStyle();

        static ImVec4 getAccentGreen();
        static ImVec4 getBackgroundColor();
        static ImVec4 getTextColor();
    };

}