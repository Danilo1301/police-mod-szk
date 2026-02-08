#include "Ped.h"

#include "CleoFunctions.h"
#include "Pullover.h"
#include "Peds.h"
#include "Vehicles.h"
#include "WorldWidgets.h"
#include "Criminals.h"
#include "InventoryItemManager.h"
#include "Checkpoint.h"
#include "FriskWindow.h"
#include "Names.h"
#include "Escort.h"

Ped::Ped(int ref, void* ptr)
{
    this->ref = ref;
    this->ptr = ptr;

    wasAlive = ACTOR_HEALTH(ref) > 0;

    bool isMale = IS_CHAR_MALE(ref);
    auto currentYear = getCurrentYear();

    rg = randomRG();
    cpf = randomCPF();
    birthDate = randomBirthDate(1970, 2000);
    name = Names::GetName(isMale);

    catHab = randomCatHab();
    cnhExpireDate = gerarValidadeCNH(currentYear - 3, currentYear + 10);

    if(calculateProbability(0.10))
    {
        catHab = "";
    }

    TryInitializeInventory();

    flags.wantedByJustice = calculateProbability(CHANCE_BEEING_WANTED_BY_JUSTICE);

    if(flags.wantedByJustice)
    {
        flags.willSurrender = calculateProbability(0.40);
    } else {
        flags.willSurrender = true;
    }

    if(flags.willSurrender == false)
    {
        flags.willKillCops = calculateProbability(CHANCE_CRIMINAL_KILL_COPS);
    }

    UpdateSeatPosition();
}

Ped::~Ped()
{
    if(widgetOptions)
    {
        widgetOptions->Close();
        widgetOptions = nullptr;
    }
}

void Ped::Update()
{
    auto pedPosition = GetPosition();
    auto playerPosition = *g_playerPosition;

    PerformAnims();

    if(flags.showBackCheckpoint)
    {
        auto checkpointPosition = GetPedPositionWithOffset(ref, CVector(0, -2, 0));;

        if(backCheckpoint == nullptr)
        {
            backCheckpoint = Checkpoints::CreateCheckpoint(checkpointPosition);
            backCheckpoint->onEnterCheckpoint = [this]() {
                OnEnterBackCheckpoint();
            };
        }

        if(backCheckpoint)
        {
            backCheckpoint->position = checkpointPosition;
            backCheckpoint->CheckEntered(playerPosition);
        }
    } else {
        if(backCheckpoint != nullptr)
        {
            Checkpoints::DestroyCheckpoint(backCheckpoint);
            backCheckpoint = nullptr;
        }
    }

    if(widgetOptions == nullptr)
    {
        auto widget = menuSZK->CreateWidget(
            200,
            200,
            100,
            modData->GetFileFromMenuSZK("assets/widget_background1.png"),
            modData->GetFile("assets/widgets/widget_pullover.png")
        );
        widget->visible = false;

        WorldWidget w;
        w.widget = widget;
        w.attachToPed = ref;
        WorldWidgets.push_back(w);

        widget->onClick->Add([this]() {

            if(Escort::IsPedBeeingCarried(this))
            {
                Escort::OpenCarryingPedOptions(this);
                return;
            }

            Pullover::OpenPedMenu(this);
        });

        widgetOptions = widget;
    }

    if(widgetOptions)
    {
        bool widgetVisible = false;

        if(distanceBetweenPoints(pedPosition, playerPosition) < 5.0f)
        {
            widgetVisible = flags.showWidget;
        }

        if(g_blockInteractions)
        {
            widgetVisible = false;
        }

        widgetOptions->visible = widgetVisible;
    }

    if(ACTOR_DEAD(ref) && wasAlive && CREATE_INJURED_PED)
    {
        wasAlive = false;
        flags.isInconcious = true;

        if(CAR_DEFINED(vehicleOwned))
        {
            Vehicles::GetVehicle(vehicleOwned)->ValidateOwners();
        }

        if(CAR_DEFINED(previousVehicle))
        {
            Vehicles::GetVehicle(previousVehicle)->ValidateOwners();
        }

        auto ref = this->ref;

        WAIT(6000, [ref]() {

            if(ref == GetPlayerActor()) return;

            if(!ACTOR_DEFINED(ref)) return;

            auto ped = Peds::GetPed(ref);
            auto modelId = GET_ACTOR_MODEL(ref);
            auto position = ped->GetPosition();

            auto newPedRef = CREATE_ACTOR_PEDTYPE(PedType::CivMale, modelId, position.x, position.y, position.z);
            auto newPed = Peds::RegisterPed(newPedRef);

            newPed->CopyFrom(*ped);

            if(Criminals::IsCriminal(ped))
            {
                Criminals::AddCriminal(newPed);
                Criminals::RemoveCriminal(ped);
            }

            newPed->flags.isInconcious = true;
            newPed->flags.hasSurrended = false;
            newPed->flags.showWidget = false;
            newPed->flags.willSurrender = true;

            PERFORM_ANIMATION_AS_ACTOR(newPedRef, "crckdeth2", "CRACK", 10.0f, 0, 0, 0, 1, -1);

            DESTROY_ACTOR(ref);
            Peds::RemovePed(ref);

            auto cped = (CPed*)newPed->ptr;
            cped->m_matrix->at = CVector(position.x, position.y, position.z - 0.3f);
        });
    }

    if(isLeavingCar)
    {
        bool isIn = IsInAnyCar();

        if(!isIn)
        {
            isLeavingCar = false;
            menuDebug->AddLine("~y~Ped left the vehicle");

            g_onPedLeaveVehicle->Emit(ref);
        }
    }

    if(isEnteringCar)
    {
        bool isIn = IsInAnyCar();

        if(isIn)
        {
            isEnteringCar = false;
            menuDebug->AddLine("~y~Ped entered the vehicle");

            g_onPedEnterVehicle->Emit(ref);
        }
    }

    if(flags.isInconcious)
    {
        auto distanceFromPlayer = distanceBetweenPoints(pedPosition, playerPosition);

        if(distanceFromPlayer > 200.0f)
        {
            QueueDestroy();
            return;
        }
    }
}

void Ped::SetCanDoHandsup()
{
    SetAnim("handsup", "PED");

    PerformAnims();
}

void Ped::PerformAnims()
{
    if(isLeavingCar) return;
    if(isEnteringCar) return;

    if(flags.isInconcious) return;

    if(currentAnim.length() == 0) return;

    if(!IsPerformingAnimation(currentAnim))
    {
        CLEAR_ACTOR_TASK(ref);
        PERFORM_ANIMATION_AS_ACTOR(ref, currentAnim.c_str(), currentAnimGroup.c_str(), 4.0f, 0, 0, 0, 1, -1);
    }
}

bool Ped::IsPerformingAnimation(const std::string& animName)
{
    return ACTOR_PERFORMING_ANIMATION(ref, animName.c_str());
}

void Ped::ShowBlip(CRGBA color)
{
    flags.showBlip = true;
    flags.blipColor = color;
}

void Ped::HideBlip()
{
    flags.showBlip = false;
}

CVector Ped::GetPosition()
{
    return GetPedPosition(ref);
}

void Ped::SetPosition(CVector position)
{
    auto cped = (CPed*)ptr;
    auto matrix = cped->GetMatrix();

    matrix->pos = position;
}

bool Ped::IsInAnyCar()
{
    return IS_CHAR_IN_ANY_CAR(ref);
}

int Ped::GetCurrentCar()
{
    return GetVehiclePedIsUsing(ref);
}

bool Ped::IsDriver()
{
    auto car = GetCurrentCar();

    if(car < 0) return false;

    return GET_DRIVER_OF_CAR(car) == ref;
}

void Ped::LeaveCar()
{
    if(!IsInAnyCar()) return;

    UpdateSeatPosition();
    isLeavingCar = true;

    EXIT_CAR_AS_ACTOR(ref);
}

void Ped::UpdateSeatPosition()
{
    fileLog->Log("UpdateSeatPosition");

    previousVehicle = -1;

    if(!IsInAnyCar())
    {
        prevSeatId = -1;
        return;
    } 

    auto carRef = GetCurrentCar();

    previousVehicle = carRef;
    prevSeatId = Vehicle::GetCurrentSeatOfPed(carRef, ref);
}

void Ped::EnterVehicle(int vehicleRef, int seatId)
{
    previousVehicle = vehicleRef;
    prevSeatId = seatId;

    if(!CAR_DEFINED(vehicleRef)) return;

    if(IsInAnyCar())
    {
        menuDebug->AddLine("~r~cant enter vehicle: already in one");
        return;
    }

    if(seatId < 0)
    {
        menuDebug->AddLine("~r~cant enter vehicle: seat is NONE");
        return;
    }

    ClearAnim();
    CLEAR_ACTOR_TASK(ref);

    isEnteringCar = true;

    auto ped = this;
    auto vehicle = Vehicles::GetVehicle(vehicleRef);
    auto vehiclePos = vehicle->GetPosition();

    TASK_GO_STRAIGHT_TO_COORD(ref, vehiclePos.x, vehiclePos.y, vehiclePos.z, 6, -1);

    CleoFunctions::AddWaitForFunction("fn", [ped, vehicle]() {
        if(!Peds::IsValid(ped)) return true;
        if(!Vehicles::IsValid(vehicle)) return true;

        auto distance = distanceBetweenPoints(ped->GetPosition(), vehicle->GetPosition());

        if(distance < 10) return true;

        return false;
    }, [ped, vehicle, seatId]() {
        if(!Peds::IsValid(ped)) return;
        if(!Vehicles::IsValid(vehicle)) return;

        SET_CHAR_STAY_IN_CAR_WHEN_JACKED(ped->ref, true);
        
        if(seatId == 0)
        {
            ENTER_CAR_AS_DRIVER_AS_ACTOR(ped->ref, vehicle->ref, 10000);
        } else if(seatId >= 1)
        {
            ACTOR_ENTER_CAR_PASSENGER_SEAT(ped->ref, vehicle->ref, 10000, seatId - 1);
        }
    });
}

void Ped::EnterPreviousVehicle()
{
    menuDebug->AddLine("enter previous vehicle");

    auto vehicle = Vehicles::GetVehicle(previousVehicle);
    if(!vehicle)
    {
        menuDebug->AddLine("~r~no valid vehicle");
        return;
    }

    vehicle->ValidateOwners();

    int seatId;

    if(!vehicle->GetSeatThatPedBelongs(ref, seatId))
    {
        menuDebug->AddLine("~r~ped does not belong to vehicle");
        return;
    }

    EnterVehicle(previousVehicle, seatId);
}

void Ped::StartDrivingRandomly()
{
    if(!IsInAnyCar()) return;
    if(!IsDriver()) return;

    auto car = GetCurrentCar();

    REMOVE_REFERENCES_TO_CAR(car);
    SET_CAR_ENGINE_OPERATION(car, true);
    SET_CAR_TRAFFIC_BEHAVIOUR(car, DrivingMode::StopForCars);
    SET_CAR_TO_PSYCHO_DRIVER(car);
    SET_CAR_MAX_SPEED(car, 20.0f);
}

void Ped::SetAnim(std::string anim, std::string animGroup)
{
    currentAnim = anim;
    currentAnimGroup = animGroup;
}

void Ped::ClearAnim()
{
    currentAnim = "";
    currentAnimGroup = "";
    ClearPedAnimations(ref);
}

void Ped::InitializeOnVehicle(int vehicleRef)
{
    auto vehicle = Vehicles::GetVehicle(vehicleRef);

    if(vehicle->originalDoc.isStolen || vehicle->flags.swappedPlate || vehicle->flags.chassisErased)
    {
        auto occupants = vehicle->GetCurrentOccupants();

        bool willSurrender = calculateProbability(CHANCE_RUNNING_AWAY_WHEN_VEHICLE_IRREGULAR);
        bool willKillCops = false;

        if(willSurrender == false)
        {
            willKillCops = calculateProbability(CHANCE_CRIMINAL_KILL_COPS);
        }

        for(auto pedRef : occupants)
        {
            auto ped = Peds::GetPed(pedRef);

            ped->flags.willSurrender = willSurrender;
            ped->flags.willKillCops = willKillCops;
        }
    }

    // if(menuSZK->debug->enabled)
    // {
    //     if(flags.willSurrender == false && flags.showBlip == false)
    //     {
    //         ShowBlip(CRGBA(80, 0, 0));
    //     }
    // }
}

void Ped::CopyFrom(const Ped& other)
{
    int oldRef = ref;
    void* oldPtr = ptr;

    *this = other; // reutiliza o operador=

    ref = oldRef;
    ptr = oldPtr;
    widgetOptions = nullptr;

    if (vehicleOwned > 0)
    {
        auto vehicle = Vehicles::GetVehicle(vehicleOwned);

        if (!vehicle)
            return;

        // 🚮 Remove o ped antigo da lista de passageiros (se existir)
        auto& passengers = vehicle->ownerPassengers;
        passengers.erase(
            std::remove(passengers.begin(), passengers.end(), oldRef),
            passengers.end()
        );

        // 🚗 Atualiza o novo dono conforme a posição no veículo
        if (prevSeatId == 0)
        {
            vehicle->ownerDriver = ref;
        }
        else if (prevSeatId > 0)
        {
            passengers.push_back(ref);
        }
    }
}

void Ped::Reanimate()
{
    ClearAnim();
    flags.isInconcious = false;
    flags.isBeingTreated = false;
    
    if(Criminals::IsCriminal(this))
    {
        flags.hasSurrended = true;
        SetCanDoHandsup();
    } else {
        HideBlip();
        flags.showWidget = false;

        REMOVE_REFERENCES_TO_ACTOR(ref);
    }
}

void Ped::DestroyImmediate()
{
    if(!ACTOR_DEFINED(ref)) return;

    DESTROY_ACTOR(ref);
    Peds::RemovePed(ref);
}

void Ped::QueueDestroy()
{
    QueueDestroyPed(ref);
}

bool Ped::IsDeadOrInconcious()
{
    if(!ACTOR_DEFINED(ref)) return true;
    if(ACTOR_DEAD(ref)) return true;
    if(flags.isInconcious) return true;
    return false;
}

void Ped::TryInitializeInventory()
{
    if(inventory.HasInitialized()) return;

    inventory.MarkAsInitialized();

    for(const auto& pair : InventoryItemManager::itemDefinitions)
    {
        auto id = pair.first;
        auto def = pair.second;

        if(!ItemDefinitionContainFlag(*def, "can_spawn_on_any_ped"))
            continue;
    
        if(calculateProbability(def->chance))
        {
            auto amount = getRandomNumber(1, def->maxAmount);

            inventory.AddItem(id, amount);
        }
    }
}

void Ped::OnEnterBackCheckpoint()
{
    menuDebug->AddLine("entrou no checkpoint");

    flags.showBackCheckpoint = false;

    if(flags.canShowFriskMenu)
    {
        auto playerActor = GetPlayerActor();
        
        WAIT(100, [this, playerActor]() {
            auto heading = GET_CHAR_HEADING(ref);
            auto newPos = GetPedPositionWithOffset(ref, CVector(0, -1, 0));

            newPos.z -= 1.0f;

            SET_CHAR_COORDINATES(playerActor, newPos.x, newPos.y, newPos.z);
            SET_CHAR_HEADING(playerActor, heading);
            FREEZE_CHAR_POSITION(playerActor, true);

            PERFORM_ANIMATION_AS_ACTOR(playerActor, "hndshkfa_swt", "gangs", 2.0f, 0, 0, 0, 0, -1);
        });

        WAIT(3000, [this, playerActor]() {
            FREEZE_CHAR_POSITION(playerActor, false);
            FriskWindow::OpenForPed(this);
        });

        return;
    }
}