#include "CompositeOscillator.h"
#include "CompositeOscillatorNode.h"

CompositeOscillator::CompositeOscillator() {
    children = std::vector<CompositeOscillatorNode>();
    current_hz = 440.0f;
    sample_rate = 44100.0f;
    child_buffer = std::vector<float>(4096); // Safe estimate to prevent resizing
}

CompositeOscillator::CompositeOscillator(float hz, float sample_rate) {
    children = std::vector<CompositeOscillatorNode>();
    current_hz = hz;
    this->sample_rate = sample_rate;
}

void CompositeOscillator::add_oscillator(std::unique_ptr<Oscillator> oscillator, float mix_volume, float frequency_ratio) {
    CompositeOscillatorNode node = CompositeOscillatorNode(std::move(oscillator), mix_volume, frequency_ratio);
    children.push_back(std::move(node));
}


void CompositeOscillator::process_sample_block(float* buffer, unsigned int num_frames, const float* fm_buffer) {
    for (auto& child : children) {
        child.oscillator->process_sample_block(child_buffer.data(), num_frames, fm_buffer);
        for (unsigned int i = 0; i < num_frames; i++) {
            buffer[i] += child_buffer[i] * child.mix_volume;
        }
    }
}

void CompositeOscillator::set_modulation_depth(float depth) {
    for (auto& child : children) {
        child.oscillator->set_modulation_depth(depth);
    }
}

void CompositeOscillator::set_frequency(float hz, float sample_rate) {
    current_hz = hz;
    this->sample_rate = sample_rate;
    for (auto& child : children) {
        child.oscillator->set_frequency(hz * child.frequency_ratio, sample_rate);
    }
}