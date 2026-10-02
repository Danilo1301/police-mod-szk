#include "Biqueiras.h"

#include "../utils/utils.h"

std::vector<Location> g_biqueiras;

void Biqueiras::Initialize() {
    {
        Location location;
        location.position = CVector(2362.81, -1771.92, 13.55);
        location.rotation = CVector(0, 0, 270);
        g_biqueiras.push_back(location);
    }

    {
        Location location;
        location.position = CVector(2171.47, -1673.27, 15.09);
        location.rotation = CVector(0, 0, 0);
        g_biqueiras.push_back(location);
    }

    {
        Location location;
        location.position = CVector(2182.26, -1341.58, 23.98);
        location.rotation = CVector(0, 0, 0);
        g_biqueiras.push_back(location);
    }

    {
        Location location;
        location.position = CVector(1993.58, -1094.40, 24.94);
        location.rotation = CVector(0, 0, 0);
        g_biqueiras.push_back(location);
    }
}

Location Biqueiras::GetRandomCloseBiqueira(CVector point) {
    if (g_biqueiras.empty()) {
        Location location;
        return location;
    }

    int limit = std::min(3, (int)g_biqueiras.size());

    std::vector<Location> closestLocations(g_biqueiras.begin(), g_biqueiras.begin() + g_biqueiras.size());

    std::partial_sort(
        closestLocations.begin(), closestLocations.begin() + limit, closestLocations.end(), [&point](const Location& a, const Location& b) {
            return distanceBetweenPoints(a.position, point) < distanceBetweenPoints(b.position, point);
        });

    // pegar random entre 0 e limit-1 usando sua função
    int index = getRandomNumber(0, limit - 1);

    return closestLocations[index];
}