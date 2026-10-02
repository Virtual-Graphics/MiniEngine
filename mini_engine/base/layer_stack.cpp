#include "layer_stack.h"
#include "layer.h"


#include <algorithm>

namespace base {
LayerStack::~LayerStack() { detach_all(); }

void LayerStack::push_layer(std::unique_ptr<Layer> layer) {
    if (!layer) {
        return;
    }

    layer->on_attach();
    m_layers.emplace(m_layers.begin() + m_layer_insert_index, std::move(layer));
    m_layer_insert_index++;
}

void LayerStack::push_overlay(std::unique_ptr<Layer> overlay) {
    if (!overlay) {
        return;
    }

    overlay->on_attach();
    m_layers.emplace_back(std::move(overlay));
}

void LayerStack::pop_layer(Layer* layer) {
    auto it = std::find_if(m_layers.begin(), m_layers.begin() + m_layer_insert_index,
            [layer](const std::unique_ptr<Layer>& ptr) { return ptr.get() == layer; });
    if (it != m_layers.begin() + m_layer_insert_index) {
        (*it)->on_detach();
        m_layers.erase(it);
        m_layer_insert_index--;
    }
}

void LayerStack::pop_overlay(Layer* overlay) {
    auto it = std::find_if(m_layers.begin() + m_layer_insert_index, m_layers.end(),
            [overlay](const std::unique_ptr<Layer>& ptr) { return ptr.get() == overlay; });
    if (it != m_layers.end()) {
        (*it)->on_detach();
        m_layers.erase(it);
    }
}

void LayerStack::detach_all() {
    for (auto it = m_layers.rbegin(); it != m_layers.rend(); ++it) {
        if (*it) {
            (*it)->on_detach();
        }
    }

    m_layers.clear();
    m_layer_insert_index = 0;
}
} // namespace base
