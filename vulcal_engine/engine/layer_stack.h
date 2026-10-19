#pragma once
#include <cstddef>

#include <memory>
#include <vector>

namespace engine {
class Layer;

class LayerStack {
public:
    LayerStack() = default;
    ~LayerStack();

    void push_layer(std::unique_ptr<Layer> layer);
    void push_overlay(std::unique_ptr<Layer> overlay);
    void pop_layer(Layer* layer);
    void pop_overlay(Layer* overlay);
    void detach_all();

    std::vector<std::unique_ptr<Layer>>::iterator begin() { return layers_.begin(); }

    std::vector<std::unique_ptr<Layer>>::iterator end() { return layers_.end(); }

    std::vector<std::unique_ptr<Layer>>::reverse_iterator rbegin() { return layers_.rbegin(); }

    std::vector<std::unique_ptr<Layer>>::reverse_iterator rend() { return layers_.rend(); }

    std::vector<std::unique_ptr<Layer>>::const_iterator cbegin() const { return layers_.begin(); }

    std::vector<std::unique_ptr<Layer>>::const_iterator cend() const { return layers_.end(); }

    std::vector<std::unique_ptr<Layer>>::const_reverse_iterator crbegin() const {
        return layers_.rbegin();
    }

    std::vector<std::unique_ptr<Layer>>::const_reverse_iterator crend() const {
        return layers_.rend();
    }

private:
    std::vector<std::unique_ptr<Layer>> layers_;
    size_t layer_insert_index_{ 0 };
};
} // namespace engine
