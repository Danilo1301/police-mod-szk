#pragma once

enum PluginOperation : unsigned int
{
    TEST_OP = 56000,

    op_onGameUpdate,

    op_checkUnits,
    op_checkCriminals,

    op_getPlayerPosition,
    op_getPlayerVehicle,

    op_firstUpdate,

    op_pedsUpdate,
    op_vehiclesUpdate,

    op_pulloverUpdate,
    op_criminalsUpdate,

    op_bottomMessageUpdate,
    op_topMessageUpdate,

    op_chaseUpdate,
    op_backupUnitsUpdate,

    op_aiControllerUpdate,
    op_escortUpdate,

    op_policeBasesUpdate,
    op_checkpointsUpdate,

    op_calloutsUpdate,

    op_audioSequence,
    op_radioSoundsUpdate,

    op_cleoFunctionsUpdate,

    op_destroyPeds,
    op_destroyVehicles,

    op_onPedAdded,
    op_onPedRemoved,

    op_onVehicleAdded,
    op_onVehicleRemoved,
};

#define BEGIN_OPERATION(operation) menuSZK->BeginOperation(operation, "PoliceMod_" #operation, "")
#define BEGIN_OPERATION_DESC(operation, description) menuSZK->BeginOperation(operation, "PoliceMod_" #operation, description)
#define END_OPERATION(operation) menuSZK->EndOperation(operation)
#define END_OPERATION_RESULT(operation, result) menuSZK->EndOperation(operation, result)