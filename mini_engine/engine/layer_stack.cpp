#include "layer_stack.h"
#include "layer.h"


#include <algorithm>

namespace engine {
LayerStack::~LayerStack() { detach_all(); }

void LayerStack::push_layer(std::unique_ptr<Layer> layer) {
    if (!layer) {
        return;
    }

    layer->on_attach();
    layers_.emplace(layers_.begin() + layer_insert_index_, std::move(layer));
    layer_insert_index_++;
}

void LayerStack::push_overlay(std::unique_ptr<Layer> overlay) {
    if (!overlay) {
        return;
    }

    overlay->on_attach();
    layers_.emplace_back(std::move(overlay));
}

void LayerStack::pop_layer(Layer* layer) {
    auto it = std::find_if(layers_.begin(), layers_.begin() + layer_insert_index_,
            [layer](const std::unique_ptr<Layer>& ptr) { return ptr.get() == layer; });
    if (it != layers_.begin() + layer_insert_index_) {
        (*it)->on_detach();
        layers_.erase(it);
        layer_insert_index_--;
    }
}

void LayerStack::pop_overlay(Layer* overlay) {
    auto it = std::find_if(layers_.begin() + layer_insert_index_, layers_.end(),
            [overlay](const std::unique_ptr<Layer>& ptr) { return ptr.get() == overlay; });
    if (it != layers_.end()) {
        (*it)->on_detach();
        layers_.erase(it);
    }
}

void LayerStack::detach_all() {
    for (auto it = layers_.rbegin(); it != layers_.rend(); ++it) {
        if (*it) {
            (*it)->on_detach();
        }
    }

    layers_.clear();
    layer_insert_index_ = 0;
}
} // namespace engine
