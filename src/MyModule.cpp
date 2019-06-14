#include "plugin.hpp"
#include "stdio.h"


struct MyModule : Module {
    enum ParamId {
        PITCH_PARAM,
        NUM_PARAMS
    };
    enum InputId {
        TRIGGER_INPUT,
        NUM_INPUTS
    };
    enum OutputId {
        PLUCK_OUTPUT,
        NUM_OUTPUTS
    };

    int samples = 1000;
    float noise [1000];
    int noisePointer = 0;
    float previousOutput = 0;


    MyModule() {
        // Configure the module
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);

        // Configure parameters
        // See engine/Param.hpp for config() arguments
        configParam(PITCH_PARAM, -3.f, 3.f, 0.f, "Pitch", " Hz", 2.f, dsp::FREQ_C4);

        regenerateTable();
    }

    void regenerateTable()
    {
        for (int i = 0; i < samples; i++)
        {
            noise[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/5.0));
        }
    }

    void onReset() override {
        regenerateTable();
    }

    void process(const ProcessArgs &args) override {
        float x = inputs[TRIGGER_INPUT].value;

        dsp::SchmittTrigger schmittTrigger;
        schmittTrigger.process(rescale(x, 0.1f, 10.f, 0.f, 1.f));
        bool triggered = schmittTrigger.isHigh();

        if (triggered == 1)
        {
            regenerateTable();
        }

        noise[noisePointer] = 0.5 * (noise[noisePointer] + previousOutput);

        outputs[PLUCK_OUTPUT].setVoltage(noise[noisePointer]);

        if (noisePointer == 0)
        {
            previousOutput = noise[samples - 1];
        }
        else
        {
            previousOutput = noise[noisePointer - 1];
        }

        noisePointer++;

        if (noisePointer >= samples)
            noisePointer = 0;
    }
};


struct MyModuleWidget : ModuleWidget {
    MyModuleWidget(MyModule *module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/panel.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addParam(createParam<Davies1900hBlackKnob>(Vec(25, 87), module, MyModule::PITCH_PARAM));

        addInput(createInput<PJ301MPort>(Vec(25, 186), module, MyModule::TRIGGER_INPUT));

        addOutput(createOutput<PJ301MPort>(Vec(25, 275), module, MyModule::PLUCK_OUTPUT));
    }
};


// Define the Model with the Module type, ModuleWidget type, and module slug
Model *modelMyModule = createModel<MyModule, MyModuleWidget>("cyc-strong_kar");