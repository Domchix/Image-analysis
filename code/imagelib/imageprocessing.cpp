#include "image.hpp"

using std::cout;
using std::endl;

void Image::FISHSignalCounts(FISHStage stage)
{
    _channels = 1;

    // DAPI cells
    int *DAPIlabels = new int[getImageSize()];
    std::vector<Component> DAPIcomponents;

    Treshold(_blueData, 20);
    Erosion(_blueData, 5, 5);
    Dilation(_blueData, 5);
    CCL(_blueData, DAPIlabels, DAPIcomponents);
    FillHoles(_blueData, DAPIlabels, 0); // Background is component 0
    CCL(_blueData, DAPIlabels, DAPIcomponents);
    cout << "DAPI cells found:" << DAPIcomponents.size() - 1 << std::endl; // Subtract background

    //Acredine mutations
    int *ACRElabels = new int[getImageSize()];
    std::vector<Component> ACREcomponents;
    Treshold(_redData, 130);
    //SetViewToSingleColor(Color::Red);
    CCL(_redData, ACRElabels, ACREcomponents);
    cout << "Acredine mutations found:" << ACREcomponents.size() - 1 << std::endl; // Subtract background

    //FITC mutations
    int *FITClabels = new int[getImageSize()];
    std::vector<Component> FITCcomponents;
    Treshold(_greenData, 30);
    //SetViewToSingleColor(Color::Green);
    //return;
    CCL(_greenData, FITClabels, FITCcomponents);
    cout << "FITC mutations found:" << FITCcomponents.size() - 1 << std::endl; // Subtract background

    int totalARCRused = 0;
    int totalFITCused = 0;

    for (Component cell : DAPIcomponents)
    {
        if (cell.Label == 0)
        {
            continue; // Skip background
        }

        int ACRECount = 0;
        int FITCCount = 0;
        // count Acredine in cell
        for (Component ACREmutation : ACREcomponents)
        {
            if (ACREmutation.Label == 0)
            {
                continue; // Skip background
            }

            // for (uint32 pixel : ACREmutation.Pixels)
            // {
            //     _blueData[pixel] = HIGHLIGHT_INTENSITY;
            // }

            if (ComponentsIntersect(cell, ACREmutation))
            {
                ACRECount++;
                totalARCRused++;
            }
        }
        // count FITC in cell
        for (Component FITCmutation : FITCcomponents)
        {
            if (FITCmutation.Label == 0)
            {
                continue; // Skip background
            }

            if (ComponentsIntersect(cell, FITCmutation))
            {
                FITCCount++;
                totalFITCused++;
                if (cell.Label == 3)
                {
                    // cout << "ACRE: " << ACREmutation.Label << endl;
                    // cout << "PIXELS: ";
                    for (uint32 pixel : FITCmutation.Pixels)
                    {
                        //cout << pixel << " ";
                        _blueData[pixel] = HIGHLIGHT_INTENSITY;
                    }
                    //cout << endl;
                }
            }
        }

        std::string ratio;
        ratio = FITCCount == 0 ? "N/A" : std::to_string((float)ACRECount / (float)FITCCount);
        cout << "Cell " << cell.Label << " Acredine mutations: " << ACRECount << "FITC mutations: " << FITCCount << " Ratio: " << ratio << endl;
    }


    cout << "Total Arc: " << totalARCRused << endl;
    cout << "Total FITC: " << totalFITCused << endl;
    SetViewToSingleColor(Color::Blue);
    updateHistogram();
}

void Image::CircuitBoard(CircuitBoardStage stage)
{
    RemoveSaltandPepper();
    int *labels = new int[getImageSize()];
    std::vector<Component> components;
    cout << "Intensity:" << (int)_data[getImageSize() / 2 - 110] << endl;

    unsigned char *otherComponentsData = new unsigned char[getImageSize()];
    CopyData(_data, otherComponentsData, getImageSize());
    CCL(otherComponentsData, labels, components);
    FilterComponents(otherComponentsData, components, 96);
    Dilation(otherComponentsData, 3);
    CCL(otherComponentsData, labels, components);
    std::vector<Component> otherComponents = components;

    CCL(_data, labels, components);

    cout << "background intensity " << (int)_data[components[0].Pixels[0]] << endl;
    cout << "wire intensity " << (int)_data[components[1].Pixels[0]] << endl;

    //Filter wires
    FilterComponents(_data, components, WIRE_INTENSITY);
    CCL(_data, labels, components);

    // Remember holes and fill
    std::vector<Component> solderingIslandHoles;
    FillHoles(_data, components, solderingIslandHoles);
    CCL(_data, labels, components);

    std::vector<Component> wires = components;
    cout << "components: " << components.size() << endl;

    RemoveWires(_data);
    // We can try erosion and dilation but components loses form
    // Erosion(_data, 1, 5);
    // Erosion(_data, 5, 1);
    // Dilation(_data, 5);
    CCL(_data, labels, components);
    updateHistogram();

    std::vector<Component> badWires;

    for (Component wire : wires)
    {
        if (wire.Label == 0)
            continue;
        int elementsConnected = 0;
        for (Component component : components)
        {
            if (component.Label == 0)
                continue;

            if (ComponentsIntersect(wire, component))
            {
                elementsConnected++;
            }
        }
        for (Component otherComponent : otherComponents)
        {
            if (otherComponent.Label == 0)
                continue;

            if (ComponentsIntersect(wire, otherComponent))
            {
                elementsConnected++;
            }
        }
        cout << "components connected to wire " << wire.Label << ": " << elementsConnected << endl;
        if (elementsConnected < 2)
        {
            badWires.push_back(wire);
        }
    }

    // Remove connectors
    for (Component component : components)
    {
        if (component.Label == 0)
            continue;
        int intensity = component.Pixels.size() < SOLDERING_ISLAND_DELTA ? MIN_INTENSITY : MAX_INTENSITY;

        for (uint32 pixel : component.Pixels)
        {
            _data[pixel] = intensity;
        }
    }
    CCL(_data, labels, components);

    // Check if holes in correct places
    for (Component component : components)
    {
        if (component.Label == 0)
            continue;
        bool foundHole = false;
        for (Component hole : solderingIslandHoles)
        {
            if (hole.Label == 0)
                continue;
            // Also checking if hole is fully inside soldeing island!
            if (ComponentInsideComponent(component, hole) == true)
            {
                foundHole = true;
                // Check if centered
                if (ComponentCenteredInsideComponent(component, hole, 1) == false)
                {
                    cout << "Component " << component.Label << " hole is not centered!" << endl;
                    for (uint32 pixel : hole.Pixels)
                    {
                        _data[pixel] = HIGHLIGHT_INTENSITY;
                    }
                }
            }
        }

        if (foundHole == false)
        {
            std::cout << "Could not found a hole for component " << component.Label << endl;
        }
    }

    // Check for incorrect soldering islands
    int incorrectComponents = 0;
    for (Component component : components)
    {
        if (component.Label == 0)
            continue; // Skip background

        if (SolderingIslandCorrect(component) == false)
        {
            for (uint32 pixel : component.Pixels)
            {
                _data[pixel] = HIGHLIGHT_INTENSITY;
            }
            incorrectComponents++;
        }
    }

    cout << "Incorect components: " << incorrectComponents << endl;

    // Show bad wires
    for (Component badWire : badWires)
    {
        for (uint32 pixel : badWire.Pixels)
        {
            _data[pixel] = HIGHLIGHT_INTENSITY;
        }
    }

    updateHistogram();
}

void Image::Bottles(BottlesStage stage)
{
    int *labels = new int[getImageSize()];
    std::vector<Component> components;

    unsigned char *liquidData = new unsigned char[getImageSize()];
    CopyData(_data, liquidData, getImageSize());

    TresholdReverse(liquidData, 190);

    Treshold(liquidData, 20);
    CCL(liquidData, labels, components);

    RemoveSmallComponents(components, liquidData, 10);

    Erosion(liquidData, 3, 3);
    Dilation(liquidData, 3);

    CCL(liquidData, labels, components);
    
    std::vector<Component> liquids = components;
    std::vector<Component> holes;

    RemoveSmallComponents(liquids, liquidData, 50);
    FillHoles(liquidData, liquids, holes);
    
    CCL(liquidData, labels, liquids);

    cout << "liquids " << liquids.size()<<endl;

    if (stage == LiquidSegmentation)
    {
        liquids.shrink_to_fit();
        cout << "Liquid components: " << liquids.size() - 1 << endl;
        CopyData(liquidData, _data, getImageSize());
        updateHistogram();
        return;
    }

    Treshold(_data, 20);
    CCL(_data, labels, components);
    FillHoles(_data, components, holes);
    CCL(_data, labels, components);
    cout << "Bottles found: " << components.size() << endl;
    
    int liquidLimit = (BOTTLENECK_START + BOTTLENECK_END) / 2;
    for (Component bottle : components)
    {
        if (bottle.Label == 0)
            continue; // Skip background

        bool liquidFound = false;
        for (Component liquid : liquids)
        {
            //cout << "label" << liquid.Label << endl;
            if (liquid.Label == 0)
                continue; // Skip background

            if (ComponentsIntersect(bottle, liquid))
            {
                for (uint32 pixel : liquid.Pixels)
                {
                    _data[pixel] = HIGHLIGHT_INTENSITY;
                }

                // Check if liquid is filled
                int minX, minY, maxX, maxY, halfX;
                GetComponentBoundaries(liquid, minX, maxX, minY, maxY);
                if (minY > liquidLimit + LIQUID_ERROR_BOTTOM)
                {
                    // Bottle not filled
                    cout << "Bottle " << bottle.Label << " not filled" << endl;
                    for (uint32 pixel : liquid.Pixels)
                    {
                        _data[pixel] = 200;
                    }
                }
                else if (minY < liquidLimit - LIQUID_ERROR_TOP)
                {
                    // Bottle is overfilled
                    cout << "Bottle " << bottle.Label << " overfilled" << endl;
                    for (uint32 pixel : liquid.Pixels)
                    {
                        _data[pixel] = 50;
                    }
                }
                liquidFound = true;
                break;
            }
        }
        if (liquidFound == false)
        {
            cout << "Could not found liquid for bottle " << bottle.Label << endl;
        }
    }

    for (uint32 y = 0; y < _height; y++)
    {
        for (uint32 x = 0; x < _width; x++)
        {
            uint32 index = y * _width + x;

            if (y == liquidLimit + LIQUID_ERROR_BOTTOM || y == liquidLimit - LIQUID_ERROR_TOP)
            {
                _data[index] = HIGHLIGHT_INTENSITY;
            }

            //Bottle neck
            // if (y == 64 || y == 89)
            // {
            //     _data[index] = HIGHLIGHT_INTENSITY;
            // }
        }
    }

    updateHistogram();
}

void Image::RemoveWires(unsigned char *data)
{

    for (uint32 y = 1; y < _height; y++)
        for (uint32 x = 1; x < _width; x++)
        {
            if ((int)_data[y * _width + x] == MAX_INTENSITY)
            {
                if (((int)_data[(y - 1) * _width + x] == MIN_INTENSITY) && ((int)_data[(y + 1) * _width + x] == MIN_INTENSITY || (int)_data[(y + 2) * _width + x] == MIN_INTENSITY || (int)_data[(y + 3) * _width + x] == MIN_INTENSITY || (int)_data[(y + 4) * _width + x] == MIN_INTENSITY || (int)_data[(y + 5) * _width + x] == MIN_INTENSITY))
                {
                    uint16 step = 0;
                    while ((int)_data[(y + step) * _width + x] == MAX_INTENSITY)
                    {
                        _data[(y + step) * _width + x] = MIN_INTENSITY;
                        step++;
                    }
                }

                if (((int)_data[y * _width + (x - 1)] == MIN_INTENSITY) && ((int)_data[y * _width + (x + 1)] == MIN_INTENSITY || (int)_data[y * _width + (x + 2)] == MIN_INTENSITY || (int)_data[y * _width + (x + 3)] == MIN_INTENSITY || (int)_data[y * _width + (x + 4)] == MIN_INTENSITY || (int)_data[y * _width + (x + 5)] == MIN_INTENSITY))
                {
                    uint16 step = 0;
                    while ((int)_data[y * _width + (x + step)] != MIN_INTENSITY)
                    {
                        _data[y * _width + (x + step)] = MIN_INTENSITY;
                        step++;
                    }
                }
            }
        }
}

void Image::FillHoles(unsigned char *data, std::vector<Component> &components, std::vector<Component> &holes)
{
    for (Component component : components)
    {
        if (component.Label == 0)
            continue;
        if (component.Intensity == MIN_INTENSITY)
        {
            holes.push_back(component);
            for (uint32 pixel : component.Pixels)
            {
                data[pixel] = MAX_INTENSITY;
            }
        }
    }
}
