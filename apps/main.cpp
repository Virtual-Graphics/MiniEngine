#include "base/application.h"
#include "base/layer.h"

#include <memory>

#include <vulcao/context.h>
#include <vulcao/frame_manager.h>
#include <vulcao/rendering.h>

class ClearLayer : public base::Layer {
public:
    ClearLayer() : base::Layer("ClearLayer") {}

    void on_render() override {
        auto& window = application().get_window();
        auto& context = application().get_context();
        auto& frames = application().get_frame_manager();

        if (window.resized()) {
            window.clear_resized();
            recreate_swapchain(window, context, frames);
            return;
        }

        vulcao::Frame frame;
        try {
            frame = frames.begin_frame();
        } catch (const vk::OutOfDateKHRError&) {
            recreate_swapchain(window, context, frames);
            return;
        }

        vk::ClearColorValue clear;
        clear.float32[0] = 0.08f;
        clear.float32[1] = 0.09f;
        clear.float32[2] = 0.11f;
        clear.float32[3] = 1.0f;

        const vk::Image image = context.swapchain_images()[frame.image_index];
        const vk::ImageView view = context.swapchain_image_views()[frame.image_index];

        frame.command_buffer->transition_to_render(image)
            .begin_rendering(context.swapchain_extent(),
                             vulcao::color_attachment(view, vk::ImageLayout::eColorAttachmentOptimal, clear))
            .end_rendering()
            .transition_to_present(image);

        frames.end_frame(frame);

        if (!frames.present(frame)) {
            recreate_swapchain(window, context, frames);
        }
    }

private:
    static void recreate_swapchain(base::Window& window,
                                   vulcao::Context& context,
                                   vulcao::FrameManager& frames) {
        if (window.width() == 0 || window.height() == 0) {
            return;
        }

        context.wait_idle();

        vk::Extent2D extent;
        extent.width = window.width();
        extent.height = window.height();
        frames.recreate_swapchain(extent);
    }
};

int main() {
    base::ApplicationCreateInfo info;
    info.width = 1280;
    info.height = 720;
    info.title = "MiniEngine";

    base::Application app(info);
    app.push_layer(std::make_unique<ClearLayer>());
    app.run();
    return 0;
}
