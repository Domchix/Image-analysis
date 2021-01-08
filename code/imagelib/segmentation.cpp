#include "image.hpp"

void Image::Treshold(unsigned char *data, int treshold)
{
    for (uint32 i = 0; i < getImageSize(); i++)
    {
        data[i] = data[i] > treshold ? MAX_INTENSITY : MIN_INTENSITY;
    }
}

void Image::TresholdReverse(unsigned char *data, int treshold)
{
    for (uint32 i = 0; i < getImageSize(); i++)
    {
        if (data[i] > treshold)
            data[i] = MIN_INTENSITY;
    }
}

void Image::LabelComponent(unsigned char *data, int *labels, Component &component, uint32 x, uint32 y)
{
    int index = x + _width * y;

    if (labels[index] != -1)
    {
        // Pixel already labeled
        return;
    }
    if (data[index] != component.Intensity)
    {
        // Pixel not same intensity as component
        return;
    }

    // Adding pixel to this component
    labels[index] = component.Label;
    component.Pixels.push_back(index);

    // We are using N4 connectivity scheme
    if (x > 0)
        LabelComponent(data, labels, component, x - 1, y);
    if (x < _width - 1)
        LabelComponent(data, labels, component, x + 1, y);
    if (y > 0)
        LabelComponent(data, labels, component, x, y - 1);
    if (y < _height - 1)
        LabelComponent(data, labels, component, x, y + 1);
}

void Image::CCL(unsigned char *data, int *labels, std::vector<Component> &components)
{
    components.clear();
    // Set labels of all pixels to nonexistent
    for (uint32 index = 0; index < getImageSize(); index++)
    {
        labels[index] = -1;
    }

    int labelNo = 0;
    for (uint32 y = 0; y < _height; y++)
    {
        for (uint32 x = 0; x < _width; x++)
        {
            uint32 index = x + _width * y;
            if (labels[index] != -1)
            {
                continue; // Pixel already labeled
            }

            // Unlabeled pixel = new component
            Component component;
            component.Intensity = data[index];
            component.Label = labelNo;
            LabelComponent(data, labels, component, x, y);

            components.push_back(component);
            labelNo++;
        }
    }
}

void Image::FillHoles(unsigned char *data, int *labels, int componentToSkip)
{
    for (uint32 index = 0; index < getImageSize(); index++)
    {
        //skip selected component
        if (labels[index] == componentToSkip)
        {
            continue;
        }

        data[index] = MAX_INTENSITY;
    }
}

bool Image::ComponentInsideComponent(Component &outsideComponent, Component &insideComponent)
{
    for (uint32 pixel : insideComponent.Pixels)
    {
        if (std::find(outsideComponent.Pixels.begin(), outsideComponent.Pixels.end(), pixel) != outsideComponent.Pixels.end())
        {
            continue;
        }
        else
        {
            return false;
        }
    }

    return true;
}

bool Image::ComponentsIntersect(Component &mainComponent, Component &sideComponent)
{
    for (uint32 pixel : sideComponent.Pixels)
    {
        if (std::find(mainComponent.Pixels.begin(), mainComponent.Pixels.end(), pixel) != mainComponent.Pixels.end())
        {
            return true;
        }
    }

    return false;
}

bool Image::ComponentCenteredInsideComponent(Component &mainComponent, Component &insideComponent, int deltaError)
{
    int minX, minY, maxX, maxY;
    GetComponentBoundaries(mainComponent, minX, maxX, minY, maxY);

    int mainComponentCenterX = (minX + maxX) / 2;
    int mainComponentCenterY = (minY + maxY) / 2;

    GetComponentBoundaries(insideComponent, minX, maxX, minY, maxY);

    int insideComponentCenterX = (minX + maxX) / 2;
    int insideComponentCenterY = (minY + maxY) / 2;

    if (std::abs(mainComponentCenterX - insideComponentCenterX) > deltaError || std::abs(mainComponentCenterY - insideComponentCenterY) > deltaError)
    {
        return false;
    }

    return true;
}

bool Image::SolderingIslandCorrect(Component &component)
{
    int minX, minY, maxX, maxY;
    GetComponentBoundaries(component, minX, maxX, minY, maxY);

    float diameterX = maxX - minX + 1;
    float diameterY = maxY - minY + 1;
    float PIArea = M_PI * pow((diameterX) / 2.0, 2.0);
    float area = component.Pixels.size();

    if (abs(diameterX - diameterY) < diameterX / 10.0 && abs(area - PIArea) < area / 10.0)
    {
        // soldering island is circular
        return true;
    }
    else if (diameterX * diameterY == area)
    {
        // soldering island is rectangular
        return true;
    }
    return false;
}

void Image::GetComponentBoundaries(Component &component, int &minX, int &maxX, int &minY, int &maxY)
{
    minX = 100000;
    minY = 100000;
    maxX = -1;
    maxY = -1;

    for (uint32 pixel : component.Pixels)
    {
        int x = pixel % _width;
        int y = pixel / _width;
        if (x > maxX)
            maxX = x;
        if (x < minX)
            minX = x;
        if (y > maxY)
            maxY = y;
        if (y < minY)
            minY = y;
    }
}

void Image::FilterComponents(unsigned char *data, std::vector<Component> &components, int fitlerIntensity)
{
    for (Component component : components)
    {
        int intensity = component.Intensity != fitlerIntensity ? MIN_INTENSITY : MAX_INTENSITY;

        for (uint32 background : component.Pixels)
        {

            data[background] = intensity;
        }
    }
}

void Image::RemoveSmallComponents(std::vector<Component> &components, unsigned char *data, int size)
{
    for (Component component : components)
    {
        if (component.Label == 0)
            continue; // Skip background
        if (component.Pixels.size() < 10)
        {
            for (uint32 pixel : component.Pixels)
            {
                data[pixel] = MIN_INTENSITY;
            }
        }
    }
}
