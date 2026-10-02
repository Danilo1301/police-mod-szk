#include "VehicleDocInfo.h"

#include "../utils/utils.h"

VehicleDocInfo::VehicleDocInfo()
{
    plate = randomPlate();
    chassis = randomVIN();
    renavam = randomRENAVAM();

    isStolen = false;
    isDocumentExpired = false;
}