# switch_biomod_demo

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
        const auto* s1mod = this.GetBiologyModules<S1Module>()[0]; // <- doesn't work
        this->RemoveBiologyModule(s1mod); // <- doesn't work
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

    //  cell->SetBioModule(2);  // uncomment this line to call the module switch function

}
```
