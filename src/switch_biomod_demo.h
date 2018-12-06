// -----------------------------------------------------------------------------
//
// Copyright (C) The BioDynaMo Project.
// All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
//
// See the LICENSE file distributed with this work for details.
// See the NOTICE file distributed with this work for additional information
// regarding copyright ownership.
//
// -----------------------------------------------------------------------------


#ifndef SWITCH_BIOMOD_DEMO_H_
#define SWITCH_BIOMOD_DEMO_H_

#include "biodynamo.h"

// set numer of simulation steps
const int simulation_steps = 50; // Time between two simulation steps equals: 0.01hours (default)

// set amount of intracellular_substance_quantity for precursor cells
const double intracellular_substance_quantity = 100;
const double counter_sub_quant_threshold = 75; // threshold of intracellular substance ... when to switch biomodules
const double default_cell_diameter = 6; // default diameter of new cells


namespace bdm {

// 1. Define S1 behaviour
    struct S1Module : public BaseBiologyModule {
        S1Module() : BaseBiologyModule(gAllEventIds) {}

        /// Empty default event constructor, because GrowthModule does not have state.
        template <typename TEvent, typename TBm>
        S1Module(const TEvent& event, TBm* other, uint64_t new_oid = 0)
                : BaseBiologyModule(event, other, new_oid) {}

        // run single simulation step
        template <typename T, typename TSimulation = Simulation<>>
        void Run(T* cell) {

            // change counter_sub_quant_ (intracellular substance quantity) ... reduce by 1% per timestep
            double currentCounterSubQuant = cell->GetCounterSubQuant();
            currentCounterSubQuant = currentCounterSubQuant * (0.99);
            cell->SetCounterSubQuant(currentCounterSubQuant);

            // cell differentiation
            runCellCycleDiffStep(cell); // grow cell

            // intracellular substance quantity is below threshold
            // so now I'd like to switch the bio module from S1 to S2
            if (currentCounterSubQuant < counter_sub_quant_threshold) {

                //  cell->SetBioModule(2);  // uncomment this line to call the module switch function

            }
        }

        // run cell differentiation step
        template <typename T, typename TSimulation = Simulation<>>
        void runCellCycleDiffStep(T* cell){
            if (cell->GetDiameter() < cell->GetMaxDiam()) { // max_diam not reached
                cell->SetDiameter(cell->GetDiameter() + 0.2); // grow cell diameter by adding some number
            } else { // max diam reached -> divide cell
                auto &&daughter = cell->Divide();
                daughter->SetDiameter(default_cell_diameter);  // init daughter with default diameter
            }
        }

    private:
        BDM_CLASS_DEF_NV(S1Module, 1);
    };

// Define S2 behaviour ... yet doesn't do anything
    struct S2Module : public BaseBiologyModule {
        S2Module() : BaseBiologyModule(gAllEventIds) {}

        /// Empty default event constructor, because GrowthModule does not have state.
        template <typename TEvent, typename TBm>
        S2Module(const TEvent& event, TBm* other, uint64_t new_oid = 0)
                : BaseBiologyModule(event, other, new_oid) {}

        // run single simulation step
        template <typename T, typename TSimulation = Simulation<>>
        void Run(T* cell) {

        }

    private:
        BDM_CLASS_DEF_NV(S2Module, 1);
    };


// Define new cell (BDM_SIM_OBJECT) which extends Cell by adding extra data
BDM_SIM_OBJECT(MyCell, Cell) {  // our object extends the Cell object
// create the header with our new data member
BDM_SIM_OBJECT_HEADER(MyCell, Cell, 1, cell_type_, counter_sub_quant_, max_diam_);

public:
    MyCellExt() {}
    explicit MyCellExt(const std::array<double, 3>& position) : Base(position) {
        // init cell with type 1 and max_diam = 10
        cell_type_[kIdx] = 1;
        max_diam_[kIdx] = 10;
    }

    /// If MyCell divides, daughter 2 copies the data members from the mother
    template <typename TMother>
    MyCellExt(const CellDivisionEvent& event, TMother* mother)
            : Base(event, mother) {
        cell_type_[kIdx] = mother->cell_type_[mother->kIdx];
        counter_sub_quant_[kIdx] = mother->counter_sub_quant_[mother->kIdx]; // intracellular substance quantity
        max_diam_[kIdx] = mother->max_diam_[mother->kIdx];
    }

    /// If a cell divides, daughter keeps the same state from its mother.
    template <typename TDaughter>
    void EventHandler(const CellDivisionEvent& event, TDaughter* daughter) {
      Base::EventHandler(event, daughter);
    }

    // getter and setter for our new data member
    void SetCellType(int cell_type) { cell_type_[kIdx] = cell_type; }
    int GetCellType() const { return cell_type_[kIdx]; }

    void SetCounterSubQuant(double counter_sub_quant) { counter_sub_quant_[kIdx] = counter_sub_quant; }
    double GetCounterSubQuant() const { return counter_sub_quant_[kIdx]; }

    void SetMaxDiam(double max_diam) { max_diam_[kIdx] = max_diam; }
    double GetMaxDiam() const { return max_diam_[kIdx]; }

    // function to switch the biomodule of the cell
    void SetBioModule(int type) {
        // Here I'd like to remove S1Module and add S2Module ... i.e. switch from cell_type_ 1 to cell_type_ 2
        if (type == 2) {
            const auto* s1mod = this.GetBiologyModules<S1Module>()[0]; // <- doesn't work
            this->RemoveBiologyModule(s1mod); // <- doesn't work
            this->AddBiologyModule(S2Module());
            std::cout << "switch to biomodule S2" << std::endl;
        }
    }

private:
    // declare new data member and define their type
    // private data can only be accessed by public function and not directly
    vec<int> cell_type_;
    vec<double> counter_sub_quant_;  // intracellular substance quantity
    vec<double> max_diam_;
};

// Define compile time parameter
BDM_CTPARAM() {
  BDM_CTPARAM_HEADER();
  using SimObjectTypes = CTList<MyCell>;  // use MyCell object
  // Override default BiologyModules for Cell
  BDM_CTPARAM_FOR(bdm, MyCell) { using BiologyModules = CTList<S1Module, S2Module>; };

};

inline int Simulate(int argc, const char** argv) {
    // set parameters of the simulation
    auto set_param = [](auto* param) {
        param->bound_space_ = true;
        param->min_bound_ = -50;
        param->max_bound_ = 50;  // plate of 100*100
        param->run_mechanical_interactions_ = true;
    };

    Simulation<> simulation(argc, argv, set_param);

    // Define initial model - in this example: single cell at origin
    auto* rm = simulation.GetResourceManager();

    // create a structure to contain cells
    auto* cells = rm->template Get<MyCell>();
    // allocate the correct number of cell in our cells structure before
    cells->reserve(1);

    MyCell cell({0, 0, 10});
    cell.SetDiameter(default_cell_diameter);
    cell.AddBiologyModule(S1Module());
    cells->push_back(cell);  // put the created cell in cells structure
    cells->Commit();  // commit cells

    // Run simulation
    simulation.GetScheduler()->Simulate(simulation_steps);
    std::cout << "Simulation completed successfully!" << std::endl;
    return 0;
}

}  // namespace bdm

#endif  // SWITCH_BIOMOD_DEMO_H_
