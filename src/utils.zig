const std = @import("std");
const log = std.log.scoped(.utils);

const zglfw = @import("zglfw");

pub fn newWindow(title: [:0]const u8, width: i32, height: i32) !*zglfw.Window {
    log.info("Creating window {s} ({d}x{d})", .{ title, width, height });

    // zglfw.setErrorCallback(zglfw.ErrorCallback.print);

    zglfw.windowHintTyped(.context_version_major, 4);
    zglfw.windowHintTyped(.context_version_minor, 6);
    zglfw.windowHintTyped(.opengl_profile, .opengl_core_profile);
    zglfw.windowHintTyped(.client_api, .opengl_api);
    zglfw.windowHintTyped(.doublebuffer, true);

    if (@import("builtin").os.tag == .macos)
        zglfw.windowHintTyped(.opengl_forward_compat, true);

    const window = try zglfw.Window.create(width, height, title, null);

    zglfw.makeContextCurrent(window);
    zglfw.swapInterval(1); // VSYNC

    return window;
}
