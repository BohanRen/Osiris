#include <functional>
#include <string>
#include <Windows.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"

#include "GUI.h"
#include "Config.h"
#include "Hacks/Misc.h"
#include "Hacks/SkinChanger.h"
#include "Hacks/Visuals.h"
#include "Hooks.h"
#include "SDK/InputSystem.h"

constexpr auto windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
| ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

GUI::GUI() noexcept
{
    ImGui::CreateContext();
    ImGui_ImplWin32_Init(FindWindowW(L"Valve001", NULL));

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 5.0f;
    style.WindowBorderSize = 0.0f;
    style.ChildBorderSize = 0.0f;
    style.GrabMinSize = 7.0f;
    style.GrabRounding = 5.0f;
    style.FrameRounding = 5.0f;
    style.PopupRounding = 5.0f;
    style.ScrollbarSize = 9.0f;

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;
	io.Fonts->AddFontFromFileTTF("c:\\windows\\fonts\\msyh.ttc", 16, NULL, io.Fonts->GetGlyphRangesChineseFull());
    static ImWchar ranges[] = { 0x0020, 0x00FF, 0x0100, 0x017f, 0 };
	/*
    if (char buffer[MAX_PATH]; GetWindowsDirectoryA(buffer, MAX_PATH)) {
        fonts.tahoma = io.Fonts->AddFontFromFileTTF(strcat(buffer, "/Fonts/Tahoma.ttf"), 16.0f, nullptr, ranges);
    }
	*/
}

void GUI::render() noexcept
{
    if (!config.style.menuStyle) {
        renderMenuBar();
        renderAimbotWindow();
        renderAntiAimWindow();
        renderTriggerbotWindow();
        renderBacktrackWindow();
        renderGlowWindow();
        renderChamsWindow();
        renderEspWindow();
        renderVisualsWindow();
        renderSkinChangerWindow();
        renderSoundWindow();
        renderStyleWindow();
        renderMiscWindow();
        renderReportbotWindow();
        renderConfigWindow();
    } else {
        renderGuiStyle2();
    }
}

void GUI::updateColors() const noexcept
{
    switch (config.style.menuColors) {
    case 0: ImGui::StyleColorsDark(); break;
    case 1: ImGui::StyleColorsLight(); break;
    case 2: ImGui::StyleColorsClassic(); break;
    }
}

void GUI::checkboxedColorPicker(const std::string& name, bool* enable, float* color) noexcept
{
    ImGui::Checkbox(("##" + name).c_str(), enable);
    ImGui::SameLine(0.0f, 5.0f);
    ImGui::PushID(0);
    bool openPopup = ImGui::ColorButton(("##" + name).c_str(), ImColor{ color[0], color[1], color[2] }, ImGuiColorEditFlags_NoTooltip);
    ImGui::PopID();
    ImGui::SameLine(0.0f, 5.0f);
    ImGui::TextUnformatted(name.c_str());
    ImGui::PushID(1);
    if (openPopup)
        ImGui::OpenPopup(("##" + name).c_str());
    if (ImGui::BeginPopup(("##" + name).c_str())) {
        ImGui::PushID(2);
        ImGui::ColorPicker3(("##" + name).c_str(), color, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoSidePreview);
        ImGui::PopID();
        ImGui::EndPopup();
    }
    ImGui::PopID();
}

void GUI::hotkey(int& key) noexcept
{
    constexpr bool stringDisplayTest = true;

    if constexpr (stringDisplayTest)
        key ? ImGui::Text("[ %s ]", interfaces.inputSystem->virtualKeyToString(key)) : ImGui::TextUnformatted("[ key ]");
    else
        key ? ImGui::Text("[ 0x%x ]", key) : ImGui::TextUnformatted("[ key ]");

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip(u8"按任意键绑定按键");
        ImGuiIO& io = ImGui::GetIO();
        for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++)
            if (ImGui::IsKeyPressed(i) && i != config.misc.menuKey)
                key = i != VK_ESCAPE ? i : 0;

        for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
            if (ImGui::IsMouseDown(i) && i + (i > 1 ? 2 : 1) != config.misc.menuKey)
                key = i + (i > 1 ? 2 : 1);
    }
}

void GUI::renderMenuBar() noexcept
{
    if (ImGui::BeginMainMenuBar()) {
		ImGui::Text(u8"更新时间:2019.10.14 Ver:2.0");
		ImGui::SameLine();
		ImGui::MenuItem(u8"自动瞄准1", nullptr, &window.aimbot);
		ImGui::MenuItem(u8"大陀螺", nullptr, &window.antiAim);
		ImGui::MenuItem(u8"自动瞄准2", nullptr, &window.triggerbot);
		ImGui::MenuItem(u8"后背追踪", nullptr, &window.backtrack);
		ImGui::MenuItem(u8"人物发光", nullptr, &window.glow);
		ImGui::MenuItem(u8"人物上色", nullptr, &window.chams);
		ImGui::MenuItem(u8"透视", nullptr, &window.esp);
		ImGui::MenuItem(u8"渲染类", nullptr, &window.visuals);
		ImGui::MenuItem(u8"皮肤更换", nullptr, &window.skinChanger);
		ImGui::MenuItem(u8"声音类", nullptr, &window.sound);
		ImGui::MenuItem(u8"风格类", nullptr, &window.style);
		ImGui::MenuItem(u8"杂项", nullptr, &window.misc);
		ImGui::MenuItem(u8"举报机器人", nullptr, &window.reportbot);
		ImGui::MenuItem(u8"配置文件", nullptr, &window.config);
        ImGui::EndMainMenuBar();
    }
}

void GUI::renderAimbotWindow() noexcept
{
    if (window.aimbot) {
        if (!config.style.menuStyle) {
            ImGui::SetNextWindowSize({ 600.0f, 0.0f });
            ImGui::Begin(u8"自动瞄准1", &window.aimbot, windowFlags);
        }
        static int currentCategory{ 0 };
        ImGui::PushItemWidth(110.0f);
        ImGui::PushID(0);
		ImGui::Combo("", &currentCategory, u8"全部\0手枪\0重武器\0冲锋枪\0步枪\0电击枪\0");
        ImGui::PopID();
        ImGui::SameLine();
        static int currentWeapon{ 0 };
        ImGui::PushID(1);

        switch (currentCategory) {
        case 0:
            currentWeapon = 0;
            ImGui::NewLine();
            break;
        case 1: {
            static int currentPistol{ 0 };
			static constexpr const char* pistols[]{ u8"全部", "Glock-18", "P2000", "USP-S", u8"双枪", "P250", "Tec-9", u8"FN手枪", "CZ-75", u8"沙鹰", u8"左轮手枪" };

            ImGui::Combo("", &currentPistol, [](void* data, int idx, const char** out_text) {
                if (config.aimbot[idx ? idx : 35].enabled) {
                    static std::string name;
                    name = pistols[idx];
                    *out_text = name.append(" *").c_str();
                } else {
                    *out_text = pistols[idx];
                }
                return true;
            }, nullptr, IM_ARRAYSIZE(pistols));

            currentWeapon = currentPistol ? currentPistol : 35;
            break;
        }
        case 2: {
            static int currentHeavy{ 0 };
			static constexpr const char* heavies[]{ u8"全部", "Nova", "XM1014", "Sawed-off", "MAG-7", "M249", "Negev" };

            ImGui::Combo("", &currentHeavy, [](void* data, int idx, const char** out_text) {
                if (config.aimbot[idx ? idx + 10 : 36].enabled) {
                    static std::string name;
                    name = heavies[idx];
                    *out_text = name.append(" *").c_str();
                } else {
                    *out_text = heavies[idx];
                }
                return true;
            }, nullptr, IM_ARRAYSIZE(heavies));

            currentWeapon = currentHeavy ? currentHeavy + 10 : 36;
            break;
        }
        case 3: {
            static int currentSmg{ 0 };
			static constexpr const char* smgs[]{ u8"全部", "Mac-10", "MP9", "MP7", "MP5-SD", "UMP-45", "P90", "PP-Bizon" };

            ImGui::Combo("", &currentSmg, [](void* data, int idx, const char** out_text) {
                if (config.aimbot[idx ? idx + 16 : 37].enabled) {
                    static std::string name;
                    name = smgs[idx];
                    *out_text = name.append(" *").c_str();
                } else {
                    *out_text = smgs[idx];
                }
                return true;
            }, nullptr, IM_ARRAYSIZE(smgs));

            currentWeapon = currentSmg ? currentSmg + 16 : 37;
            break;
        }
        case 4: {
            static int currentRifle{ 0 };
			static constexpr const char* rifles[]{ u8"全部", "Galil AR", "Famas", "AK-47", "M4A4", "M4A1-S", "SSG-08", "SG-553", "AUG", "AWP", "G3SG1", "SCAR-20" };

            ImGui::Combo("", &currentRifle, [](void* data, int idx, const char** out_text) {
                if (config.aimbot[idx ? idx + 23 : 38].enabled) {
                    static std::string name;
                    name = rifles[idx];
                    *out_text = name.append(" *").c_str();
                } else {
                    *out_text = rifles[idx];
                }
                return true;
            }, nullptr, IM_ARRAYSIZE(rifles));

            currentWeapon = currentRifle ? currentRifle + 23 : 38;
            break;
        }
        }
        ImGui::PopID();
        ImGui::SameLine();
		ImGui::Checkbox(u8"启用", &config.aimbot[currentWeapon].enabled);
        ImGui::Separator();
        ImGui::Columns(2, nullptr, false);
        ImGui::SetColumnOffset(1, 220.0f);
		ImGui::Checkbox(u8"热键", &config.aimbot[currentWeapon].onKey);
        ImGui::SameLine();
        hotkey(config.aimbot[currentWeapon].key);
        ImGui::SameLine();
        ImGui::PushID(2);
        ImGui::PushItemWidth(70.0f);
		ImGui::Combo("", &config.aimbot[currentWeapon].keyMode, u8"按住开启\0按住切换\0");
        ImGui::PopItemWidth();
        ImGui::PopID();
		ImGui::Checkbox(u8"锁定自瞄", &config.aimbot[currentWeapon].aimlock);
		ImGui::Checkbox(u8"静默自瞄", &config.aimbot[currentWeapon].silent);
		ImGui::Checkbox(u8"攻击队友", &config.aimbot[currentWeapon].friendlyFire);
		ImGui::Checkbox(u8"可视检查", &config.aimbot[currentWeapon].visibleOnly);
		ImGui::Checkbox(u8"仅开镜时", &config.aimbot[currentWeapon].scopedOnly);
		ImGui::Checkbox(u8"忽视闪光", &config.aimbot[currentWeapon].ignoreFlash);
		ImGui::Checkbox(u8"忽视烟雾", &config.aimbot[currentWeapon].ignoreSmoke);
		ImGui::Checkbox(u8"自动开枪", &config.aimbot[currentWeapon].autoShot);
		ImGui::Checkbox(u8"自动开镜", &config.aimbot[currentWeapon].autoScope);
		ImGui::Checkbox(u8"自动压枪", &config.aimbot[currentWeapon].recoilbasedFov);
		ImGui::Combo(u8"瞄准位置", &config.aimbot[currentWeapon].bone, u8"最近位置\0最高伤害\0头\0脖子\0胸骨\0胸膛\0胃\0骨盆\0");
        ImGui::NextColumn();
        ImGui::PushItemWidth(240.0f);
		ImGui::SliderFloat(u8"瞄准范围", &config.aimbot[currentWeapon].fov, 0.0f, 255.0f, "%.2f");
		ImGui::SliderFloat(u8"平滑度", &config.aimbot[currentWeapon].smooth, 1.0f, 100.0f, "%.2f");
		ImGui::SliderFloat(u8"X后坐力控制", &config.aimbot[currentWeapon].recoilControlX, 0.0f, 1.0f, "%.2f");
		ImGui::SliderFloat(u8"Y后坐力控制", &config.aimbot[currentWeapon].recoilControlY, 0.0f, 1.0f, "%.2f");
		ImGui::SliderFloat(u8"最大自瞄抖动", &config.aimbot[currentWeapon].maxAimInaccuracy, 0.0f, 1.0f, "%.5f", 2.0f);
		ImGui::SliderFloat(u8"最大开枪抖动", &config.aimbot[currentWeapon].maxShotInaccuracy, 0.0f, 1.0f, "%.5f", 2.0f);
		ImGui::InputInt(u8"最小伤害", &config.aimbot[currentWeapon].minDamage);
        config.aimbot[currentWeapon].minDamage = std::clamp(config.aimbot[currentWeapon].minDamage, 0, 250);
		ImGui::Checkbox(u8"一击致命", &config.aimbot[currentWeapon].killshot);
		ImGui::Checkbox(u8"间隙射击", &config.aimbot[currentWeapon].betweenShots);
		ImGui::Checkbox(u8"绘制自瞄圆圈", &config.aimbot[currentWeapon].aimbotCircle);
        ImGui::Columns(1);
        if (!config.style.menuStyle)
            ImGui::End();
    }
}

void GUI::renderAntiAimWindow() noexcept
{
    if (window.antiAim) {
        if (!config.style.menuStyle) {
            ImGui::SetNextWindowSize({ 0.0f, 0.0f });
            ImGui::Begin(u8"大陀螺", &window.antiAim, windowFlags);
        }
		ImGui::Checkbox(u8"启用", &config.antiAim.enabled);
        ImGui::Checkbox("##pitch", &config.antiAim.pitch);
        ImGui::SameLine();
        ImGui::SliderFloat("Pitch", &config.antiAim.pitchAngle, -89.0f, 89.0f, "%.2f");
        ImGui::Checkbox("Yaw", &config.antiAim.yaw);
        if (!config.style.menuStyle)
            ImGui::End();
    }
}

void GUI::renderTriggerbotWindow() noexcept
{
    if (window.triggerbot) {
        if (!config.style.menuStyle) {
            ImGui::SetNextWindowSize({ 0.0f, 0.0f });
			ImGui::Begin(u8"自动瞄准2", &window.triggerbot, windowFlags);
        }
        static int currentCategory{ 0 };
        ImGui::PushItemWidth(110.0f);
        ImGui::PushID(0);
		ImGui::Combo("", &currentCategory, u8"全部\0手枪\0重武器\0冲锋枪\0步枪\0电击枪\0");
        ImGui::PopID();
        ImGui::SameLine();
        static int currentWeapon{ 0 };
        ImGui::PushID(1);
        switch (currentCategory) {
        case 0:
            currentWeapon = 0;
            ImGui::NewLine();
            break;
        case 5:
            currentWeapon = 39;
            ImGui::NewLine();
            break;

        case 1: {
            static int currentPistol{ 0 };
			static constexpr const char* pistols[]{ u8"全部", "Glock-18", "P2000", "USP-S", "Dual Berettas", "P250", "Tec-9", "Five-Seven", "CZ-75", "Desert Eagle", "Revolver" };

            ImGui::Combo("", &currentPistol, [](void* data, int idx, const char** out_text) {
                if (config.triggerbot[idx ? idx : 35].enabled) {
                    static std::string name;
                    name = pistols[idx];
                    *out_text = name.append(" *").c_str();
                } else {
                    *out_text = pistols[idx];
                }
                return true;
            }, nullptr, IM_ARRAYSIZE(pistols));

            currentWeapon = currentPistol ? currentPistol : 35;
            break;
        }
        case 2: {
            static int currentHeavy{ 0 };
			static constexpr const char* heavies[]{ u8"全部", "Nova", "XM1014", "Sawed-off", "MAG-7", "M249", "Negev" };

            ImGui::Combo("", &currentHeavy, [](void* data, int idx, const char** out_text) {
                if (config.triggerbot[idx ? idx + 10 : 36].enabled) {
                    static std::string name;
                    name = heavies[idx];
                    *out_text = name.append(" *").c_str();
                } else {
                    *out_text = heavies[idx];
                }
                return true;
            }, nullptr, IM_ARRAYSIZE(heavies));

            currentWeapon = currentHeavy ? currentHeavy + 10 : 36;
            break;
        }
        case 3: {
            static int currentSmg{ 0 };
			static constexpr const char* smgs[]{ u8"全部", "Mac-10", "MP9", "MP7", "MP5-SD", "UMP-45", "P90", "PP-Bizon" };

            ImGui::Combo("", &currentSmg, [](void* data, int idx, const char** out_text) {
                if (config.triggerbot[idx ? idx + 16 : 37].enabled) {
                    static std::string name;
                    name = smgs[idx];
                    *out_text = name.append(" *").c_str();
                } else {
                    *out_text = smgs[idx];
                }
                return true;
            }, nullptr, IM_ARRAYSIZE(smgs));

            currentWeapon = currentSmg ? currentSmg + 16 : 37;
            break;
        }
        case 4: {
            static int currentRifle{ 0 };
			static constexpr const char* rifles[]{ u8"全部", "Galil AR", "Famas", "AK-47", "M4A4", "M4A1-S", "SSG-08", "SG-553", "AUG", "AWP", "G3SG1", "SCAR-20" };

            ImGui::Combo("", &currentRifle, [](void* data, int idx, const char** out_text) {
                if (config.triggerbot[idx ? idx + 23 : 38].enabled) {
                    static std::string name;
                    name = rifles[idx];
                    *out_text = name.append(" *").c_str();
                } else {
                    *out_text = rifles[idx];
                }
                return true;
            }, nullptr, IM_ARRAYSIZE(rifles));

            currentWeapon = currentRifle ? currentRifle + 23 : 38;
            break;
        }
        }
        ImGui::PopID();
        ImGui::SameLine();
		ImGui::Checkbox(u8"启用", &config.triggerbot[currentWeapon].enabled);
		ImGui::Separator();
		ImGui::Checkbox(u8"热键", &config.triggerbot[currentWeapon].onKey);
        ImGui::SameLine();
        hotkey(config.triggerbot[currentWeapon].key);
		ImGui::Checkbox(u8"攻击队友", &config.triggerbot[currentWeapon].friendlyFire);
		ImGui::Checkbox(u8"仅开镜时", &config.triggerbot[currentWeapon].scopedOnly);
		ImGui::Checkbox(u8"忽视闪光", &config.triggerbot[currentWeapon].ignoreFlash);
		ImGui::Checkbox(u8"忽视烟雾", &config.triggerbot[currentWeapon].ignoreSmoke);
        ImGui::SetNextItemWidth(85.0f);
		ImGui::Combo(u8"瞄准位置", &config.triggerbot[currentWeapon].hitgroup, u8"全部\0头部\0胸膛\0胃\0左手臂\0右手臂\0左脚\0右脚\0");
        ImGui::PushItemWidth(220.0f);
        ImGui::SliderInt("", &config.triggerbot[currentWeapon].shotDelay, 0, 250, u8"射击间隔: %d ms");
        ImGui::InputInt(u8"最小伤害", &config.triggerbot[currentWeapon].minDamage);
		config.triggerbot[currentWeapon].minDamage = std::clamp(config.triggerbot[currentWeapon].minDamage, 0, 250);
		ImGui::Checkbox(u8"一击致命", &config.triggerbot[currentWeapon].killshot);
        
        if (!config.style.menuStyle)
            ImGui::End();
    }
}

void GUI::renderBacktrackWindow() noexcept
{
    if (window.backtrack) {
        if (!config.style.menuStyle) {
			ImGui::SetNextWindowSize({ 0.0f, 0.0f });
			ImGui::Begin(u8"后背跟踪", &window.backtrack, windowFlags);
        }
		ImGui::Checkbox(u8"启用", &config.backtrack.enabled);
		ImGui::Checkbox(u8"忽视烟雾", &config.backtrack.ignoreSmoke);
		ImGui::Checkbox(u8"后坐力视角", &config.backtrack.recoilBasedFov);
        ImGui::PushItemWidth(220.0f);
        ImGui::SliderInt("", &config.backtrack.timeLimit, 1, 200, u8"时间限制: %d ms");
        ImGui::PopItemWidth();
        if (!config.style.menuStyle)
            ImGui::End();
    }
}

void GUI::renderGlowWindow() noexcept
{
    if (window.glow) {
        if (!config.style.menuStyle) {
			ImGui::SetNextWindowSize({ 450.0f, 0.0f });
			ImGui::Begin(u8"发光", &window.glow, windowFlags);
        }
        static int currentCategory{ 0 };
        ImGui::PushItemWidth(110.0f);
        ImGui::PushID(0);
		ImGui::Combo("", &currentCategory, u8"队友\0敌人\0植物\0C4引信\0本地玩家\0武器\0C4\0安放的C4\0鸡\0拆弹装备\0投掷物\0人质\0精灵\0");
		ImGui::PopID();
        static int currentItem{ 0 };
        if (currentCategory <= 3) {
            ImGui::SameLine();
            static int currentType{ 0 };
            ImGui::PushID(1);
			ImGui::Combo("", &currentType, u8"所有\0可视的\0被挡住的\0");
			ImGui::PopID();
            currentItem = currentCategory * 3 + currentType;
        }
        else {
            currentItem = currentCategory + 8;
        }

        ImGui::SameLine();
		ImGui::Checkbox(u8"启用", &config.glow[currentItem].enabled);
		ImGui::Separator();
		ImGui::Columns(2, nullptr, false);
		ImGui::SetColumnOffset(1, 150.0f);
		ImGui::Checkbox(u8"基于生命值", &config.glow[currentItem].healthBased);
		ImGui::Checkbox(u8"彩虹发光", &config.glow[currentItem].rainbow);
		bool openPopup = ImGui::ColorButton(u8"颜色", ImVec4{ config.glow[currentItem].color }, ImGuiColorEditFlags_NoTooltip);
		ImGui::SameLine(0.0f, 5.0f);
		ImGui::TextUnformatted(u8"颜色");
		ImGui::PushID(2);
		if (openPopup)
			ImGui::OpenPopup("");
		if (ImGui::BeginPopup("")) {
			ImGui::PushID(3);
			ImGui::ColorPicker3("", config.glow[currentItem].color, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoSidePreview);
			ImGui::PopID();
			ImGui::EndPopup();
		}
        ImGui::PopID();
        ImGui::NextColumn();
        ImGui::PushItemWidth(220.0f);
		ImGui::SliderFloat(u8"厚度", &config.glow[currentItem].thickness, 0.0f, 1.0f, "%.2f");
		ImGui::SliderFloat(u8"透明度", &config.glow[currentItem].alpha, 0.0f, 1.0f, "%.2f");
		ImGui::SliderInt(u8"样式", &config.glow[currentItem].style, 0, 3);
        ImGui::Columns(1);
        if (!config.style.menuStyle)
            ImGui::End();
    }
}

void GUI::renderChamsWindow() noexcept
{
    if (window.chams) {
		if (!config.style.menuStyle) {
			ImGui::SetNextWindowSize({ 0.0f, 0.0f });
			ImGui::Begin(u8"人物上色", &window.chams, windowFlags);
		}
        static int currentCategory{ 0 };
        ImGui::PushItemWidth(110.0f);
        ImGui::PushID(0);
		ImGui::Combo("", &currentCategory, u8"队友\0敌人\0植物\0C4引信\0本地玩家\0武器\0手\0后背追踪\0");
		ImGui::PopID();
        static int currentItem{ 0 };

        if (currentCategory <= 3) {
            ImGui::SameLine();
            static int currentType{ 0 };
            ImGui::PushID(1);
			ImGui::Combo("", &currentType, u8"所有\0可视的\0被挡住的\0");
			ImGui::PopID();
            currentItem = currentCategory * 3 + currentType;
        }
        else {
            currentItem = currentCategory + 8;
        }

        ImGui::SameLine();
        static auto material{ 1 };
        ImGui::InputInt("##mat", &material, 1, 2);
        material = std::clamp(material, 1, 2);
        ImGui::SameLine();
        ImGui::Checkbox(u8"启用", &config.chams[currentItem].materials[material - 1].enabled);
        ImGui::Separator();
        ImGui::Checkbox(u8"基于生命", &config.chams[currentItem].materials[material - 1].healthBased);
        ImGui::Checkbox(u8"彩虹效果", &config.chams[currentItem].materials[material - 1].rainbow);
        ImGui::Checkbox(u8"闪烁", &config.chams[currentItem].materials[material - 1].blinking);
        ImGui::Combo(u8"材质", &config.chams[currentItem].materials[material - 1].material, u8"正常\0平面的\0动画效果\0白金材质\0玻璃材质\0铬合金材质\0水晶材质\0金属材质\0黄金材质\0塑料材质\0");
        ImGui::Checkbox(u8"线框", &config.chams[currentItem].materials[material - 1].wireframe);

        bool openPopup = ImGui::ColorButton(u8"颜色", ImVec4{ config.chams[currentItem].materials[material - 1].color }, ImGuiColorEditFlags_NoTooltip);
        ImGui::SameLine(0.0f, 5.0f);
        ImGui::TextUnformatted(u8"颜色");
        ImGui::PushID(2);
        if (openPopup)
            ImGui::OpenPopup("");
        if (ImGui::BeginPopup("")) {
            ImGui::PushID(3);
            ImGui::ColorPicker3("", config.chams[currentItem].materials[material - 1].color, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoSidePreview);
            ImGui::PopID();
            ImGui::EndPopup();
        }
        ImGui::PopID();
        ImGui::PushItemWidth(220.0f);
        ImGui::PushID(4);
        ImGui::SliderFloat("", &config.chams[currentItem].materials[material - 1].alpha, 0.0f, 1.0f, u8"透明度: %.2f");
        ImGui::PopID();
        if (!config.style.menuStyle) {
            ImGui::End();
        }
    }
}

void GUI::renderEspWindow() noexcept
{
    if (window.esp) {
        if (!config.style.menuStyle) {
            ImGui::SetNextWindowSize({ 500.0f, 0.0f });
            ImGui::Begin(u8"透视", &window.esp, windowFlags);
        }
        
        static int currentCategory = 0;
        static int currentItem = 0;

        if (ImGui::ListBoxHeader("##", { 100.0f, 250.0f })) {
            static constexpr const char* players[]{ u8"所有", u8"可见的", u8"不可见的" };
            
            ImGui::Text(u8"队友");
            ImGui::Indent();
            ImGui::PushID(u8"队友");

            for (int i = 0; i < IM_ARRAYSIZE(players); i++) {
                bool isSelected = currentCategory == 0 && currentItem == i;

                if ((i == 0 || !config.esp.players[0].enabled) && ImGui::Selectable(players[i], isSelected)) {
                    currentItem = i;
                    currentCategory = 0;
                }
            }

            ImGui::PopID();
            ImGui::Unindent();
            ImGui::Text(u8"敌人");
            ImGui::Indent();
            ImGui::PushID(u8"敌人");

            for (int i = 0; i < IM_ARRAYSIZE(players); i++) {
                bool isSelected = currentCategory == 1 && currentItem == i;

                if ((i == 0 || !config.esp.players[3].enabled) && ImGui::Selectable(players[i], isSelected)) {
                    currentItem = i;
                    currentCategory = 1;
                }
            }
            ImGui::PopID();
            ImGui::Unindent();
            if (bool isSelected = currentCategory == 2; ImGui::Selectable(u8"武器", isSelected))
                currentCategory = 2;

            ImGui::Text(u8"危险区域");
            ImGui::Indent();
            ImGui::PushID(u8"危险区域");
            static constexpr const char* dangerZone[]{ u8"哨兵(哨塔)", u8"无人机" };

            for (int i = 0; i < IM_ARRAYSIZE(dangerZone); i++) {
                bool isSelected = currentCategory == 3 && currentItem == i;

                if (ImGui::Selectable(dangerZone[i], isSelected)) {
                    currentItem = i;
                    currentCategory = 3;
                }
            }

            ImGui::PopID();
            ImGui::ListBoxFooter();
        }
        ImGui::SameLine();
        if (ImGui::BeginChild("##child", { 400.0f, 0.0f })) {
            switch (currentCategory) {
            case 0:
            case 1: {
                int selected = currentCategory * 3 + currentItem;
                ImGui::Checkbox(u8"启用", &config.esp.players[selected].enabled);
                ImGui::SameLine(0.0f, 50.0f);
                ImGui::SetNextItemWidth(85.0f);
                ImGui::InputInt(u8"字体", &config.esp.players[selected].font, 1, 294);
                config.esp.players[selected].font = std::clamp(config.esp.players[selected].font, 1, 294);

                ImGui::Separator();

                constexpr auto spacing{ 200.0f };
                checkboxedColorPicker(u8"追踪线条", &config.esp.players[selected].snaplines, config.esp.players[selected].snaplinesColor);
                ImGui::SameLine(spacing);
                checkboxedColorPicker(u8"方框", &config.esp.players[selected].box, config.esp.players[selected].boxColor);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(95.0f);
                ImGui::Combo("", &config.esp.players[selected].boxType, u8"2D\0""2D 四角\0""3D\0""3D 八角\0");
                checkboxedColorPicker(u8"视线追踪", &config.esp.players[selected].eyeTraces, config.esp.players[selected].eyeTracesColor);
                ImGui::SameLine(spacing);
                checkboxedColorPicker(u8"生命透视", &config.esp.players[selected].health, config.esp.players[selected].healthColor);
                checkboxedColorPicker(u8"头上打点", &config.esp.players[selected].headDot, config.esp.players[selected].headDotColor);
                ImGui::SameLine(spacing);
                checkboxedColorPicker(u8"生命长条", &config.esp.players[selected].healthBar, config.esp.players[selected].healthBarColor);
                checkboxedColorPicker(u8"名字", &config.esp.players[selected].name, config.esp.players[selected].nameColor);
                ImGui::SameLine(spacing);
                checkboxedColorPicker(u8"护甲透视", &config.esp.players[selected].armor, config.esp.players[selected].armorColor);
                checkboxedColorPicker(u8"钱数", &config.esp.players[selected].money, config.esp.players[selected].moneyColor);
                ImGui::SameLine(spacing);
                checkboxedColorPicker(u8"护甲长条", &config.esp.players[selected].armorBar, config.esp.players[selected].armorBarColor);
                checkboxedColorPicker(u8"轮廓线", &config.esp.players[selected].outline, config.esp.players[selected].outlineColor);
                ImGui::SameLine(spacing);
                checkboxedColorPicker(u8"距离透视", &config.esp.players[selected].distance, config.esp.players[selected].distanceColor);
				ImGui::PushItemWidth(240.0f);
				ImGui::PushID(0);
				ImGui::SliderInt("", &config.esp.players[selected].maxEspDistance, 0, 10000, u8"最大透视距离: %d");
				ImGui::PopID();
				ImGui::PushID(1);
				ImGui::SliderInt("", &config.esp.players[selected].maxsnaplineDistance, 0, 10000, u8"最大追踪线距离: %d");
				ImGui::PopID();

                break;
            }
            case 2: {
                ImGui::Checkbox(u8"启用", &config.esp.weapon.enabled);
                ImGui::SameLine(0.0f, 50.0f);
                ImGui::SetNextItemWidth(85.0f);
                ImGui::InputInt(u8"字体", &config.esp.weapon.font, 1, 294);
                config.esp.weapon.font = std::clamp(config.esp.weapon.font, 1, 294);

                ImGui::Separator();

                constexpr auto spacing{ 200.0f };
                checkboxedColorPicker(u8"追踪线条", &config.esp.weapon.snaplines, config.esp.weapon.snaplinesColor);
                ImGui::SameLine(spacing);
                checkboxedColorPicker(u8"方框", &config.esp.weapon.box, config.esp.weapon.boxColor);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(95.0f);
                ImGui::Combo("", &config.esp.weapon.boxType, u8"2D\0""2D 四角\0""3D\0""3D 八角\0");
                checkboxedColorPicker(u8"名字", &config.esp.weapon.name, config.esp.weapon.nameColor);
                ImGui::SameLine(spacing);
                checkboxedColorPicker(u8"轮廓线", &config.esp.weapon.outline, config.esp.weapon.outlineColor);
                checkboxedColorPicker(u8"距离", &config.esp.weapon.distance, config.esp.weapon.distanceColor);
                break;
            }
            case 3: {
                int selected = currentItem;
                ImGui::Checkbox(u8"启用", &config.esp.dangerZone[selected].enabled);
                ImGui::SameLine(0.0f, 50.0f);
                ImGui::SetNextItemWidth(85.0f);
                ImGui::InputInt(u8"字体", &config.esp.dangerZone[selected].font, 1, 294);
                config.esp.dangerZone[selected].font = std::clamp(config.esp.dangerZone[selected].font, 1, 294);

                ImGui::Separator();

                constexpr auto spacing{ 200.0f };
                checkboxedColorPicker(u8"追踪线条", &config.esp.dangerZone[selected].snaplines, config.esp.dangerZone[selected].snaplinesColor);
                ImGui::SameLine(spacing);
                checkboxedColorPicker(u8"方框", &config.esp.dangerZone[selected].box, config.esp.dangerZone[selected].boxColor);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(95.0f);
                ImGui::Combo("", &config.esp.dangerZone[selected].boxType, u8"2D\0""2D 四角\0""3D\0""3D 八角\0");
                checkboxedColorPicker(u8"姓名", &config.esp.dangerZone[selected].name, config.esp.dangerZone[selected].nameColor);
                ImGui::SameLine(spacing);
                checkboxedColorPicker(u8"轮廓线", &config.esp.dangerZone[selected].outline, config.esp.dangerZone[selected].outlineColor);
                checkboxedColorPicker(u8"距离", &config.esp.dangerZone[selected].distance, config.esp.dangerZone[selected].distanceColor);
                break;
            } }

            ImGui::EndChild();
        }

        if (!config.style.menuStyle)
            ImGui::End();
    }
}

void GUI::renderVisualsWindow() noexcept
{
    if (window.visuals) {
        if (!config.style.menuStyle) {
            ImGui::SetNextWindowSize({ 520.0f, 0.0f });
			ImGui::Begin(u8"渲染类", &window.visuals, windowFlags);
        }
        ImGui::Columns(2, nullptr, false);
        ImGui::SetColumnOffset(1, 210.0f);
		ImGui::Checkbox(u8"(专业) 禁用后处理(post-processing)", &config.visuals.disablePostProcessing);
		ImGui::Checkbox(u8"(专业) 反转ragdoll重力", &config.visuals.inverseRagdollGravity);
		ImGui::Checkbox(u8"无视雾霾", &config.visuals.noFog);
		ImGui::Checkbox(u8"禁用3D天空", &config.visuals.no3dSky);
		ImGui::Checkbox(u8"没有被打的抖动", &config.visuals.noAimPunch);
		ImGui::Checkbox(u8"没有视角抖动", &config.visuals.noViewPunch);
		ImGui::Checkbox(u8"删除手臂", &config.visuals.noHands);
		ImGui::Checkbox(u8"删除袖子", &config.visuals.noSleeves);
		ImGui::Checkbox(u8"删除武器", &config.visuals.noWeapons);
		ImGui::Checkbox(u8"无视烟雾", &config.visuals.noSmoke);
		ImGui::Checkbox(u8"无视模糊", &config.visuals.noBlur);
		ImGui::Checkbox(u8"无视开镜的遮挡", &config.visuals.noScopeOverlay);
		ImGui::Checkbox(u8"除草", &config.visuals.noGrass);
		ImGui::Checkbox(u8"关闭阴影", &config.visuals.noShadows);
		ImGui::Checkbox(u8"烟雾线框", &config.visuals.wireframeSmoke);
		ImGui::NextColumn();
		ImGui::Checkbox(u8"变焦", &config.visuals.zoom);
		ImGui::SameLine();
		hotkey(config.visuals.zoomKey);
		ImGui::Checkbox(u8"第三人称", &config.visuals.thirdperson);
        ImGui::SameLine();
        hotkey(config.visuals.thirdpersonKey);
        ImGui::PushItemWidth(290.0f);
        ImGui::PushID(0);
		ImGui::SliderInt("", &config.visuals.thirdpersonDistance, 0, 1000, u8"第三人称距离: %d");
		ImGui::PopID();
		ImGui::PushID(1);
		ImGui::SliderInt("", &config.visuals.viewmodelFov, -60, 60, u8"视图模型视野: %d");
		ImGui::PopID();
		ImGui::PushID(2);
		ImGui::SliderInt("", &config.visuals.fov, -60, 60, u8"视野: %d");
		ImGui::PopID();
		ImGui::PushID(3);
		ImGui::SliderInt("", &config.visuals.farZ, 0, 2000, u8"最远渲染距离: %d");
		ImGui::PopID();
		ImGui::PushID(4);
		ImGui::SliderInt("", &config.visuals.flashReduction, 0, 100, u8"闪光减小度: %d%%");
		ImGui::PopID();
		ImGui::PushID(5);
		ImGui::SliderFloat("", &config.visuals.brightness, 0.0f, 1.0f, u8"明亮度: %.2f");
        ImGui::PopID();
        ImGui::PopItemWidth();
		ImGui::Combo(u8"天空盒", &config.visuals.skybox, u8"默认\0cs_baggage_skybox_\0cs_tibet\0embassy\0italy\0jungle\0nukeblank\0office\0sky_cs15_daylight01_hdr\0sky_cs15_daylight02_hdr\0sky_cs15_daylight03_hdr\0sky_cs15_daylight04_hdr\0sky_csgo_cloudy01\0sky_csgo_night_flat\0sky_csgo_night02\0sky_day02_05_hdr\0sky_day02_05\0sky_dust\0sky_l4d_rural02_ldr\0sky_venice\0vertigo_hdr\0vertigo\0vertigoblue_hdr\0vietnam\0");
		ImGui::ColorEdit3(u8"世界颜色", config.visuals.worldColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip);
		ImGui::Checkbox(u8"Deagle spinner", &config.visuals.deagleSpinner);
		ImGui::Combo(u8"屏幕效果", &config.visuals.screenEffect, u8"无\0Drone cam\0Drone cam with noise\0Underwater\0Healthboost\0Dangerzone\0");
		ImGui::Combo(u8"打击标记", &config.visuals.hitMarker, u8"无\0Drone cam\0Drone cam with noise\0Underwater\0Healthboost\0Dangerzone\0");
		ImGui::SliderFloat(u8"打击标记时间", &config.visuals.hitMarkerTime, 0.1f, 1.5f, "%.2fs");
		ImGui::Columns(1);

        if (!config.style.menuStyle)
            ImGui::End();
    }
}

void GUI::renderSkinChangerWindow() noexcept
{
    if (window.skinChanger) {
        if (!config.style.menuStyle) {
            ImGui::SetNextWindowSize({ 700.0f, 0.0f });
            ImGui::Begin("nSkinz", &window.skinChanger, windowFlags);
        } 

        static auto itemIndex = 0;

        ImGui::PushItemWidth(110.0f);
        ImGui::Combo("##1", &itemIndex, [](void* data, int idx, const char** out_text) {
            *out_text = game_data::weapon_names[idx].name;
            return true;
        }, nullptr, IM_ARRAYSIZE(game_data::weapon_names), 5);
        ImGui::PopItemWidth();

        auto& selected_entry = config.skinChanger[itemIndex];
        selected_entry.definition_vector_index = itemIndex;

        {
            ImGui::SameLine();
            ImGui::Checkbox("Enabled", &selected_entry.enabled);
            ImGui::Separator();
            ImGui::Columns(2, nullptr, false);
            ImGui::InputInt("Seed", &selected_entry.seed);
            ImGui::InputInt("StatTrak", &selected_entry.stat_trak);
            ImGui::SliderFloat("Wear", &selected_entry.wear, FLT_MIN, 1.f, "%.10f", 5);

            ImGui::Combo("Paint Kit", &selected_entry.paint_kit_vector_index, [](void* data, int idx, const char** out_text) {
                *out_text = (itemIndex == 1 ? SkinChanger::gloveKits : SkinChanger::skinKits)[idx].name.c_str();
                return true;
            }, nullptr, (itemIndex == 1 ? SkinChanger::gloveKits : SkinChanger::skinKits).size(), 10);

            ImGui::Combo("Quality", &selected_entry.entity_quality_vector_index, [](void* data, int idx, const char** out_text) {
                *out_text = game_data::quality_names[idx].name;
                return true;
            }, nullptr, IM_ARRAYSIZE(game_data::quality_names), 5);

            if (itemIndex == 0) {
                ImGui::Combo("Knife", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text) {
                    *out_text = game_data::knife_names[idx].name;
                    return true;
                }, nullptr, IM_ARRAYSIZE(game_data::knife_names), 5);
            } else if (itemIndex == 1) {
                ImGui::Combo("Glove", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text) {
                    *out_text = game_data::glove_names[idx].name;
                    return true;
                }, nullptr, IM_ARRAYSIZE(game_data::glove_names), 5);
            } else {
                static auto unused_value = 0;
                selected_entry.definition_override_vector_index = 0;
                ImGui::Combo("Unavailable", &unused_value, "For knives or gloves\0");
            }

            ImGui::InputText("Name Tag", selected_entry.custom_name, 32);
        }

        ImGui::NextColumn();

        {
            ImGui::PushID("sticker");

            static auto selectedStickerSlot = 0;

            ImGui::PushItemWidth(-1);

            ImGui::ListBox("", &selectedStickerSlot, [](void* data, int idx, const char** out_text) {
                static char elementName[64];
                auto kit_vector_index = config.skinChanger[itemIndex].stickers[idx].kit_vector_index;
                sprintf_s(elementName, "#%d (%s)", idx + 1, SkinChanger::stickerKits[kit_vector_index].name.c_str());
                *out_text = elementName;
                return true;
            }, nullptr, 5, 5);

            ImGui::PopItemWidth();

            auto& selected_sticker = selected_entry.stickers[selectedStickerSlot];

            ImGui::Combo("Sticker Kit", &selected_sticker.kit_vector_index, [](void* data, int idx, const char** out_text) {
                *out_text = SkinChanger::stickerKits[idx].name.c_str();
                return true;
            }, nullptr, SkinChanger::stickerKits.size(), 10);

            ImGui::SliderFloat("Wear", &selected_sticker.wear, FLT_MIN, 1.0f, "%.10f", 5.0f);
            ImGui::SliderFloat("Scale", &selected_sticker.scale, 0.1f, 5.0f);
            ImGui::SliderFloat("Rotation", &selected_sticker.rotation, 0.0f, 360.0f);

            ImGui::PopID();
        }
        selected_entry.update();

        ImGui::Columns(1);

        ImGui::Separator();

        if (ImGui::Button("Update", { 130.0f, 30.0f }))
            SkinChanger::scheduleHudUpdate();

        ImGui::TextUnformatted("nSkinz by namazso");

        if (!config.style.menuStyle)
            ImGui::End();
    }
}

void GUI::renderSoundWindow() noexcept
{
    if (window.sound) {
		if (!config.style.menuStyle) {
			ImGui::SetNextWindowSize({ 0.0f, 0.0f });
			ImGui::Begin(u8"声音类", &window.sound, windowFlags);
		}
		ImGui::SliderInt(u8"鸡的声音", &config.sound.chickenVolume, 0, 200, "%d%%");

		static int currentCategory{ 0 };
		ImGui::PushItemWidth(110.0f);
		ImGui::Combo("", &currentCategory, u8"本地玩家\0队友\0敌人\0");
		ImGui::PopItemWidth();
		ImGui::SliderInt(u8"主音量", &config.sound.players[currentCategory].masterVolume, 0, 200, "%d%%");
		ImGui::SliderInt(u8"爆头声音", &config.sound.players[currentCategory].headshotVolume, 0, 200, "%d%%");
		ImGui::SliderInt(u8"武器声音", &config.sound.players[currentCategory].weaponVolume, 0, 200, "%d%%");
		ImGui::SliderInt(u8"脚步声音", &config.sound.players[currentCategory].footstepVolume, 0, 200, "%d%%");

        if (!config.style.menuStyle)
            ImGui::End();
    }
}

void GUI::renderStyleWindow() noexcept
{
    if (window.style) {
        if (!config.style.menuStyle) {
            ImGui::SetNextWindowSize({ 0.0f, 0.0f });
            ImGui::Begin(u8"主题", &window.style, windowFlags);
        }

        ImGui::PushItemWidth(150.0f);
        if (ImGui::Combo(u8"菜单风格", &config.style.menuStyle, u8"经典\0单一窗口(不建议)\0"))
            window = { };
        if (ImGui::Combo("菜单颜色", &config.style.menuColors, u8"黑色风格\0白色风格\0古典风格\0自定义\0"))
            updateColors();
        ImGui::PopItemWidth();

        if (config.style.menuColors == 3) {
            ImGuiStyle& style = ImGui::GetStyle();
            for (int i = 0; i < ImGuiCol_COUNT; i++) {
                if (i && i % 4) ImGui::SameLine(220.0f * (i % 4));

                const char* name = ImGui::GetStyleColorName(i);
                ImGui::PushID(i);
                bool openPopup = ImGui::ColorButton("##colorbutton", style.Colors[i], ImGuiColorEditFlags_NoTooltip);
                ImGui::SameLine(0.0f, 5.0f);
                ImGui::TextUnformatted(name);
                if (openPopup)
                    ImGui::OpenPopup(name);
                if (ImGui::BeginPopup(name)) {
                    ImGui::ColorPicker3("##colorpicker", (float*)& style.Colors[i], ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoSidePreview);
                    ImGui::EndPopup();
                }
                ImGui::PopID();
            }
        }

        if (!config.style.menuStyle)
            ImGui::End();
    }
}

void GUI::renderMiscWindow() noexcept
{
    if (window.misc) {
        if (!config.style.menuStyle) {
            ImGui::SetNextWindowSize({ 580.0f, 0.0f });
            ImGui::Begin(u8"杂项", &window.misc, windowFlags);
        }
        ImGui::Columns(2, nullptr, false);
        ImGui::SetColumnOffset(1, 230.0f);
		ImGui::TextUnformatted(u8"呼出菜单热键");
        ImGui::SameLine();
        hotkey(config.misc.menuKey);

		ImGui::Checkbox(u8"显示附近人数",&config.misc.showRoundPeople);
		ImGui::Checkbox(u8"防挂机踢人", &config.misc.antiAfkKick);
		ImGui::Checkbox(u8"自动扫射", &config.misc.autoStrafe);
		ImGui::Checkbox(u8"自动连跳", &config.misc.bunnyHop);
		ImGui::Checkbox(u8"快速闪避", &config.misc.fastDuck);
		ImGui::Checkbox(u8"月球行走", &config.misc.moonwalk);
		ImGui::Checkbox(u8"狙击准星", &config.misc.sniperCrosshair);
		ImGui::Checkbox(u8"后坐力准星", &config.misc.recoilCrosshair);
		ImGui::Checkbox(u8"自动打手枪", &config.misc.autoPistol);
		ImGui::Checkbox(u8"自动重载", &config.misc.autoReload);
		ImGui::Checkbox(u8"自动同意", &config.misc.autoAccept);
		ImGui::Checkbox(u8"雷达透视", &config.misc.radarHack);
		ImGui::Checkbox(u8"显示排名", &config.misc.revealRanks);
		ImGui::Checkbox(u8"显示观战者", &config.misc.spectatorList);
		ImGui::Checkbox(u8"水印", &config.misc.watermark);
		ImGui::Checkbox(u8"修复动画LOD", &config.misc.fixAnimationLOD);
		ImGui::Checkbox(u8"修复骨骼矩阵", &config.misc.fixBoneMatrix);
		ImGui::Checkbox(u8"修复移动", &config.misc.fixMovement);
		ImGui::Checkbox(u8"禁用模型遮挡", &config.misc.disableModelOcclusion);
		ImGui::NextColumn();
		ImGui::Checkbox(u8"队伍标识动画", &config.misc.animatedClanTag);
		ImGui::Checkbox(u8"时钟标识", &config.misc.clocktag);
		ImGui::Checkbox(u8"自定义队名", &config.misc.customClanTag);
        ImGui::SameLine();
        ImGui::PushItemWidth(120.0f);
        ImGui::PushID(0);
        if (ImGui::InputText("", config.misc.clanTag, IM_ARRAYSIZE(config.misc.clanTag)))
            Misc::updateClanTag(true);
        ImGui::PopID();
		ImGui::Checkbox(u8"杀人信息", &config.misc.killMessage);
        ImGui::SameLine();
        ImGui::PushItemWidth(120.0f);
        ImGui::PushID(1);
        ImGui::InputText("", config.misc.killMessageString, IM_ARRAYSIZE(config.misc.killMessageString));
        ImGui::PopID();
		ImGui::Checkbox(u8"随机更改为队友名字", &config.misc.nameStealer);
        ImGui::PushID(2);
        ImGui::InputText("", config.misc.voteText, IM_ARRAYSIZE(config.misc.voteText));
        ImGui::PopID();
        ImGui::SameLine();
		if (ImGui::Button(u8"启用虚假投票"))
            Misc::fakeVote(true);

        ImGui::PushID(3);
        ImGui::SetNextItemWidth(100.0f);
		ImGui::Combo("", &config.misc.banColor, u8"白色\0红色\0紫色\0绿色\0亮绿\0青色\0亮红\0灰色\0黄色\0灰色2\0亮蓝\0灰色/紫色\0蓝色\0粉色\0深橙色\0橙色\0");
		ImGui::PopID();
        ImGui::SameLine();
        ImGui::PushID(4);
        ImGui::InputText("", config.misc.banText, IM_ARRAYSIZE(config.misc.banText));
        ImGui::PopID();
        ImGui::SameLine();
		if (ImGui::Button(u8"启用虚假ban"))
            Misc::fakeBan(true);
		ImGui::Checkbox(u8"快速安C4", &config.misc.fastPlant);
		ImGui::Checkbox(u8"显示C4时间", &config.misc.bombTimer);
		ImGui::Checkbox(u8"快速装弹", &config.misc.quickReload);
		ImGui::Checkbox(u8"自动准备左轮手枪", &config.misc.prepareRevolver);
        ImGui::SameLine();
        hotkey(config.misc.prepareRevolverKey);
		ImGui::Combo(u8"击中音效", &config.misc.hitSound, u8"无\0金属\0Gamesense\0铃声\0草\0");
		ImGui::PushItemWidth(90.0f);
		ImGui::InputInt(u8"阻塞数据包", &config.misc.chokedPackets, 1, 5);
        config.misc.chokedPackets = std::clamp(config.misc.chokedPackets, 0, 64);
        ImGui::SameLine();
        hotkey(config.misc.chokedPacketsKey);
        ImGui::Text(u8"快速生命射击");
        ImGui::SameLine();
        hotkey(config.misc.quickHealthshotKey);
		ImGui::Checkbox(u8"手雷预测", &config.misc.nadePredict);
		ImGui::PushItemWidth(120.0f);
		ImGui::SliderFloat(u8"最大更改角度", &config.misc.maxAngleDelta, 0.0f, 255.0f, "%.2f");
        ImGui::PushItemWidth(290.0f);

		if (ImGui::Button(u8"取消hook"))
            hooks.restore();

        ImGui::Columns(1);
        if (!config.style.menuStyle)
            ImGui::End();
    }
}

void GUI::renderReportbotWindow() noexcept
{
    if (window.reportbot) {
        if (!config.style.menuStyle) {
            ImGui::SetNextWindowSize({ 0.0f, 0.0f });
			ImGui::Begin(u8"举报机器人", &window.reportbot, windowFlags);
        }
		ImGui::Checkbox(u8"启用", &config.reportbot.enabled);
		ImGui::Combo(u8"目标", &config.reportbot.target, u8"敌人\0队友\0全部\0");
		ImGui::InputInt(u8"间隔时间(秒)", &config.reportbot.delay, 1, 5);
		config.reportbot.delay = (std::max)(config.reportbot.delay, 0);
		ImGui::Checkbox(u8"自瞄举报", &config.reportbot.aimbot);
		ImGui::Checkbox(u8"透视举报", &config.reportbot.wallhack);
		ImGui::Checkbox(u8"其他举报", &config.reportbot.other);
		ImGui::Checkbox(u8"恶意破坏举报", &config.reportbot.griefing);
		ImGui::Checkbox(u8"言语辱骂举报", &config.reportbot.voiceAbuse);
		ImGui::Checkbox(u8"文字辱骂举报", &config.reportbot.textAbuse);
        if (!config.style.menuStyle)
            ImGui::End();
    }
}

void GUI::renderConfigWindow() noexcept
{
    if (window.config) {
        if (!config.style.menuStyle) {
            ImGui::SetNextWindowSize({ 290.0f, 190.0f });
			ImGui::Begin(u8"配置文件(记得保存配置!)", &window.config, windowFlags);
        }

        ImGui::Columns(2, nullptr, false);
        ImGui::SetColumnOffset(1, 170.0f);

        ImGui::PushItemWidth(160.0f);

        constexpr auto& configItems = config.getConfigs();
        static int currentConfig = -1;

        if (static_cast<size_t>(currentConfig) >= configItems.size())
            currentConfig = -1;

        static char buffer[16];

        if (ImGui::ListBox("", &currentConfig, [](void* data, int idx, const char** out_text) {
            auto& vector = *static_cast<std::vector<std::string>*>(data);
            *out_text = vector[idx].c_str();
            return true;
        }, &configItems, configItems.size(), 5) && currentConfig != -1)
            strcpy(buffer, configItems[currentConfig].c_str());

        ImGui::PushID(0);
        if (ImGui::InputText("", buffer, IM_ARRAYSIZE(buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
            if (currentConfig != -1)
                config.rename(currentConfig, buffer);
        }
        ImGui::PopID();
        ImGui::NextColumn();

        ImGui::PushItemWidth(100.0f);

		if (ImGui::Button(u8"新建配置", { 100.0f, 25.0f }))
			config.add(buffer);

		if (ImGui::Button(u8"重置配置", { 100.0f, 25.0f }))
			ImGui::OpenPopup(u8"要重置的配置");

        if (ImGui::BeginPopup(u8"要重置的配置")) {
			static constexpr const char* names[]{ u8"全部", u8"自瞄1", u8"自瞄2", u8"后背追踪", u8"陀螺", u8"发光", u8"上色", u8"透视", u8"渲染类", u8"皮肤更改", u8"声音类", u8"风格类", u8"杂项", u8"举报机器人" };
			for (int i = 0; i < IM_ARRAYSIZE(names); i++) {
                if (i == 1) ImGui::Separator();

                if (ImGui::Selectable(names[i])) {
                    switch (i) {
                    case 0: config.reset(); updateColors(); Misc::updateClanTag(true); SkinChanger::scheduleHudUpdate(); break;
                    case 1: config.aimbot = { }; break;
                    case 2: config.triggerbot = { }; break;
                    case 3: config.backtrack = { }; break;
                    case 4: config.antiAim = { }; break;
                    case 5: config.glow = { }; break;
                    case 6: config.chams = { }; break;
                    case 7: config.esp = { }; break;
                    case 8: config.visuals = { }; break;
                    case 9: config.skinChanger = { }; SkinChanger::scheduleHudUpdate(); break;
                    case 10: config.sound = { }; break;
                    case 11: config.style = { }; updateColors(); break;
                    case 12: config.misc = { };  Misc::updateClanTag(true); break;
                    case 13: config.reportbot = { }; break;
                    }
                }
            }
            ImGui::EndPopup();
        }
        if (currentConfig != -1) {
			if (ImGui::Button(u8"载入选中项", { 100.0f, 25.0f })) {
                config.load(currentConfig);
                updateColors();
                SkinChanger::scheduleHudUpdate();
                Misc::updateClanTag(true);
            }
			if (ImGui::Button(u8"保存选中项", { 100.0f, 25.0f }))
                config.save(currentConfig);
			if (ImGui::Button(u8"删除选中项", { 100.0f, 25.0f }))
                config.remove(currentConfig);
        }
        ImGui::Columns(1);
        if (!config.style.menuStyle)
            ImGui::End();
    }
}

void GUI::renderGuiStyle2() noexcept
{
    ImGui::SetNextWindowSize({ 800.0f, 0.0f });
    ImGui::Begin("BobHCsgo", nullptr, windowFlags | ImGuiWindowFlags_NoTitleBar);

    if (ImGui::BeginTabBar("TabBar", ImGuiTabBarFlags_Reorderable)) {
        if (ImGui::BeginTabItem("Aimbot")) {
            window = { };
            window.aimbot = true;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Anti aim")) {
            window = { };
            window.antiAim = true;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Triggerbot")) {
            window = { };
            window.triggerbot = true;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Backtrack")) {
            window = { };
            window.backtrack = true;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Glow")) {
            window = { };
            window.glow = true;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Chams")) {
            window = { };
            window.chams = true;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Esp")) {
            window = { };
            window.esp = true;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Visuals")) {
            window = { };
            window.visuals = true;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Skin changer")) {
            window = { };
            window.skinChanger = true;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Sound")) {
            window = { };
            window.sound = true;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Style")) {
            window = { };
            window.style = true;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Misc")) {
            window = { };
            window.misc = true;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Reportbot")) {
            window = { };
            window.reportbot = true;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Config")) {
            window = { };
            window.config = true;
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    renderAimbotWindow();
    renderAntiAimWindow();
    renderTriggerbotWindow();
    renderBacktrackWindow();
    renderGlowWindow();
    renderChamsWindow();
    renderEspWindow();
    renderVisualsWindow();
    renderSkinChangerWindow();
    renderSoundWindow();
    renderStyleWindow();
    renderMiscWindow();
    renderReportbotWindow();
    renderConfigWindow();

    ImGui::End();
}
