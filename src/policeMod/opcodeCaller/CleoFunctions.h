#pragma once

#include "../../pch.h"

#include "../../utils/utils.h"

#include "OpcodeCaller_fixed.h"

enum WAIT_FN_STATE
{
    WAIT_FN_NONE,
    WAIT_FN_COMPLETED,
    WAIT_FN_CANCELLED
};

struct WaitFunction
{
    int timePassed = 0;
    int time = 0;
    std::string name = "function";
    WAIT_FN_STATE state = WAIT_FN_STATE::WAIT_FN_NONE;
    std::function<void()> onComplete;
    std::function<void()> onCancel;

    bool isTestFunction = false;
    std::function<bool()> testFn;

    bool isConditionFunction = false;
    std::function<void(std::function<void()>, std::function<void()>)> conditionFn;
};

enum class PedType : int
{
    Player1 = 0,
    Player2 = 1,
    PlayerNetwork = 2,
    PlayerUnused = 3,
    CivMale = 4,
    CivFemale = 5,
    Cop = 6,
    Gang1 = 7,
    Gang2 = 8,
    Gang3 = 9,
    Gang4 = 10,
    Gang5 = 11,
    Gang6 = 12,
    Gang7 = 13,
    Gang8 = 14,
    Gang9 = 15,
    Gang10 = 16,
    Dealer = 17,
    Emergency = 18,
    Fireman = 19,
    Criminal = 20,
    Bum = 21,
    Prostitute = 22,
    Special = 23,
    Mission1 = 24,
    Mission2 = 25,
    Mission3 = 26,
    Mission4 = 27,
    Mission5 = 28,
    Mission6 = 29,
    Mission7 = 30,
    Mission8 = 31
};

class CleoFunctions
{
  public:
    static void Update(int dt);

    static WaitFunction *AddWaitFunction(int time, std::function<void()> callback);
    static void RemoveWaitFunction(WaitFunction *waitFunction);

    static WaitFunction *AddWaitForFunction(
        std::string name, std::function<bool()> testFn, std::function<void()> callback);
    static WaitFunction *AddCondition(std::function<void(std::function<void()>, std::function<void()>)> fn,
        std::function<void()> onComplete, std::function<void()> onCancel);
};

// --------------------------------------------

#define DEFOPCODE_CF(opcode, name) static constexpr uint16_t name##_OPCODE = 0x##opcode

// 0247: load_model 110
DEFOPCODE_CF(0247, LOAD_MODEL);
inline void LOAD_MODEL(int modelId) { Command<LOAD_MODEL_OPCODE>(modelId); }

// 01F5: $PLAYER_ACTOR = get_player_actor $PLAYER_CHAR
DEFOPCODE_CF(01F5, GET_PLAYER_ACTOR);
inline int GET_PLAYER_ACTOR(int player)
{
    int playerActor = 0;

    Command<GET_PLAYER_ACTOR_OPCODE>(0, &playerActor);

    return playerActor;
}

// 07AF: 6@ = player $PLAYER_CHAR group
DEFOPCODE_CF(07AF, GET_PLAYER_GROUP);
inline int GET_PLAYER_GROUP(int player)
{
    int group = 0;

    Command<GET_PLAYER_GROUP_OPCODE>(player, &group);

    return group;
}

// 0630: put_actor $PLAYER_ACTOR in_group 4@ as_leader
DEFOPCODE_CF(0630, PUT_ACTOR_IN_GROUP_AS_LEADER);
inline void PUT_ACTOR_IN_GROUP_AS_LEADER(int group, int _char)
{
    Command<PUT_ACTOR_IN_GROUP_AS_LEADER_OPCODE>(group, _char);
}

// 0631: put_actor 4@ in_group 6@
DEFOPCODE_CF(0631, PUT_ACTOR_IN_GROUP);
inline void PUT_ACTOR_IN_GROUP(int group, int _char) { Command<PUT_ACTOR_IN_GROUP_OPCODE>(group, _char); }

// 0457: player $PLAYER_CHAR aiming_at_actor 0@
DEFOPCODE_CF(0457, PLAYER_AIMING_AT_ACTOR);
inline bool PLAYER_AIMING_AT_ACTOR(int player, int _char)
{
    return Command<PLAYER_AIMING_AT_ACTOR_OPCODE>(player, _char);
}

// 02C1: store_to 127@ 128@ 129@ car_path_coords_closest_to 124@ 125@ 126@
DEFOPCODE_CF(02C1, GET_CLOSEST_CAR_NODE);
inline CVector GET_CLOSEST_CAR_NODE(float x, float y, float z)
{
    CVector result;

    Command<GET_CLOSEST_CAR_NODE_OPCODE>(x, y, z, &result.x, &result.y, &result.z);

    return result;
}

// 0683: attach_car 216@ to_car 196@ with_offset 0.0 -4.6 0.65 rotation 15.0 0.0 0.0
DEFOPCODE_CF(0683, ATTACH_CAR_TO_CAR);
inline void ATTACH_CAR_TO_CAR(
    int car, int toCar, float offsetX, float offsetY, float offsetZ, float rotX, float rotY, float rotZ)
{
    Command<ATTACH_CAR_TO_CAR_OPCODE>(car, toCar, offsetX, offsetY, offsetZ, rotX, rotY, rotZ);
}

// 00A5: 6@ = create_car #COPCARLA at 3@ 4@ 5@
DEFOPCODE_CF(00A5, CREATE_CAR_AT);
inline int CREATE_CAR_AT(int modelId, float x, float y, float z)
{
    int car = 0;

    Command<CREATE_CAR_AT_OPCODE>(modelId, x, y, z, &car);

    return car;
}

// 01C8: $P2 = create_actor_pedtype 23 model 280 in_car $POLICE_CAR passenger_seat 0
DEFOPCODE_CF(01C8, CREATE_ACTOR_PEDTYPE_IN_CAR_PASSENGER_SEAT);
inline int CREATE_ACTOR_PEDTYPE_IN_CAR_PASSENGER_SEAT(int vehicle, PedType pedType, int modelId, int seatId)
{
    int ped = 0;

    Command<CREATE_ACTOR_PEDTYPE_IN_CAR_PASSENGER_SEAT_OPCODE>(vehicle, (int)pedType, modelId, seatId, &ped);

    return ped;
}

// 02D4: car 3@ turn_off_engine
DEFOPCODE_CF(02D4, CAR_TURN_OFF_ENGINE);
inline void CAR_TURN_OFF_ENGINE(int car) { Command<CAR_TURN_OFF_ENGINE_OPCODE>(car); }

// 02C0: store_to 3@ 4@ 5@ ped_path_coords_closest_to 0@ 1@ 2@
DEFOPCODE_CF(02C0, STORE_PED_PATH_COORDS_CLOSEST_TO);
inline CVector STORE_PED_PATH_COORDS_CLOSEST_TO(float x, float y, float z)
{
    CVector result = CVector(0, 0, 0);

    Command<STORE_PED_PATH_COORDS_CLOSEST_TO_OPCODE>(x, y, z, &result.x, &result.y, &result.z);

    return result;
}

// 0129: 10@ = create_actor_pedtype 23 model #LAPD1 in_car 6@ driverseat
DEFOPCODE_CF(0129, CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT);
inline int CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(int car, PedType pedType, int modelId)
{
    int _char = 0;

    Command<CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT_OPCODE>(car, (int)pedType, modelId, &_char);

    return _char;
}

// 009B: destroy_actor 4@
DEFOPCODE_CF(009B, DESTROY_ACTOR);
inline void DESTROY_ACTOR(int actor) { Command<DESTROY_ACTOR_OPCODE>(actor); }

// 00A6: destroy_car 7@
DEFOPCODE_CF(00A6, DESTROY_CAR);
inline void DESTROY_CAR(int car) { Command<DESTROY_CAR_OPCODE>(car); }

// 0186: $60 = create_marker_above_car $59
DEFOPCODE_CF(0186, ADD_BLIP_FOR_CAR);
inline int ADD_BLIP_FOR_CAR(int car)
{
    int blip = 0;

    Command<ADD_BLIP_FOR_CAR_OPCODE>(car, &blip);

    return blip;
}

// 0165: set_marker 9@ color_to 2
DEFOPCODE_CF(0165, SET_MARKER_COLOR_TO);
inline void SET_MARKER_COLOR_TO(int blip, int color) { Command<SET_MARKER_COLOR_TO_OPCODE>(blip, color); }

// 044B: actor %1d male
DEFOPCODE_CF(044B, ACTOR_MALE);
inline bool ACTOR_MALE(int actor) { return Command<ACTOR_MALE_OPCODE>(actor); }

// 044C: actor %1d driving
DEFOPCODE_CF(044C, ACTOR_DRIVING);
inline bool ACTOR_DRIVING(int actor) { return Command<ACTOR_DRIVING_OPCODE>(actor); }

// 01B2: give_actor 3@ weapon 22 ammo 10000
DEFOPCODE_CF(01B2, GIVE_ACTOR_WEAPON);
inline void GIVE_ACTOR_WEAPON(int _char, int weaponType, int ammo)
{
    Command<GIVE_ACTOR_WEAPON_OPCODE>(_char, weaponType, ammo);
}

// 02E3: car %1d speed
DEFOPCODE_CF(02E3, CAR_SPEED);
inline float CAR_SPEED(int vehicle)
{
    float speed = 0.0f;

    Command<CAR_SPEED_OPCODE>(vehicle, &speed);

    return speed;
}

// 0227: 5@ = car 23@ health
DEFOPCODE_CF(0227, GET_CAR_HEALTH);
inline int GET_CAR_HEALTH(int vehicle)
{
    int result = 0;

    Command<GET_CAR_HEALTH_OPCODE>(vehicle, &result);

    return result;
}

// 0224: set_car $2868 health_to 1000
DEFOPCODE_CF(0224, SET_CAR_HEALTH);
inline void SET_CAR_HEALTH(int vehicle, int health) { Command<SET_CAR_HEALTH_OPCODE>(vehicle, health); }

// 01BD: put_car_at %1d x %2f y %3f z %4f
DEFOPCODE_CF(01BD, PUT_CAR_AT);
inline void PUT_CAR_AT(int vehicle, float x, float y, float z) { Command<PUT_CAR_AT_OPCODE>(vehicle, x, y, z); }

// 0175: set_car_z_angle %1d to %2f
DEFOPCODE_CF(0175, SET_CAR_Z_ANGLE);
inline void SET_CAR_Z_ANGLE(int vehicle, float heading) { Command<SET_CAR_Z_ANGLE_OPCODE>(vehicle, heading); }

// 0173
DEFOPCODE_CF(0173, SET_CHAR_HEADING);
inline void SET_CHAR_HEADING(int handle, float heading) { Command<SET_CHAR_HEADING_OPCODE>(handle, heading); }

// 0172
DEFOPCODE_CF(0172, GET_CHAR_HEADING);
inline float GET_CHAR_HEADING(int handle)
{
    float result;

    Command<GET_CHAR_HEADING_OPCODE>(handle, &result);

    return result;
}

// 017A: set_car_immunities %1d BP %2d FP %3d EP %4d CP %5d MP %6d
DEFOPCODE_CF(017A, SET_CAR_IMMUNITIES);
inline void SET_CAR_IMMUNITIES(int vehicle, bool BP, bool FP, bool EP, bool CP, bool MP)
{
    Command<SET_CAR_IMMUNITIES_OPCODE>(vehicle, BP ? 1 : 0, FP ? 1 : 0, EP ? 1 : 0, CP ? 1 : 0, MP ? 1 : 0);
}

// 03AC: load_requested_models
DEFOPCODE_CF(03AC, LOAD_REQUESTED_MODELS);
inline void LOAD_REQUESTED_MODELS() { Command<LOAD_REQUESTED_MODELS_OPCODE>(); }

// 038B: load_requested_anims
DEFOPCODE_CF(038B, LOAD_REQUESTED_ANIMS);
inline void LOAD_REQUESTED_ANIMS() { Command<LOAD_REQUESTED_ANIMS_OPCODE>(); }

// 0247: request_model %1d
DEFOPCODE_CF(0247, REQUEST_MODEL);
inline void REQUEST_MODEL(int modelId) { Command<REQUEST_MODEL_OPCODE>(modelId); }

// 0249: release_model %1d
DEFOPCODE_CF(0249, RELEASE_MODEL);
inline void RELEASE_MODEL(int modelId) { Command<RELEASE_MODEL_OPCODE>(modelId); }

// 09C7: change_player 0 model_to 280
DEFOPCODE_CF(09C7, CHANGE_PLAYER_MODEL_TO);
inline void CHANGE_PLAYER_MODEL_TO(int player, int modelId) { Command<CHANGE_PLAYER_MODEL_TO_OPCODE>(player, modelId); }

// 04EE: animation "GANGS" loaded
DEFOPCODE_CF(04EE, HAS_ANIMATION_LOADED);
inline bool HAS_ANIMATION_LOADED(const char *animationFile)
{
    return Command<HAS_ANIMATION_LOADED_OPCODE>(animationFile);
}

// 03A3
DEFOPCODE_CF(03A3, IS_CHAR_MALE);
inline bool IS_CHAR_MALE(int handle) { return Command<IS_CHAR_MALE_OPCODE>(handle); }

// 04D7
DEFOPCODE_CF(04D7, FREEZE_CHAR_POSITION);
inline void FREEZE_CHAR_POSITION(int handle, bool state) { Command<FREEZE_CHAR_POSITION_OPCODE>(handle, state); }

// 00A1
DEFOPCODE_CF(00A1, SET_CHAR_COORDINATES);
inline void SET_CHAR_COORDINATES(int handle, float x, float y, float z)
{
    Command<SET_CHAR_COORDINATES_OPCODE>(handle, x, y, z);
}

// 04ED: load_animation "GANGS"
DEFOPCODE_CF(04ED, LOAD_ANIMATION);
inline void LOAD_ANIMATION(const char *animationFile) { Command<LOAD_ANIMATION_OPCODE>(animationFile); }

// 0256: player $PLAYER_CHAR defined
DEFOPCODE_CF(0256, PLAYER_DEFINED);
inline bool PLAYER_DEFINED(int player) { return Command<PLAYER_DEFINED_OPCODE>(0); }

// 056E: car 3@ defined
DEFOPCODE_CF(056E, CAR_DEFINED);
inline bool CAR_DEFINED(int car) { return Command<CAR_DEFINED_OPCODE>(car); }

// 056D: actor 0@ defined
DEFOPCODE_CF(056D, ACTOR_DEFINED);
inline bool ACTOR_DEFINED(int actor) { return Command<ACTOR_DEFINED_OPCODE>(actor); }

// 0248: model %1d available
DEFOPCODE_CF(0248, HAS_MODEL_LOADED);
inline bool HAS_MODEL_LOADED(int modelId) { return Command<HAS_MODEL_LOADED_OPCODE>(modelId); }

// 02F6: actor %1d in_zone %2s
DEFOPCODE_CF(02F6, ACTOR_IN_ZONE);
inline bool ACTOR_IN_ZONE(int actor, const char *zoneName) { return Command<ACTOR_IN_ZONE_OPCODE>(actor, zoneName); }

// 0107: player %1d money += %2d
DEFOPCODE_CF(0107, ADD_PLAYER_MONEY);
inline void ADD_PLAYER_MONEY(int player, int amount) { Command<ADD_PLAYER_MONEY_OPCODE>(player, amount); }

// 0109: player %1d money = %2d
DEFOPCODE_CF(0109, SET_PLAYER_MONEY);
inline void SET_PLAYER_MONEY(int player, int amount) { Command<SET_PLAYER_MONEY_OPCODE>(player, amount); }

// 0108: player %1d money
DEFOPCODE_CF(0108, GET_PLAYER_MONEY);
inline int GET_PLAYER_MONEY(int player)
{
    int money = 0;

    Command<GET_PLAYER_MONEY_OPCODE>(player, &money);

    return money;
}

// 01C2: remove_references_to_actor %1d
DEFOPCODE_CF(01C2, REMOVE_REFERENCES_TO_ACTOR);
inline void REMOVE_REFERENCES_TO_ACTOR(int actor) { Command<REMOVE_REFERENCES_TO_ACTOR_OPCODE>(actor); }

// 01C3: remove_references_to_car %1d
DEFOPCODE_CF(01C3, REMOVE_REFERENCES_TO_CAR);
inline void REMOVE_REFERENCES_TO_CAR(int vehicle) { Command<REMOVE_REFERENCES_TO_CAR_OPCODE>(vehicle); }

// 01EA: 68@ = car 67@ max_passengers
DEFOPCODE_CF(01EA, CAR_MAX_PASSENGERS);
inline int CAR_MAX_PASSENGERS(int car)
{
    int maxPassengers = 0;

    Command<CAR_MAX_PASSENGERS_OPCODE>(car, &maxPassengers);

    return maxPassengers;
}

// 0432: 19@ = get_actor_handle_from_car $47 passenger_seat 0
DEFOPCODE_CF(0432, GET_ACTOR_HANDLE_FROM_CAR_PASSENGER_SEAT);
inline int GET_ACTOR_HANDLE_FROM_CAR_PASSENGER_SEAT(int car, int seatId)
{
    int _char = 0;

    Command<GET_ACTOR_HANDLE_FROM_CAR_PASSENGER_SEAT_OPCODE>(car, seatId, &_char);

    return _char;
}

// 0665: get_actor 0@ model_to 7@
DEFOPCODE_CF(0665, GET_ACTOR_MODEL);
inline int GET_ACTOR_MODEL(int _char)
{
    int modelId = 0;

    Command<GET_ACTOR_MODEL_OPCODE>(_char, &modelId);

    return modelId;
}

// 0441: 7@ = car $47 model
DEFOPCODE_CF(0441, GET_CAR_MODEL);
inline int GET_CAR_MODEL(int car)
{
    int modelId = 0;

    Command<GET_CAR_MODEL_OPCODE>(car, &modelId);

    return modelId;
}

// 09C9: disembark_actor $132 from_car 44@ and_freeze_actor_position
DEFOPCODE_CF(09C9, REMOVE_CHAR_FROM_CAR_MAINTAIN_POSITION);
inline void REMOVE_CHAR_FROM_CAR_MAINTAIN_POSITION(int actor, int car)
{
    Command<REMOVE_CHAR_FROM_CAR_MAINTAIN_POSITION_OPCODE>(actor, car);
}

// 0431: car $47 passenger_seat_free 0
DEFOPCODE_CF(0431, CAR_PASSENGER_SEAT_FREE);
inline bool CAR_PASSENGER_SEAT_FREE(int car, int seatId)
{
    return Command<CAR_PASSENGER_SEAT_FREE_OPCODE>(car, seatId);
}

// 00A7: car 7@ drive_to 0@ 1@ 2@
DEFOPCODE_CF(00A7, CAR_DRIVE_TO);
inline void CAR_DRIVE_TO(int car, float x, float y, float z) { Command<CAR_DRIVE_TO_OPCODE>(car, x, y, z); }

// 01C4: remove_references_to_object %1d
DEFOPCODE_CF(01C4, REMOVE_REFERENCES_TO_OBJECT);
inline void REMOVE_REFERENCES_TO_OBJECT(int obj) { Command<REMOVE_REFERENCES_TO_OBJECT_OPCODE>(obj); }

// 01C7: remove_references_to_heli %1d
DEFOPCODE_CF(01C7, REMOVE_REFERENCES_TO_HELI);
inline void REMOVE_REFERENCES_TO_HELI(int heli) { Command<REMOVE_REFERENCES_TO_HELI_OPCODE>(heli); }

// 01C8: remove_references_to_pickup %1d
DEFOPCODE_CF(01C8, REMOVE_REFERENCES_TO_PICKUP);
inline void REMOVE_REFERENCES_TO_PICKUP(int pickup) { Command<REMOVE_REFERENCES_TO_PICKUP_OPCODE>(pickup); }

// 04C4: store_coords_to 4@ 5@ 6@ from_actor $PLAYER_ACTOR with_offset 1.0 2.0 0.0
DEFOPCODE_CF(04C4, STORE_COORDS_FROM_ACTOR_WITH_OFFSET);
inline void STORE_COORDS_FROM_ACTOR_WITH_OFFSET(
    int _char, float offsetX, float offsetY, float offsetZ, float *x, float *y, float *z)
{
    Command<STORE_COORDS_FROM_ACTOR_WITH_OFFSET_OPCODE>(_char, offsetX, offsetY, offsetZ, x, y, z);
}

// 00DF: actor $PLAYER_ACTOR driving
DEFOPCODE_CF(00DF, IS_CHAR_IN_ANY_CAR);
inline bool IS_CHAR_IN_ANY_CAR(int _char) { return Command<IS_CHAR_IN_ANY_CAR_OPCODE>(_char); }

// 0811: $47 = actor $PLAYER_ACTOR used_car
DEFOPCODE_CF(0811, ACTOR_USED_CAR);
inline int ACTOR_USED_CAR(int _char)
{
    int car = 0;

    Command<ACTOR_USED_CAR_OPCODE>(_char, &car);

    return car;
}

// 0407: store_coords_to 128@ 138@ 148@ from_car 551@ with_offset -0.337 1.566 0.657
DEFOPCODE_CF(0407, STORE_COORDS_FROM_CAR_WITH_OFFSET);
inline void STORE_COORDS_FROM_CAR_WITH_OFFSET(
    int car, float offsetX, float offsetY, float offsetZ, float *x, float *y, float *z)
{
    Command<STORE_COORDS_FROM_CAR_WITH_OFFSET_OPCODE>(car, offsetX, offsetY, offsetZ, x, y, z);
}

// 0812: AS_actor 0@ perform_animation "handsup" IFP "PED" framedelta 4.0 loopA 0 lockX 0 lockY 0 lockF 1 time -1
DEFOPCODE_CF(0812, PERFORM_ANIMATION_AS_ACTOR);
inline void PERFORM_ANIMATION_AS_ACTOR(int _char, const char *animationName, const char *animationFile,
    float frameDelta, bool loop, bool lockX, bool lockY, bool lockF, int time)
{
    Command<PERFORM_ANIMATION_AS_ACTOR_OPCODE>(
        _char, animationName, animationFile, frameDelta, loop, lockX, lockY, lockF, time);
}

// 0464: put_actor $PED into_turret_on_car $CAR at_car_offset 0.0 0.0 0.0 position 0 shooting_angle_limit 0.0
// with_weapon 0
DEFOPCODE_CF(0464, PUT_ACTOR_INTO_TURRET_ON_CAR);
inline void PUT_ACTOR_INTO_TURRET_ON_CAR(
    int _char, int vehicle, float offsetX, float offsetY, float offsetZ, int position, float angleLimit, int weaponType)
{
    Command<PUT_ACTOR_INTO_TURRET_ON_CAR_OPCODE>(
        _char, vehicle, offsetX, offsetY, offsetZ, position, angleLimit, weaponType);
}

// 0465: remove_actor $PED from_turret_mode
DEFOPCODE_CF(0465, REMOVE_ACTOR_FROM_TURRET_MODE);
inline void REMOVE_ACTOR_FROM_TURRET_MODE(int _char) { Command<REMOVE_ACTOR_FROM_TURRET_MODE_OPCODE>(_char); }

// 0850: AS_actor 105@ follow_actor $PLAYER_ACTOR
DEFOPCODE_CF(0850, TASK_FOLLOW_FOOTSTEPS);
inline void TASK_FOLLOW_FOOTSTEPS(int handle, int target) { Command<TASK_FOLLOW_FOOTSTEPS_OPCODE>(handle, target); }

// 009A: 6@ = create_actor_pedtype 20 model #DNFYLC at 3@ 4@ 5@
DEFOPCODE_CF(009A, CREATE_ACTOR_PEDTYPE);
inline int CREATE_ACTOR_PEDTYPE(PedType pedType, int modelId, float x, float y, float z)
{
    int _char = 0;

    Command<CREATE_ACTOR_PEDTYPE_OPCODE>((int)pedType, modelId, x, y, z, &_char);

    return _char;
}

// 0726: heli 3@ follow_actor -1 follow_car 5@ radius 15.0
DEFOPCODE_CF(0726, HELI_FOLLOW);
inline void HELI_FOLLOW(int heli, int _char, int vehicle, float radius)
{
    Command<HELI_FOLLOW_OPCODE>(heli, _char, vehicle, radius);
}

// 0743: heli 45@ fly_to -2244.48 129.14 34.56 altitude 0.0 0.0
DEFOPCODE_CF(0743, HELI_FLY_TO);
inline void HELI_FLY_TO(int heli, float x, float y, float z, float minAltitude, float maxAltitude)
{
    Command<HELI_FLY_TO_OPCODE>(heli, x, y, z, minAltitude, maxAltitude);
}

// 0687: clear_actor $PLAYER_ACTOR task
DEFOPCODE_CF(0687, CLEAR_ACTOR_TASK);
inline void CLEAR_ACTOR_TASK(int _char) { Command<CLEAR_ACTOR_TASK_OPCODE>(_char); }

// 0611: actor 2@ performing_animation "LRGIRL_IDLE_TO_L0"
DEFOPCODE_CF(0611, ACTOR_PERFORMING_ANIMATION);
inline bool ACTOR_PERFORMING_ANIMATION(int _char, const char *animationName)
{
    return Command<ACTOR_PERFORMING_ANIMATION_OPCODE>(_char, animationName);
}

// 0603: AS_actor @3 goto_point_any_means 2493.82 -1669.91 12.8 mode 7 use_car -1
DEFOPCODE_CF(0603, TASK_GO_TO_COORD_ANY_MEANS);
inline void TASK_GO_TO_COORD_ANY_MEANS(int actor, float x, float y, float z, int mode, bool useCar)
{
    Command<TASK_GO_TO_COORD_ANY_MEANS_OPCODE>(actor, x, y, z, mode, useCar ? 1 : 0);
}

// 0918: set_car 3@ engine_operation 1
DEFOPCODE_CF(0918, SET_CAR_ENGINE_OPERATION);
inline void SET_CAR_ENGINE_OPERATION(int car, bool state) { Command<SET_CAR_ENGINE_OPERATION_OPCODE>(car, state); }

// 0825: set_helicopter 3@ instant_rotor_start
DEFOPCODE_CF(0825, SET_HELICOPTER_INSTANT_ROTOR_START);
inline void SET_HELICOPTER_INSTANT_ROTOR_START(int heli) { Command<SET_HELICOPTER_INSTANT_ROTOR_START_OPCODE>(heli); }

enum DrivingMode
{
    StopForCars = 0,
    SlowDownForCars = 1,
    AvoidCars = 2,
    PloughThrough = 3,
    StopForCarsIgnoreLights = 4,
    AvoidCarsObeyLights = 5,
    AvoidCarsStopForPedsObeyLights = 6
};

// 00AE: set_car 3@ traffic_behaviour_to 2
DEFOPCODE_CF(00AE, SET_CAR_TRAFFIC_BEHAVIOUR);
inline void SET_CAR_TRAFFIC_BEHAVIOUR(int car, DrivingMode drivingStyle)
{
    Command<SET_CAR_TRAFFIC_BEHAVIOUR_OPCODE>(car, (int)drivingStyle);
}

// 0397: enable_car 6@ siren 1
DEFOPCODE_CF(0397, ENABLE_CAR_SIREN);
inline void ENABLE_CAR_SIREN(int car, bool state) { Command<ENABLE_CAR_SIREN_OPCODE>(car, state); }

// 0635: AS_actor -1 aim_at_actor $PLAYER_ACTOR 2000 ms
DEFOPCODE_CF(0635, AIM_AT_ACTOR);
inline void AIM_AT_ACTOR(int _char, int target, int time) { Command<AIM_AT_ACTOR_OPCODE>(_char, target, time); }

// 05DD: AS_actor 7@ flee_from_actor 6@ from_origin_radius 1000.0 timelimit -1
DEFOPCODE_CF(05DD, FLEE_FROM_ACTOR);
inline void FLEE_FROM_ACTOR(int _char, int threat, float radius, int time)
{
    Command<FLEE_FROM_ACTOR_OPCODE>(_char, threat, radius, time);
}

// 00A8: set_car 52@ to_psycho_driver
DEFOPCODE_CF(00A8, SET_CAR_TO_PSYCHO_DRIVER);
inline void SET_CAR_TO_PSYCHO_DRIVER(int car) { Command<SET_CAR_TO_PSYCHO_DRIVER_OPCODE>(car); }

// 00AD: set_car 3@ max_speed_to 50.0
DEFOPCODE_CF(00AD, SET_CAR_MAX_SPEED);
inline void SET_CAR_MAX_SPEED(int car, float maxSpeed) { Command<SET_CAR_MAX_SPEED_OPCODE>(car, maxSpeed); }

// 07F8: car 6@ follow_car 8@ radius 8.0
DEFOPCODE_CF(07F8, CAR_FOLLOW_CAR);
inline void CAR_FOLLOW_CAR(int car, int followCar, float radius)
{
    Command<CAR_FOLLOW_CAR_OPCODE>(car, followCar, radius);
}

// 046C: 133@ = car 50@ driver
DEFOPCODE_CF(046C, GET_DRIVER_OF_CAR);
inline int GET_DRIVER_OF_CAR(int car)
{
    int _char = 0;

    Command<GET_DRIVER_OF_CAR_OPCODE>(car, &_char);

    return _char;
}

// 03BC: 7@ = create_sphere_at 1536.1325 -1671.2093 13.3828 radius 3.0
DEFOPCODE_CF(03BC, CREATE_SPHERE);
inline int CREATE_SPHERE(float x, float y, float z, float radius)
{
    int sphere = 0;

    Command<CREATE_SPHERE_OPCODE>(x, y, z, radius, &sphere);

    return sphere;
}

// 03BD: destroy_sphere 7@
DEFOPCODE_CF(03BD, DESTROY_SPHERE);
inline void DESTROY_SPHERE(int sphere) { Command<DESTROY_SPHERE_OPCODE>(sphere); }

// 0633: AS_actor 4@ exit_car
DEFOPCODE_CF(0633, EXIT_CAR_AS_ACTOR);
inline void EXIT_CAR_AS_ACTOR(int _actor) { Command<EXIT_CAR_AS_ACTOR_OPCODE>(_actor); }

// 05CB: AS_actor 21@ enter_car 0@ as_driver 20000 ms
DEFOPCODE_CF(05CB, ENTER_CAR_AS_DRIVER_AS_ACTOR);
inline void ENTER_CAR_AS_DRIVER_AS_ACTOR(int _char, int vehicle, int time)
{
    Command<ENTER_CAR_AS_DRIVER_AS_ACTOR_OPCODE>(_char, vehicle, time);
}

// 05CA: AS_actor 3@ enter_car 7@ passenger_seat 1 time 10000
DEFOPCODE_CF(05CA, ACTOR_ENTER_CAR_PASSENGER_SEAT);
inline void ACTOR_ENTER_CAR_PASSENGER_SEAT(int _char, int vehicle, int time, int seatId)
{
    Command<ACTOR_ENTER_CAR_PASSENGER_SEAT_OPCODE>(_char, vehicle, time, seatId);
}

// 04B8: get_weapon_data_from_actor $PLAYER_ACTOR slot 2 weapon 404@ ammo 405@ model 405@
DEFOPCODE_CF(04B8, GET_WEAPON_DATA_FROM_ACTOR);
inline void GET_WEAPON_DATA_FROM_ACTOR(int _char, int weaponSlotId, int *weaponType, int *weaponAmmo, int *weaponModel)
{
    Command<GET_WEAPON_DATA_FROM_ACTOR_OPCODE>(_char, weaponSlotId, weaponType, weaponAmmo, weaponModel);
}

// 0187: 47@ = create_marker_above_actor 75@
DEFOPCODE_CF(0187, ADD_BLIP_FOR_CHAR);
inline int ADD_BLIP_FOR_CHAR(int _char)
{
    int blip = 0;

    Command<ADD_BLIP_FOR_CHAR_OPCODE>(_char, &blip);

    return blip;
}

// 01F0: set_max_wanted_level_to 0
DEFOPCODE_CF(01F0, SET_MAX_WANTED_LEVEL_TO);
inline void SET_MAX_WANTED_LEVEL_TO(int wantedLevel) { Command<SET_MAX_WANTED_LEVEL_TO_OPCODE>(wantedLevel); }

// 010D: set_player $PLAYER_CHAR wanted_level_to 0
DEFOPCODE_CF(010D, SET_PLAYER_WANTED_LEVEL);
inline void SET_PLAYER_WANTED_LEVEL(int player, int wantedLevel)
{
    Command<SET_PLAYER_WANTED_LEVEL_OPCODE>(player, wantedLevel);
}

// 0526
DEFOPCODE_CF(0526, SET_CHAR_STAY_IN_CAR_WHEN_JACKED);
inline void SET_CHAR_STAY_IN_CAR_WHEN_JACKED(int ped, bool state)
{
    Command<SET_CHAR_STAY_IN_CAR_WHEN_JACKED_OPCODE>(ped, state);
}

// 0107: $OBJ1 = create_object 1459 at 4@ 5@ 6@
DEFOPCODE_CF(0107, CREATE_OBJECT);
inline int CREATE_OBJECT(int modelId, float x, float y, float z)
{
    int object = 0;

    Command<CREATE_OBJECT_OPCODE>(modelId, x, y, z, &object);

    return object;
}

// 0177: set_object $OBJ1 Z_angle_to $ANGLE
DEFOPCODE_CF(0177, SET_OBJECT_Z_ANGLE);
inline void SET_OBJECT_Z_ANGLE(int object, float heading) { Command<SET_OBJECT_Z_ANGLE_OPCODE>(object, heading); }

// 0164: disable_marker $482
DEFOPCODE_CF(0164, DISABLE_MARKER);
inline void DISABLE_MARKER(int blip) { Command<DISABLE_MARKER_OPCODE>(blip); }

// 05E2: AS_actor 6@ kill_actor 7@
DEFOPCODE_CF(05E2, KILL_ACTOR);
inline void KILL_ACTOR(int killer, int target) { Command<KILL_ACTOR_OPCODE>(killer, target); }

// 0226: $7826 = actor 173@ health
DEFOPCODE_CF(0226, ACTOR_HEALTH);
inline int ACTOR_HEALTH(int _char)
{
    int health = 0;

    Command<ACTOR_HEALTH_OPCODE>(_char, &health);

    return health;
}

// 0118: actor 0@ dead
DEFOPCODE_CF(0118, ACTOR_DEAD);
inline bool ACTOR_DEAD(int actor) { return Command<ACTOR_DEAD_OPCODE>(actor); }

// 0223: set_actor 2@ health_to 500
DEFOPCODE_CF(0223, SET_ACTOR_HEALTH);
inline void SET_ACTOR_HEALTH(int _char, int health) { Command<SET_ACTOR_HEALTH_OPCODE>(_char, health); }

// 05D3
DEFOPCODE_CF(05D3, TASK_GO_STRAIGHT_TO_COORD);
inline void TASK_GO_STRAIGHT_TO_COORD(int handle, float x, float y, float z, int speed, int time)
{
    Command<TASK_GO_STRAIGHT_TO_COORD_OPCODE>(handle, x, y, z, speed, time);
}

// 020C
DEFOPCODE_CF(020C, ADD_EXPLOSION);
inline void ADD_EXPLOSION(float x, float y, float z, int explosionType)
{
    Command<ADD_EXPLOSION_OPCODE>(x, y, z, explosionType);
}

// 09CA
DEFOPCODE_CF(09CA, SET_OBJECT_PROOFS);
inline void SET_OBJECT_PROOFS(
    int objectId, bool bulletProof, bool fireProof, bool explosionProof, bool collisionProof, bool meleeProof)
{
    Command<SET_OBJECT_PROOFS_OPCODE>(objectId, bulletProof, fireProof, explosionProof, collisionProof, meleeProof);
}

// 0167: 7@ = create_marker_at 0@ 1@ 2@ color 0 flag 3
DEFOPCODE_CF(0167, CREATE_MARKER_AT);
inline int CREATE_MARKER_AT(float x, float y, float z, int color, int display)
{
    int blip = 0;

    Command<CREATE_MARKER_AT_OPCODE>(x, y, z, color, display, &blip);

    return blip;
}

// 0108: destroy_object $1173
DEFOPCODE_CF(0108, DESTROY_OBJECT);
inline void DESTROY_OBJECT(int object) { Command<DESTROY_OBJECT_OPCODE>(object); }

// 070A: AS_actor $PLAYER_ACTOR attach_to_object 38@ offset 0.0 0.0 0.0 on_bone 6 16 perform_animation "PHONE_TALK"
// IFP_file "PED" time 1
DEFOPCODE_CF(070A, ATTACH_TO_OBJECT_AND_PERFORM_ANIMATION);
inline void ATTACH_TO_OBJECT_AND_PERFORM_ANIMATION(int _char, int object, float offsetX, float offsetY, float offsetZ,
    int boneId, int _p7, const char *animationName, const char *animationFile, int time)
{
    Command<ATTACH_TO_OBJECT_AND_PERFORM_ANIMATION_OPCODE>(
        _char, object, offsetX, offsetY, offsetZ, boneId, _p7, animationName, animationFile, time);
}

// 0168: set_marker 9@ size 3
DEFOPCODE_CF(0168, SET_MARKER_SIZE);
inline void SET_MARKER_SIZE(int blip, int size) { Command<SET_MARKER_SIZE_OPCODE>(blip, size); }

// --------------------------------------------

inline void WAIT(int time, std::function<void()> callback) { CleoFunctions::AddWaitFunction(time, callback); }

// --------------------------------------------

inline int GetPlayerActor() { return GET_PLAYER_ACTOR(0); }

inline CVector GetPedPositionWithOffset(int hPed, CVector offset)
{
    float x = 0, y = 0, z = 0;

    STORE_COORDS_FROM_ACTOR_WITH_OFFSET(hPed, offset.x, offset.y, offset.z, &x, &y, &z);

    return CVector(x, y, z);
}

inline CVector GetPedPosition(int hPed) { return GetPedPositionWithOffset(hPed, CVector(0, 0, 0)); }

inline int GetVehiclePedIsUsing(int hPed)
{
    if (!IS_CHAR_IN_ANY_CAR(hPed))
        return 0;

    return ACTOR_USED_CAR(hPed);
}

inline void ClearPedAnimations(int hPed)
{
    if (IS_CHAR_IN_ANY_CAR(hPed))
        return;

    CLEAR_ACTOR_TASK(hPed);

    PERFORM_ANIMATION_AS_ACTOR(hPed, "hndshkfa_swt", "gangs", 500.0f, 0, 0, 0, 0, 1);
}

inline CVector GetPlayerPosition() { return GetPedPosition(GetPlayerActor()); }

inline CVector GetPlayerPositionInForward(float distanceY)
{
    return GetPedPositionWithOffset(GetPlayerActor(), CVector(0, distanceY, 0));
}

inline CVector GetCarPositionWithOffset(int hVehicle, CVector offset)
{
    float x = 0, y = 0, z = 0;

    STORE_COORDS_FROM_CAR_WITH_OFFSET(hVehicle, offset.x, offset.y, offset.z, &x, &y, &z);

    return CVector(x, y, z);
}

inline CVector GetCarPosition(int hVehicle) { return GetCarPositionWithOffset(hVehicle, CVector(0, 0, 0)); }

inline double DistanceFromPed(int hPed, CVector position)
{
    auto pedPosition = GetPedPosition(hPed);
    auto distance = distanceBetweenPoints(pedPosition, position);

    return distance;
}

inline double DistanceFromVehicle(int hCar, CVector position)
{
    auto carPosition = GetCarPosition(hCar);
    auto distance = distanceBetweenPoints(carPosition, position);

    return distance;
}

inline int CreateMarker(float x, float y, float z, int color, int display, int size)
{
    int blip = CREATE_MARKER_AT(x, y, z, color, display);

    SET_MARKER_SIZE(blip, size);

    return blip;
}

inline int SpawnPedRandomlyAtPosition_PedNode(CVector position, PedType pedType, int modelId, float radius)
{
    CVector offset = CVector(radius / 2 + (float)(getRandomNumber(0, (int)radius)),
        radius / 2 + (float)(getRandomNumber(0, (int)radius)), 0);

    position += offset;

    auto nodePosition = STORE_PED_PATH_COORDS_CLOSEST_TO(position.x, position.y, position.z);

    int pedRef = CREATE_ACTOR_PEDTYPE(pedType, modelId, nodePosition.x, nodePosition.y, nodePosition.z);

    return pedRef;
}

inline bool PedHasWeaponId(int pedRef, int weaponId)
{
    for (int i = 1; i <= 13; i++)
    {
        int weaponType = 0;
        int weaponAmmo = 0;
        int weaponModel = 0;

        GET_WEAPON_DATA_FROM_ACTOR(pedRef, i, &weaponType, &weaponAmmo, &weaponModel);

        if (weaponId == weaponType)
            return true;
    }

    return false;
}