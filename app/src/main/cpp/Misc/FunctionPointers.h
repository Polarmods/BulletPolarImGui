//
// Created by letha on 9/4/2021.
// Updated by Tomie on 3/27/2023
//

#ifndef IMGUIANDROID_FUNCTIONPOINTERS_H
#define IMGUIANDROID_FUNCTIONPOINTERS_H
using namespace BNM::UNITY_STRUCTS;
namespace Pointers {
    BNM::LoadClass Component;
    BNM::Property<void *> Component_Transform;

    BNM::LoadClass GameObject;
    BNM::Property<void *> GameObject_Transform;

    BNM::LoadClass Transform;
    BNM::Property<Vector3> Transform_Position;
    BNM::Method<void> Transform_LookAt;

    BNM::LoadClass Camera;
    BNM::Method<Vector3 *> Camera_WorldToScreenPoint;
    BNM::Property<void *> Camera_Main;

    BNM::LoadClass PhotonNetwork;
    BNM::Property<BNM::MONO_STRUCTS::monoArray<void *>*> PhotonNetwork_otherPlayers;
    BNM::Property<void *> PhotonNetwork_player;

    BNM::LoadClass PhotonPlayer;
    BNM::Field<void *> PhotonPlayer_playerScript;

    BNM::LoadClass PlayerScript;
    BNM::Field<Quaternion *> PlayerScript_rotation;

    BNM::LoadClass PlayerUtils;
    BNM::Method<bool> PlayerUtils_IsPlayerOnTeam;
    void LoadPointers() {
            Component = BNM::LoadClass("UnityEngine", "Component");
            Component_Transform = Component.GetPropertyByName("transform");

            GameObject = BNM::LoadClass("UnityEngine", "GameObject");
            GameObject_Transform = GameObject.GetPropertyByName("transform");

            Transform = BNM::LoadClass("UnityEngine", "Transform");
            Transform_Position = Transform.GetPropertyByName("position");

            Camera = BNM::LoadClass("UnityEngine", "Camera");
            Camera_WorldToScreenPoint = Camera.GetMethodByName("WorldToScreenPoint", 1);
            Camera_Main = Camera.GetPropertyByName("main");

            PhotonNetwork = BNM::LoadClass("", "PhotonNetwork");
            PhotonNetwork_otherPlayers = PhotonNetwork.GetPropertyByName("otherPlayers");
            PhotonNetwork_player = PhotonNetwork.GetPropertyByName("player");

            PhotonPlayer = BNM::LoadClass("", "PhotonPlayer");
            PhotonPlayer_playerScript = PhotonPlayer.GetFieldByName("playerScript");

            PlayerUtils = BNM::LoadClass("", "PlayerUtils");
            PlayerUtils_IsPlayerOnTeam = PlayerUtils.GetMethodByName("IsPlayerOnTeam");

            PlayerScript = BNM::LoadClass("", "PlayerScript");
            PlayerScript_rotation = PlayerScript.GetFieldByName("rotation");
    }
}
#endif IMGUIANDROID_FUNCTIONPOINTERS_H