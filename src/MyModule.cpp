#include "plugin.hpp"
#include "stdio.h"


struct MyModule : Module {
    enum ParamId {
        PITCH_PARAM,
        NUM_PARAMS
    };
    enum InputId {
        PITCH_INPUT,
        NUM_INPUTS
    };
    enum OutputId {
        SINE_OUTPUT,
        NUM_OUTPUTS
    };
    enum LightId {
        BLINK_LIGHT,
        NUM_LIGHTS
    };

    int samples = 1000;
    float phase = 0.f;
    float blinkPhase = 0.f;
    float noise [1000];
    int noisePointer = 0;
    float previousOutput = 0;


    MyModule() {
        // Configure the module
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        // Configure parameters
        // See engine/Param.hpp for config() arguments
        configParam(PITCH_PARAM, -3.f, 3.f, 0.f, "Pitch", " Hz", 2.f, dsp::FREQ_C4);

        for (int i = 0; i < samples; i++)
        {
            noise[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/5.0));
        }
    }
// Python definition of the karplus strong algorithm for reference
//    def karplus_strong(wavetable, n_samples):
//        """Synthesizes a new waveform from an existing wavetable, modifies last sample by averaging."""
//        samples = []
//        current_sample = 0
//        previous_value = 0
//        while len(samples) < n_samples:
//                wavetable[current_sample] = 0.5 * (wavetable[current_sample] + previous_value)
//                samples.append(wavetable[current_sample])
//                previous_value = samples[-1]
//                current_sample += 1
//                current_sample = current_sample % wavetable.size

    void onReset() override {
        for (int i = 0; i < samples; i++)
        {
            noise[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/5.0));
        }
    }

    void process(const ProcessArgs &args) override {
        noise[noisePointer] = 0.5 * (noise[noisePointer] + previousOutput);

        outputs[SINE_OUTPUT].setVoltage(noise[noisePointer]);

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

    void deprecatedProcess(const ProcessArgs &args) {
        // Implement a simple sine oscillator

        // Compute the frequency from the pitch parameter and input
        float pitch = params[PITCH_PARAM].getValue();
        pitch += inputs[PITCH_INPUT].getVoltage();
        pitch = clamp(pitch, -4.f, 4.f);
        // The default pitch is C4 = 261.6256f
        float freq = dsp::FREQ_C4 * std::pow(2.f, pitch);

        // Accumulate the phase
        phase += freq * args.sampleTime;
        if (phase >= 0.5f)
            phase -= 1.f;

        // Compute the sine output
        float sine = std::sin(2.f * M_PI * phase);
        // Audio signals are typically +/-5V
        // https://vcvrack.com/manual/VoltageStandards.html
        outputs[SINE_OUTPUT].setVoltage(5.f * sine);

        // Blink light at 1Hz
        blinkPhase += args.sampleTime;
        if (blinkPhase >= 1.f)
            blinkPhase -= 1.f;
        lights[BLINK_LIGHT].setBrightness(blinkPhase < 0.5f ? 1.f : 0.f);
    }

    // For more advanced Module features, see engine/Module.hpp in the Rack API.
    // - dataToJson, dataFromJson: serialization of internal data
    // - onSampleRateChange: event triggered by a change of sample rate
    // - onReset, onRandomize: implements custom behavior requested by the user
};


struct MyModuleWidget : ModuleWidget {
    MyModuleWidget(MyModule *module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/MyModule.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addParam(createParam<Davies1900hBlackKnob>(Vec(28, 87), module, MyModule::PITCH_PARAM));

        addInput(createInput<PJ301MPort>(Vec(33, 186), module, MyModule::PITCH_INPUT));

        addOutput(createOutput<PJ301MPort>(Vec(33, 275), module, MyModule::SINE_OUTPUT));

        addChild(createLight<MediumLight<RedLight>>(Vec(41, 59), module, MyModule::BLINK_LIGHT));
    }
};


// Define the Model with the Module type, ModuleWidget type, and module slug
Model *modelMyModule = createModel<MyModule, MyModuleWidget>("cyc-strong_kar");