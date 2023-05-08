#ifndef MENU
#define MENU

#include "Misc/ESP.h"
#include "Unity/Screen.h"

namespace Menu {
    ImVec4 color = ImVec4(1, 1, 1, 1);
    static int screenX, screenY;
    static bool aimbot = false;
    static bool lineEsp = false;

    Vector3 GetPosition(void *playerScript) {
        auto transform = Pointers::Component_Transform[playerScript].get();
        if (!transform) {
            return Vector3::zero;
        }
        auto location = Pointers::Transform_Position[transform].get();
        return location;
    }

    void* GetMyPlayerScript() {
        auto myPlayer = Pointers::PhotonNetwork_player.get();
        if (!myPlayer) {
            return nullptr;
        }
        void *myPlayerScript = Pointers::PhotonPlayer_playerScript[myPlayer].get();
        if (!myPlayerScript) {
            return nullptr;
        }
        return myPlayerScript;
    }

    Vector3 GetMyPosition() {
        auto myPlayer = GetMyPlayerScript();
        if (!myPlayer) {
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
        if (!myPlayerScript) {
            return finalList;
        }

        auto playersMonoArray = Pointers::PhotonNetwork_otherPlayers.get();
        if (playersMonoArray) {
            auto players = playersMonoArray->toCPPlist();
            finalList = players;
        }

        return finalList;
    }

    void* GetClosestEnemy() {
        std::vector<void *> playerList = GetPlayerList();

        auto myPlayer = GetMyPlayerScript();
        auto myPos = GetMyPosition();
        auto closestDistance = 99999999;
        void *closestEnemy = nullptr;

        if (!playerList.empty()) {
            for (void *player : playerList) {
                if (!player) {
                    continue;
                }
                void *playerScript = Pointers::PhotonPlayer_playerScript[player].get();
                if (!playerScript) {
                    continue;
                }
                if (Pointers::PlayerUtils_IsPlayerOnTeam.call(playerScript, myPlayer)) {
                    continue;
                }

                auto position = GetPosition(playerScript);
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
            if (myPlayer) {
                auto closestEnemy = GetClosestEnemy();
                if (closestEnemy) {
                    LOGD("Got closest player!");
                    auto myPos = GetMyPosition();
                    auto enemyPos = GetPosition(closestEnemy);
                    LOGD("My pos: %f, %f, %f", myPos.x, myPos.y, myPos.z);
                    LOGD("Enemy pos: %f, %f, %f", enemyPos.x, enemyPos.y, enemyPos.z);
                    auto screenPos = Pointers::Camera_WorldToScreenPoint[Pointers::Camera_Main.get()].call(enemyPos);
                    ESP::DrawLine(ImVec2(myPos.x, -10), ImVec2(screenPos->x, screenPos->y - Unity::Screen::Height.get()), ImVec4(1,0,1,1));
                    auto rotation = Quaternion::LookRotation(enemyPos - myPos, Vector3(0, 1, 0));
                    Pointers::PlayerScript_rotation[myPlayer].set(&rotation);
                }
            }
        } else if (lineEsp) {
            auto playerList = GetPlayerList();
            for (void *player: playerList) {
                if (!player) {
                    continue;
                }
                void *playerScript = Pointers::PhotonPlayer_playerScript[player].get();
                if (!playerScript) {
                    continue;
                }
                bool myTeam = Pointers::PlayerUtils_IsPlayerOnTeam(GetMyPlayerScript(), playerScript);
                auto espColor = myTeam ? ImVec4(0,0,1,1) : ImVec4(1,0,0,1);
                auto worldPos = GetPosition(playerScript);
                auto screenPos = Pointers::Camera_WorldToScreenPoint[Pointers::Camera_Main.get()].call(worldPos);
                ESP::DrawLine(ImVec2(screenX / 2, screenY + 10), ImVec2((int)screenPos->x,(int)screenPos->y),  espColor);
            }
        }
    }

    void DrawMenu() {
        LOGD("Here we are rendering the menu");
        ExecuteHacks();
        static bool closed = false;
        ImGui::Begin("Toshi's Bullet Force (Auto Update)", &closed);
        ImGui::Checkbox("Aimbot", &aimbot);
        ImGui::Checkbox("Line ESP", &lineEsp);
        ImGui::End();
    }

    void DrawImGui() {
        if (init && Unity::Screen::is_done) {
            screenX = Unity::Screen::Height.get();
            screenY = Unity::Screen::Width.get();

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
            LOGD("Here is the menu rendered");
        }
    }
}
#endif MENU