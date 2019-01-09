# switch_biomod_demo

```
$biodynamo --version
v0.1.0-135-g39a8687
```

The desired behaviour would be a switch of the biomodule from S1Module() to S2Module().
The function to do that is 
```cpp
void MyCellExt::SetBioModule(int type)
```
in `MyCellExt()`
```cpp
// function to switch the biomodule of the cell
void SetBioModule(int type) {
    // Here I'd like to remove S1Module and add S2Module ... i.e. switch from cell_type_ 1 to cell_type_ 2
    if (type == 2) {
        const auto* s1mod = this->template GetBiologyModules<S1Module>()[0]; // <- this now compiles
        this->RemoveBiologyModule(s1mod); // <- this now compiles 
        this->AddBiologyModule(S2Module());
        std::cout << "switch to biomodule S2" << std::endl;
    }
}
```

The function should be called as soon as the intracellular substace quantity `counter_sub_quant_` falls below a given threshold.
So `S1Module()` calls `SetBioModule()`if this condition is fulfilled.
```cpp
// intracellular substance quantity is below threshold
// so now I'd like to switch the bio module from S1 to S2
if (currentCounterSubQuant < counter_sub_quant_threshold) {

    cell->SetBioModule(2);  // <- this results in runtime error wenn running the simulation

}
```

Running the simulation results in the following console output...

```
Run switch_biomod_demo 

...

Info: Initialize new simulation using BioDynaMo v0.1.0-135-g39a8687
Fatal in <Variant>: You called GetIf with a type that is not in Types...
aborting
[/Users/glue/.bdm/third_party/root/lib/libCore.so] ErrorHandler (no debug info)
[/Users/glue/.bdm/third_party/root/lib/libCore.so] Fatal(char const*, char const*, ...) (no debug info)
[/Users/glue/work/BioDynaMo/switch_biomod_demo/./build/switch_biomod_demo] std::__1::enable_if<(GetIndex<bdm::S1Module const, bdm::S1Module, bdm::S2Module>()) == (-(1)), bdm::S1Module const*>::type bdm::Variant<bdm::S1Module, bdm::S2Module>::GetIf<bdm::S1Module const>() (no debug info)
[/Users/glue/work/BioDynaMo/switch_biomod_demo/./build/switch_biomod_demo] void bdm::SimulationObjectExt<bdm::CompileTimeParam<bdm::SoaRef>, bdm::Capsule<bdm::MyCellExt> >::RemoveBiologyModule<bdm::S1Module const>(bdm::S1Module const*) (no debug info)
[/Users/glue/work/BioDynaMo/switch_biomod_demo/./build/switch_biomod_demo] bdm::MyCellExt<bdm::CompileTimeParam<bdm::SoaRef>, bdm::Capsule<bdm::MyCellExt> >::SetBioModule() (no debug info)
[/Users/glue/work/BioDynaMo/switch_biomod_demo/./build/switch_biomod_demo] .omp_outlined..231 (no debug info)
[/usr/local/opt/llvm/lib/libomp.dylib] __kmp_invoke_microtask (no debug info)
[/usr/local/opt/llvm/lib/libomp.dylib] __kmp_invoke_task_func (no debug info)
[/usr/local/opt/llvm/lib/libomp.dylib] __kmp_fork_call (no debug info)
[/usr/local/opt/llvm/lib/libomp.dylib] __kmpc_fork_call (no debug info)
[/Users/glue/work/BioDynaMo/switch_biomod_demo/./build/switch_biomod_demo] void bdm::OpTimer<bdm::BiologyModuleOp>::operator()<bdm::MyCellExt<bdm::CompileTimeParam<bdm::Soa>, bdm::Capsule<bdm::MyCellExt> >, bdm::Simulation<bdm::CompileTimeParam<bdm::Soa> > >(bdm::MyCellExt<bdm::CompileTimeParam<bdm::Soa>, bdm::Capsule<bdm::MyCellExt> >*, unsigned short) (no debug info)
[/Users/glue/work/BioDynaMo/switch_biomod_demo/./build/switch_biomod_demo] bdm::Scheduler<bdm::Simulation<bdm::CompileTimeParam<bdm::Soa> > >::Execute(bool) (no debug info)
[/Users/glue/work/BioDynaMo/switch_biomod_demo/./build/switch_biomod_demo] bdm::Simulate(int, char const**) (no debug info)
[/usr/lib/system/libdyld.dylib] start (no debug info)
[<unknown binary>] (no debug info)

```