#ifndef MENU
#define MENU

#include "Misc/ESP.h"
#include "Unity/Screen.h"

namespace Menu {

    ImVec4 color = ImVec4(1, 1, 1, 1);
    static bool aimbot = false;


    Vector3 GetPosition(void *playerScript) {
        auto transform = Pointers::GameObject_Transform[playerScript].get();
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



    void* GetClosestEnemy() {
        auto myPlayer = Pointers::PhotonNetwork_player.get();
        if (!myPlayer) {
            LOGD("FAILED TO GET OWN PLAYER");
            return nullptr;
        }
        void *myPlayerScript = Pointers::PhotonPlayer_playerScript[myPlayer].get();
        if (!myPlayerScript) {
            LOGD("FAILED TO GET OWN PLAYERSCRIPT");
            return nullptr;
        }
        auto myPos = GetMyPosition();
        auto closestDistance = 99999999;
        void *closestEnemy = nullptr;
        auto playersMonoArray = Pointers::PhotonNetwork_otherPlayers.get();
        if (playersMonoArray) {
            LOGD("WE GOT THE LIST OF PLAYERS");
            auto players = playersMonoArray->toCPPlist();
            for (void *player: players) {
                LOGD("WE ARE LOOPING OVER A PLAYER");
                if (!player) {
                    continue;
                }
                void *playerScript = Pointers::PhotonPlayer_playerScript[player].get();
                if (!playerScript) {
                    continue;
                }
                LOGD("CHECKING IF THEY ARE ON OUR TEAM");
                if (Pointers::PlayerUtils_IsPlayerOnTeam.call(player, myPlayer)) {
                    continue;
                }
                auto transform = Pointers::GameObject_Transform[playerScript].get();
                if (!transform) {
                    continue;
                }
                LOGD("CHECKING THE TRANSFORM");
                auto location = Pointers::Transform_Position[transform].get();
                LOGD("CHECKING THE DISTANCE");
                auto distance = Vector3::Distance(myPos, location);
                if (closestDistance > distance) {
                    closestDistance = distance;
                    closestEnemy = player;
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
                    auto myPos = GetMyPosition();
                    auto enemyPos = GetPosition(closestEnemy);
                    auto rotation = Quaternion::LookRotation(enemyPos - myPos, Vector3(0, 1, 0));
                    Pointers::PlayerScript_rotation[myPlayer].set(&rotation);
                }
            }
        }
    }

    void DrawMenu() {
        ExecuteHacks();
        static bool closed = false;
        ImGui::Begin("Toshi's Bullet Force (Auto Update)", &closed);
        ImGui::Checkbox("Aimbot", &aimbot);
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

            ImGui_ImplAndroid_NewFrame(Unity::Screen::Width.get(), Unity::Screen::Height.get());

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