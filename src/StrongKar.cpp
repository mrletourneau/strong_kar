#include "plugin.hpp"
#include "stdio.h"


struct StrongKar : Module {
    enum ParamId {
        DAMP_PARAM,
        FREQ_PARAM,
        NUM_PARAMS
    };
    enum InputId {
        TRIGGER_INPUT,
        PITCH_INPUT,
        NUM_INPUTS
    };
    enum OutputId {
        PLUCK_OUTPUT,
        NUM_OUTPUTS
    };

    int samplesPeriod = 0;
    float* noise = NULL;
    int noisePointer = 0;
    float previousOutput = 0;


    StrongKar() {
        // Configure the module
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);

        // Configure parameters
        // See engine/Param.hpp for config() arguments
        configParam(DAMP_PARAM, .01f, .5f, 0.5f, "Damp", " Hz", 2.f, dsp::FREQ_C4);
        configParam(FREQ_PARAM, -3.f, 3.f, 0.f, "Pitch", " Hz", 2.f, dsp::FREQ_C4);

        noise = new float[1];
        noise[0] = 0;
    }

    void regenerateTable(int frequency, float sampleRate)
    {
        noisePointer = 0;
        samplesPeriod = sampleRate / frequency;

        delete [] noise;
        noise = new float[samplesPeriod];

        for (int i = 0; i < samplesPeriod; i++)
        {
            noise[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/5.0));
        }
    }

    void onReset() override {
        delete [] noise;
        noise = new float[1];
        noise[0] = 0;
    }

    void process(const ProcessArgs &args) override {
        float triggerVoltage = inputs[TRIGGER_INPUT].value;

        dsp::SchmittTrigger schmittTrigger;
        schmittTrigger.process(rescale(triggerVoltage, 0.1f, 10.f, 0.f, 1.f));
        bool triggered = schmittTrigger.isHigh();

        float pitch = params[FREQ_PARAM].getValue();
        pitch += inputs[PITCH_INPUT].getVoltage();
        pitch = clamp(pitch, -4.f, 4.f);
        // The default pitch is C4 = 261.6256f
        float freq = dsp::FREQ_C4 * std::pow(2.f, pitch);

        if (triggered == 1)
        {
            regenerateTable(freq, args.sampleRate);
        }


        float dampAmount = params[DAMP_PARAM].getValue();


        noise[noisePointer] = dampAmount * (noise[noisePointer] + previousOutput);

        outputs[PLUCK_OUTPUT].setVoltage(noise[noisePointer]);

        if (noisePointer == 0)
        {
            previousOutput = noise[samplesPeriod - 1];
        }
        else
        {
            previousOutput = noise[noisePointer - 1];
        }

        noisePointer++;

        if (noisePointer >= samplesPeriod)
            noisePointer = 0;
    }
};


struct StrongKarWidget : ModuleWidget {
    StrongKarWidget(StrongKar *module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/panel.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addParam(createParam<Davies1900hBlackKnob>(mm2px(Vec(5.659-1.2, 128.5-95.647-9.525-1.2)), module, StrongKar::DAMP_PARAM));
        addParam(createParam<Davies1900hBlackKnob>(mm2px(Vec(5.659-1.2, 128.5-77.655-9.525-1.2)), module, StrongKar::FREQ_PARAM));

        addInput(createInput<PJ301MPort>(mm2px(Vec(6.086, 128.5-59.454-8.356)), module, StrongKar::TRIGGER_INPUT));
        addInput(createInput<PJ301MPort>(mm2px(Vec(6.086, 128.5-45.167-8.356)), module, StrongKar::PITCH_INPUT));

        addOutput(createOutput<PJ301MPort>(mm2px(Vec(6.086, 128.5-30.086-8.356)), module, StrongKar::PLUCK_OUTPUT));
    }
};


// Define the Model with the Module type, ModuleWidget type, and module slug
Model *modelMyModule = createModel<StrongKar, StrongKarWidget>("cyc-strong_kar");