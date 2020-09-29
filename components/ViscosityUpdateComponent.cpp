//
// Created by Mateusz Święszek on 09/09/2020.
//

#include "ViscosityUpdateComponent.h"
#include <cmath>

ViscosityUpdateComponent::ViscosityUpdateComponent(Configurations &config) : config(config) {}

void ViscosityUpdateComponent::update(sycl::queue& queue, CellGrid& cells,
                                      sycl::buffer<Cell::Params, 1>& cellParamsBuf,
                                      sycl::buffer<OilPoint::Params, 1>& opParamsBuf,
                                      sycl::buffer<OilComponent, 2>& opCompBuf,
                                      int timestep) {
    queue.submit([&](sycl::handler& cgh){
       auto opParamsIO = opParamsBuf.get_access<sycl::access::mode::read_write>(cgh);

       auto emulsification = [](const OilPoint::Params& op) -> double{
           return (op.massOfEmulsion - op.mass) / op.massOfEmulsion;
       };

       double viscosityParameter = config.viscosityParameter;
       double mousseViscosity = config.mousseViscosity;
       cgh.parallel_for<class VUCUpdate>(sycl::range<1>(opParamsBuf.get_count()), [=](sycl::id<1> i){
           auto& op = opParamsIO[i];

           if(op.removed)
               return;

           double actualVis = op.viscosity;
           double deltaVis = viscosityParameter * actualVis * op.lastDeltaF +
                              2.5 * actualVis * op.lastDeltaY /
                                      ((1 + mousseViscosity * emulsification(op))*(1 + mousseViscosity * emulsification(op)));
           op.viscosity = actualVis + deltaVis;
       });
    });
}
