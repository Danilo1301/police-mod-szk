#include "Vehicle.h"

#include "Checkpoint.h"
#include "Escort.h"
#include "Peds.h"
#include "PoliceVehicles.h"
#include "Pullover.h"
#include "Trunk.h"
#include "Vehicles.h"
#include "WorldWidgets.h"
#include "opcodeCaller/CleoFunctions.h"

extern bool freezingCriminals;

Vehicle::Vehicle(int ref, void* ptr)
{
    this->ref = ref;
    this->ptr = ptr;

    trunk = new Trunk(ref);

    {
        auto widget = menuSZK->CreateWidget(200, 200, 100, "", modData->GetFile("assets/widgets/widget_frisk_car.png"));

        widget->SetVisible(false);
        widget->GetContainer()->canBlockTouchEvents = true;

        worldWidget = new WorldWidget();
        worldWidget->widget = widget;
        worldWidget->attachToVehicle = ref;

        widget->onClick->Add([this]() { Pullover::OpenVehicleMenu(this); });

        widgetOptions = widget;
    }

    originalDoc.isStolen = calculateProbability(0.20);
    originalDoc.isDocumentExpired = calculateProbability(0.20);

    flags.swappedPlate = calculateProbability(originalDoc.isStolen ? 0.50 : 0.05);
    if (flags.swappedPlate == false) { currentDoc = originalDoc; }

    if (originalDoc.isStolen)
    {
        if (calculateProbability(0.50)) { flags.chassisErased = true; }
    }

    policeVehicleData = GetPoliceVehicleDataByModelId(GetModelId());
}

Vehicle::~Vehicle()
{
    // fileLog->Log("Vehicle: ~Vehicle");

    if (widgetOptions)
    {
        widgetOptions->Destroy();
        widgetOptions = nullptr;
    }

    if (worldWidget)
    {
        delete worldWidget;
        worldWidget = nullptr;
    }

    if (trunk)
    {
        delete trunk;
        trunk = nullptr;
    }

    if (trunkCheckpoint != nullptr)
    {
        Checkpoints::DestroyCheckpoint(trunkCheckpoint);
        trunkCheckpoint = nullptr;
    }
}

void Vehicle::Update()
{
    timeAlive = std::min(timeAlive + g_deltaTime, 100000);
    timeSinceLastRepair = std::min(timeSinceLastRepair + g_deltaTime, 100000);

    auto carPosition = GetPosition();
    auto playerPosition = *g_playerPosition;

    if (widgetOptions)
    {
        bool widgetVisible = false;

        if (distanceBetweenPoints(carPosition, playerPosition) < 10.0f)
        {
            widgetVisible = flags.showWidget;

            if (g_canShowCarWidgetAnyTime) widgetVisible = true;
        }

        widgetOptions->SetVisible(widgetVisible);
    }

    if (!hasInitializedPeds)
    {
        logger->Info("initializing peds of vehicle, from update");

        TryInitializePedsInside();
    }

    auto carHealth = GET_CAR_HEALTH(ref);
    if (carHealth < 300 && timeSinceLastRepair > 1000)
    {
        timeSinceLastRepair = 0;
        SET_CAR_HEALTH(ref, 350);
        logger->Info("car health restored");
    }

    if (policeVehicleData != nullptr)
    {
        if (Escort::IsCarryingSomeone())
        {
            auto trunkPosition = GetCarPositionWithOffset(ref, trunkOffset);

            if (!trunkCheckpoint) { trunkCheckpoint = Checkpoints::CreateCheckpoint(trunkPosition); }

            trunkCheckpoint->position = trunkPosition;
        }
        else
        {
            if (trunkCheckpoint != nullptr)
            {
                Checkpoints::DestroyCheckpoint(trunkCheckpoint);
                trunkCheckpoint = nullptr;
            }
        }
    }
}

void Vehicle::ShowBlip(CRGBA color)
{
    flags.showBlip = true;
    flags.blipColor = color;
}

void Vehicle::HideBlip()
{
    flags.showBlip = false;
}

void Vehicle::RemoveBlips()
{
    HideBlip();

    auto occupants = GetCurrentOccupants();

    for (auto pedRef : occupants)
    {
        auto ped = Peds::GetPed(pedRef);
        ped->HideBlip();
    }
}

CVector Vehicle::GetPosition()
{
    return GetCarPosition(ref);
}

bool Vehicle::IsPlayerInside()
{
    return false;
}

int Vehicle::GetCurrentDriver()
{
    auto refDriver = GET_DRIVER_OF_CAR(ref);
    return refDriver;
}

bool Vehicle::HasDriver()
{
    auto driver = GetCurrentDriver();
    return driver > 0;
}

std::vector<int> Vehicle::GetCurrentPassengers()
{
    std::vector<int> passengers;

    auto maxPassengers = CAR_MAX_PASSENGERS(ref);

    for (int seatId = 0; seatId < maxPassengers; seatId++)
    {
        if (!CAR_PASSENGER_SEAT_FREE(ref, seatId))
        {
            auto passengerHandle = GET_ACTOR_HANDLE_FROM_CAR_PASSENGER_SEAT(ref, seatId);
            passengers.push_back(passengerHandle);
        }
    }

    return passengers;
}

std::vector<int> Vehicle::GetCurrentOccupants()
{
    std::vector<int> occupants;

    auto driverRef = GetCurrentDriver();
    if (driverRef > 0) { occupants.push_back(driverRef); }

    auto passengers = GetCurrentPassengers();
    occupants.insert(occupants.end(), passengers.begin(), passengers.end());

    return occupants;
}

bool Vehicle::GetSeatThatPedBelongs(int pedRef, int& seatId)
{
    // DRIVER É seatId 0
    if (ownerDriver == pedRef)
    {
        seatId = 0;
        return true;
    }

    // PASSAGEIROS seatId 0 até 3
    for (int i = 0; i < ownerPassengers.size(); i++)
    {
        if (ownerPassengers[i] == pedRef)
        {
            seatId = i + 1;
            return true;
        }
    }

    seatId = -1;
    return false;
}

int Vehicle::GetCurrentSeatOfPed(int carRef, int pedRef)
{
    auto driverRef = GET_DRIVER_OF_CAR(carRef);

    if (driverRef > 0)
    {
        if (driverRef == pedRef) { return 0; }
    }

    auto maxPassengers = CAR_MAX_PASSENGERS(carRef);

    for (int seatId = 0; seatId < maxPassengers; seatId++)
    {
        if (!CAR_PASSENGER_SEAT_FREE(carRef, seatId))
        {
            auto passengerHandle = GET_ACTOR_HANDLE_FROM_CAR_PASSENGER_SEAT(carRef, seatId);

            if (pedRef == passengerHandle) return seatId + 1;
        }
    }

    return -1;
}

void Vehicle::MakeOccupantsLeave()
{
    auto ocuppants = GetCurrentOccupants();

    for (auto pedRef : ocuppants)
    {
        auto ped = Peds::GetPed(pedRef);
        ped->LeaveCar();
    }
}

void Vehicle::MakeOwnersEnter()
{
    auto owners = GetOwners();

    for (auto pedRef : owners)
    {
        auto ped = Peds::GetPed(pedRef);

        int seatId;

        GetSeatThatPedBelongs(pedRef, seatId);

        ped->EnterVehicle(ref, seatId);
    }
}

void Vehicle::SetOwners()
{
    logger->Info("Vehicle::SetOwners");

    ownerPassengers.clear();
    ownerDriver = -1;

    auto driverRef = GetCurrentDriver();
    if (driverRef > 0)
    {
        ownerDriver = driverRef;

        auto ped = Peds::GetPed(driverRef);
        ped->vehicleOwned = ref;
    }

    auto passengersRef = GetCurrentPassengers();
    for (auto passengerRef : passengersRef) { ownerPassengers.push_back(passengerRef); }

    TryInitializePedsInside();
}

std::vector<int> Vehicle::GetOwners()
{
    std::vector<int> owners;

    if (ownerDriver > 0)
    {
        if (ACTOR_DEFINED(ownerDriver)) { owners.push_back(ownerDriver); }
    }

    if (ownerPassengers.size() > 0)
    {
        for (auto passengerRef : ownerPassengers)
        {
            if (passengerRef <= 0) continue;
            if (!ACTOR_DEFINED(passengerRef)) continue;

            owners.push_back(passengerRef);
        }
    }
    return owners;
}

void Vehicle::ValidateOwners()
{
    // caso não tenha referência válida do veículo
    if (!CAR_DEFINED(ref)) return;

    // checa se o motorista atual ainda é válido
    bool hasValidDriver = ACTOR_DEFINED(ownerDriver);
    if (hasValidDriver)
    {
        auto driver = Peds::GetPed(ownerDriver);
        if (!driver || driver->IsDeadOrInconcious()) hasValidDriver = false;
    }

    // se não tem motorista válido, tenta promover um passageiro
    if (!hasValidDriver)
    {
        int newDriverRef = -1;

        // percorre os passageiros procurando um válido
        for (auto it = ownerPassengers.begin(); it != ownerPassengers.end(); ++it)
        {
            auto pedRef = *it;

            if (!ACTOR_DEFINED(pedRef)) continue;

            auto ped = Peds::GetPed(pedRef);
            if (!ped) continue;
            if (ped->IsDeadOrInconcious()) continue;

            // achou um bom candidato
            newDriverRef = pedRef;
            ownerDriver = pedRef;
            ped->vehicleOwned = ref;

            // remove dos passageiros, já que agora é o motorista
            ownerPassengers.erase(it);
            break;
        }

        // se achou um novo motorista, pode logar ou atualizar algo
        if (newDriverRef > 0)
        {
            // fileLog->Log("Vehicle: Promoted passenger to driver");
        }
        else
        {
            // nenhum passageiro válido -> veículo fica sem motorista
            ownerDriver = -1;
            // fileLog->Log("Vehicle: Driver dead and no passengers to promote");
        }
    }

    // remove passageiros mortos/inválidos
    std::vector<int> validPassengers;
    for (auto pedRef : ownerPassengers)
    {
        if (!ACTOR_DEFINED(pedRef)) continue;

        auto ped = Peds::GetPed(pedRef);
        if (!ped || ped->IsDeadOrInconcious()) continue;

        validPassengers.push_back(pedRef);
    }

    ownerPassengers = validPassengers;
}

void Vehicle::DestroyImmediate()
{
    if (!CAR_DEFINED(ref)) return;

    DESTROY_CAR(ref);
    Vehicles::RemoveVehicle(ref);
}

void Vehicle::QueueDestroy(bool destroyOccupants)
{
    QueueDestroyVehicle(ref);

    if (destroyOccupants)
    {
        for (auto ref : GetCurrentOccupants())
        {
            auto ped = Peds::GetPed(ref);
            ped->QueueDestroy();
        }
    }
}

void Vehicle::TryInitializePedsInside()
{
    if (hasInitializedPeds) return;
    hasInitializedPeds = true;

    // fileLog->Log("Vehicle: TryInitializePedsInside");

    auto ocuppants = GetCurrentOccupants();

    if (ocuppants.size() > 0)
    {
        for (auto pedRef : ocuppants)
        {
            auto ped = Peds::GetPed(pedRef);

            ped->InitializeOnVehicle(ref);
        }
    }
}

int Vehicle::GetModelId()
{
    if (modelId <= 0) { modelId = GET_CAR_MODEL(ref); }

    return modelId;
}

bool Vehicle::IsAllOwnersInside()
{
    auto owners = GetOwners();

    if (owners.size() == 0) return false;
    if (GetCurrentOccupants().size() == 0) return false;

    for (auto ownerRef : owners)
    {
        auto owner = Peds::GetPed(ownerRef);

        if (!owner->IsInAnyCar()) return false;
    }

    return true;
}

bool Vehicle::IsAnyOwnersLeavingOrEnteringCar()
{
    auto owners = GetOwners();

    if (owners.size() == 0) return false;

    for (auto ownerRef : owners)
    {
        auto owner = Peds::GetPed(ownerRef);

        if (owner->isLeavingCar) return true;
        if (owner->isEnteringCar) return true;
    }

    return false;
}

float Vehicle::GetSpeed()
{
    return CAR_SPEED(ref);
}

CRGBA Vehicle::GetBlipColor()
{
    if (ownerDriver > 0 && ACTOR_DEFINED(ownerDriver))
    {
        auto driver = Peds::GetPed(ownerDriver);

        if (driver)
        {
            if (driver->flags.showBlip) return driver->flags.blipColor;
        }
    }

    for (auto occupantRef : GetCurrentOccupants())
    {
        auto ped = Peds::GetPed(occupantRef);

        if (ped)
        {
            if (ped->flags.showBlip) { return ped->flags.blipColor; }
        }
    }

    return flags.blipColor;
}

void Vehicle::RemoveReferences()
{
    REMOVE_REFERENCES_TO_CAR(ref);

    for (auto pedRef : GetCurrentOccupants()) { REMOVE_REFERENCES_TO_ACTOR(pedRef); }
}