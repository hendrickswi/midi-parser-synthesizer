#include "CompositeOscillator.h"
#include "CompositeOscillatorNode.h"

CompositeOscillator::CompositeOscillator() {
    children = std::array<CompositeOscillatorNode, MAX_CHILDREN_IN_COMPOSITE>();
    num_children = 0;
    child_buffer = std::array<float, 4096>();
}

CompositeOscillator::CompositeOscillator(float hz, float sample_rate) {
    children = std::array<CompositeOscillatorNode, MAX_CHILDREN_IN_COMPOSITE>();
    num_children = 0;
    child_buffer = std::array<float, 4096>();
}

void CompositeOscillator::configure(const CompositeOscillatorParams& params, float base_hz, float sample_rate) {
    for (int i = 0; i < params.num_children; i++) {
        auto& child = children[i];
        child.mix_volume = params.mix_volumes[i];
        child.frequency_ratio = params.child_frequency_ratios[i];

        switch (params.child_types[i]) {
            case OscillatorType::SAWTOOTH : {
                child.oscillator = SawtoothOscillator();
                std::get<SawtoothOscillator>(child.oscillator).set_frequency(base_hz * child.frequency_ratio, sample_rate);
                break;
            }
            case OscillatorType::SQUARE : {
                child.oscillator = SquareOscillator();
                std::get<SquareOscillator>(child.oscillator).set_frequency(base_hz * child.frequency_ratio, sample_rate);
                break;
            }
            case OscillatorType::SINE : {
                child.oscillator = SineOscillator();
                std::get<SineOscillator>(child.oscillator).set_frequency(base_hz * child.frequency_ratio, sample_rate);
                break;
            }
            case OscillatorType::TRIANGLE : {
                child.oscillator = TriangleOscillator();
                std::get<TriangleOscillator>(child.oscillator).set_frequency(base_hz * child.frequency_ratio, sample_rate);
                break;
            }
            case OscillatorType::NOISE : {
                child.oscillator = NoiseOscillator();
                std::get<NoiseOscillator>(child.oscillator).set_frequency(base_hz * child.frequency_ratio, sample_rate);
                break;
            }
        }
    }
    num_children = params.num_children;
    set_frequency(base_hz, sample_rate);
}

void CompositeOscillator::process_sample_block(float* buffer, unsigned int num_frames, const float* fm_buffer) {
    std::fill(buffer, buffer + num_frames, 0.0f);

    for (int i = 0; i < num_children; i++) {
        auto& child = children[i];
        std::visit([&](auto& oscillator) -> void {
                oscillator.process_sample_block(child_buffer.data(), num_frames, fm_buffer);
            }, child.oscillator);

        for (unsigned int j = 0; j < num_frames; j++) {
            buffer[j] += child_buffer[j] * child.mix_volume;
        }
    }
}

void CompositeOscillator::set_modulation_depth(float depth) {
    for (int i = 0; i < num_children; i++) {
        auto& child = children[i];
        std::visit([&](auto& oscillator) -> void {
                oscillator.set_modulation_depth(depth);
            }, child.oscillator);
    }
}

void CompositeOscillator::set_frequency(float hz, float sample_rate) {
    for (int i = 0; i < num_children; i++) {
        auto& child = children[i];
        std::visit([&](auto& oscillator) -> void {
                oscillator.set_frequency(hz * child.frequency_ratio, sample_rate);
        }, child.oscillator);
    }
}