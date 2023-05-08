#ifndef MENU
#define MENU

#include "Misc/ESP.h"
#include "Unity/Screen.h"

namespace Menu {
    ImVec4 color = ImVec4(1, 1, 1, 1);
    static bool aimbot = false;
    static bool lineEsp = false;
    static bool triggerBot = false;
    static int screenX, screenY;
    Vector3 GetPosition(void *playerScript) {
        auto transform = Pointers::Component_Transform[playerScript].get();
        if (!transform) {
            return Vector3::zero;
        }
        auto location = Pointers::Transform_Position[transform].get();
        return location;
    }

    void *GetMyPlayerScript() {
        auto myPlayer = Pointers::PhotonNetwork_player.get();
        if (!myPlayer) {
            return nullptr;
        }
        void *myPlayerScript = Pointers::PhotonPlayer_playerScript[myPlayer].get();
        if (!BNM::IsUnityObjectAlive(myPlayer)) {
            return nullptr;
        }
        return myPlayerScript;
    }

    Vector3 GetMyPosition() {
        auto myPlayer = GetMyPlayerScript();
        if (!BNM::IsUnityObjectAlive(myPlayer)) {
            return Vector3::zero;
        }
        return GetPosition(myPlayer);
    }

    std::vector<void *> GetPlayerList() {
        std::vector<void *> finalList = std::vector<void *>();

        auto myPlayer = Pointers::PhotonNetwork_player.get();
        if (!myPlayer) {
            return finalList;
        }

        void *myPlayerScript = Pointers::PhotonPlayer_playerScript[myPlayer].get();
        if (!BNM::IsUnityObjectAlive(myPlayerScript)) {
            return finalList;
        }

        auto playersMonoArray = Pointers::PhotonNetwork_otherPlayers.get();
        if (playersMonoArray) {
            auto players = playersMonoArray->toCPPlist();
            finalList = players;
        }

        return finalList;
    }

    void *GetClosestEnemy() {
        auto myPlayer = GetMyPlayerScript();
        if (!BNM::IsUnityObjectAlive(myPlayer)) {
            return nullptr;
        }
        std::vector<void *> playerList = GetPlayerList();
        auto myPos = GetMyPosition();
        auto closestDistance = 99999999;
        void *closestEnemy = nullptr;

        if (!playerList.empty()) {
            for (void *player: playerList) {
                if (!player) {
                    continue;
                }
                void *playerScript = Pointers::PhotonPlayer_playerScript[player].get();
                if (!BNM::IsUnityObjectAlive(playerScript)) {
                    continue;
                }
                if (Pointers::PlayerUtils_IsPlayerOnTeam.call(playerScript, myPlayer)) {
                    continue;
                }
                if (0 >= Pointers::PlayerScript_health[playerScript].get()) {
                    continue;
                }
                auto position = GetPosition(playerScript);
                if (position == Vector3::zero) {
                    continue;
                }
                auto distance = Vector3::Distance(myPos, position);

                if (distance < closestDistance) {
                    closestDistance = distance;
                    closestEnemy = playerScript;
                }
            }
        }

        return closestEnemy;
    }

    void ExecuteHacks() {
        if (aimbot) {
            auto myPlayer = GetMyPlayerScript();
            if (BNM::IsUnityObjectAlive(myPlayer)) {
                auto closestEnemy = GetClosestEnemy();
                if (BNM::IsUnityObjectAlive(closestEnemy)) {
                    auto myPos = GetMyPosition();
                    auto enemyPos = GetPosition(closestEnemy);
                    enemyPos -= Vector3(0, 0.5, 0);
                    auto rotation = Quaternion::LookRotation(enemyPos - myPos, Vector3(0.0, 0.1, 0.0));
                    Pointers::PlayerScript_rotation[myPlayer].set(rotation);
                }
            }
        }
        if (lineEsp) {
            auto myPlayer = GetMyPlayerScript();
            if (BNM::IsUnityObjectAlive(myPlayer)) {
                auto playerList = GetPlayerList();
                for (void *player: playerList) {
                    if (!player) {
                        continue;
                    }
                    void *playerScript = Pointers::PhotonPlayer_playerScript[player].get();
                    if (!BNM::IsUnityObjectAlive(playerScript)) {
                        continue;
                    }
                    bool myTeam = Pointers::PlayerUtils_IsPlayerOnTeam(GetMyPlayerScript(),
                                                                       playerScript);
                    auto espColor = myTeam ? ImVec4(0, 0, 1, 1) : ImVec4(1, 0, 0, 1);
                    auto worldPos = GetPosition(playerScript);
                    if (worldPos == Vector3::zero) {
                        continue;
                    }
                    auto camera = Pointers::PlayerScript_mCamera[myPlayer].get();
                    auto screenPos = Pointers::Camera_WorldToScreenPoint[camera].call(worldPos);
                    ESP::DrawLine(ImVec2(screenX / 2, 0),
                                  ImVec2((int) screenPos.x, screenY - (int) screenPos.y), espColor);
                }
            }
        }
        if (triggerBot) {
            auto myPlayer = GetMyPlayerScript();
            if (BNM::IsUnityObjectAlive(myPlayer)) {
                auto aimingAt = Pointers::PlayerScript_GetPlayerAimingAt[myPlayer].call();
                if (BNM::IsUnityObjectAlive(aimingAt)) {
                    if (!Pointers::PlayerUtils_IsPlayerOnTeam.call(aimingAt, myPlayer)) {
                        Pointers::PlayerScript_Shoot[myPlayer].call();
                    }
                }
            }
        }
    }

    void DrawMenu() {
        ExecuteHacks();
        static bool closed = false;
        ImGui::Begin("Toshi's Bullet Force (Auto Update)", &closed);
        ImGui::Checkbox("Aimbot", &aimbot);
        ImGui::Checkbox("Line ESP", &lineEsp);
        ImGui::Checkbox("TriggerBot", &triggerBot);
        ImGui::End();
    }

    void DrawImGui() {
        if (init && Unity::Screen::is_done) {

            ImGuiIO &io = ImGui::GetIO();

            static bool WantTextInputLast = false;
            if (io.WantTextInput && !WantTextInputLast)
                displayKeyboard(true);
            WantTextInputLast = io.WantTextInput;

            ImGui_ImplOpenGL3_NewFrame();

            ImGui_ImplAndroid_NewFrame(screenX, screenY);

            ImGui::NewFrame();
            DrawMenu();
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            io.KeysDown[io.KeyMap[ImGuiKey_UpArrow]] = false;
            io.KeysDown[io.KeyMap[ImGuiKey_DownArrow]] = false;
            io.KeysDown[io.KeyMap[ImGuiKey_LeftArrow]] = false;
            io.KeysDown[io.KeyMap[ImGuiKey_RightArrow]] = false;
            io.KeysDown[io.KeyMap[ImGuiKey_Tab]] = false;
            io.KeysDown[io.KeyMap[ImGuiKey_Enter]] = false;
            io.KeysDown[io.KeyMap[ImGuiKey_Backspace]] = false;
            io.KeysDown[io.KeyMap[ImGuiKey_PageUp]] = false;
            io.KeysDown[io.KeyMap[ImGuiKey_PageDown]] = false;
            io.KeysDown[io.KeyMap[ImGuiKey_Escape]] = false;
            io.KeysDown[io.KeyMap[ImGuiKey_Delete]] = false;
            io.KeysDown[io.KeyMap[ImGuiKey_Home]] = false;
            io.KeysDown[io.KeyMap[ImGuiKey_End]] = false;
            io.KeysDown[io.KeyMap[ImGuiKey_Insert]] = false;
            ImGui::EndFrame();

        }
    }
}
#endif MENU