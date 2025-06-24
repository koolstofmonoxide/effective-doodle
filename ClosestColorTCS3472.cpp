#include "ClosestColor.h"

const Color ClosestColor::colors[] = {
    {0, 0, 0, "Black"},
    {255, 255, 255, "White"},
    {255, 0, 0, "Red"},
    {0, 255, 0, "Lime"},
    {0, 0, 255, "Blue"},
    {255, 255, 0, "Yellow"},
    {0, 255, 255, "Cyan"},
    {255, 0, 255, "Magenta"},
    {192, 192, 192, "Silver"},
    {128, 128, 128, "Gray"},
    {128, 0, 0, "Maroon"},
    {128, 128, 0, "Olive"},
    {0, 128, 0, "Green"},
    {128, 0, 128, "Purple"},
    {0, 128, 128, "Teal"},
    {0, 0, 128, "Navy"}};

ClosestColor::ClosestColor() {}

int ClosestColor::distanceBetweenToColors(int r1, int g1, int b1, int r2, int g2, int b2)
{
    return (r1 - r2) * (r1 - r2) + (g1 - g2) * (g1 - g2) + (b1 - b2) * (b1 - b2);
}

String ClosestColor::getClosestColor(int r, int g, int b)
{
    int closestDistance = INT8_MAX; // Largest int possible
    String closestColor = "Unknown";

    int numColors = sizeof(colors) / sizeof(colors[0]);

    for (int i = 0; i < numColors; i++)
    {
        int distance = distanceBetweenToColors(r, g, b, colors[i].R_, colors[i].G_, colors[i].B_);
        if (distance < closestDistance)
        {
            closestDistance = distance;
            closestColor = colors[i].name_;
        }
    }

    return closestColor;
}