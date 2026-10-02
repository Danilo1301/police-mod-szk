#pragma once

struct AIVehicle {
public:
    int vehicleRef = -1;

    virtual ~AIVehicle();

    virtual void Start();
    virtual void Update();
};