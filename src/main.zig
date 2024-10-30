const std = @import("std");
const log = std.log.scoped(.main);

const zopengl = @import("zopengl");
const zglfw = @import("zglfw");

const Shader = @import("shader.zig").Shader;

const utils = @import("utils.zig");
const gl = zopengl.bindings;

pub fn main() !void {
    try zglfw.init();
    defer zglfw.terminate();

    const window = try utils.newWindow("Jenjin", 1024, 768);
    defer window.destroy();

    try zopengl.loadCoreProfile(zglfw.getProcAddress, 4, 6);

    const vx = @embedFile("shaders/vert.glsl");
    const fx = @embedFile("shaders/frag.glsl");
    const shader = try Shader.init(vx, fx);
    defer shader.deinit();

    while (!window.shouldClose() and window.getKey(.escape) != .press) {
        zglfw.pollEvents();

        gl.clearBufferfv(gl.COLOR, 0, &[_]f32{ 0.1, 0.1, 0.1, 1.0 });
        gl.clear(gl.COLOR);

        shader.use();

        window.swapBuffers();
    }
}
