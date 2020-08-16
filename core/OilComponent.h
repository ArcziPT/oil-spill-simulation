#ifndef OIL_COMPONENT_H
#define OIL_COMPONENT_H

class OilComponent
{

public:
    OilComponent(double x, double molecularWeigth, double tb) : x(x), molecularWeigth(molecularWeigth), tb(tb) {}

    double getX();
    void setX(double x);

    double getMolecularWeight();
    void setMolecularWeight(double molecularWeight);

    double getTb();
    void setTb(double tb);

private:
    double x;               // ulamek masowy
    double molecularWeigth; // masa molowa kg/moll
    double tb;              // temperatura wrzenia w K
};

#endif