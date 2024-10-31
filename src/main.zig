const std = @import("std");
const log = std.log.scoped(.main);

const zopengl = @import("zopengl");
const zglfw = @import("zglfw");

const Shader = @import("shader.zig").Shader;
const Buffers = @import("buffers.zig").Buffers;
const Object = @import("object.zig").Object;
const Camera = @import("camera.zig").Camera;

const utils = @import("utils.zig");
const gl = zopengl.bindings;

var camera = Camera.init(1024, 768);

fn resizeCallback(
    window: *zglfw.Window,
    width: i32,
    height: i32,
) callconv(.C) void {
    _ = window;
    gl.viewport(0, 0, width, height);
    camera.resize(width, height);
}

pub fn main() !void {
    var frame_count: u32 = 0;

    try zglfw.init();
    defer zglfw.terminate();

    const window = try utils.newWindow("Jenjin", 1024, 768);
    defer window.destroy();

    try zopengl.loadCoreProfile(zglfw.getProcAddress, 4, 6);

    const vx = @embedFile("shaders/vert.glsl");
    const fx = @embedFile("shaders/frag.glsl");
    var shader = try Shader.init(vx, fx);
    defer shader.deinit();
    camera.setShader(&shader);

    var buffers = Buffers.init();
    defer buffers.deinit();

    var player = Object.init(&shader);
    player.colour = .{ 1.0, 1.0, 1.0 };
    player.setTexture("player.png") catch unreachable;

    var asteroid = Object.init(&shader);
    asteroid.colour = .{ 0.3, 0.3, 0.3 };
    asteroid.position[0] = -2;
    asteroid.position[1] = -2;

    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    var objects = try std.ArrayList(*Object).initCapacity(allocator, 2);
    defer objects.deinit();

    try objects.append(&player);
    try objects.append(&asteroid);

    _ = window.setFramebufferSizeCallback(resizeCallback);
    while (!window.shouldClose() and window.getKey(.escape) != .press) {
        frame_count += 1;

        zglfw.pollEvents();

        gl.clearBufferfv(gl.COLOR, 0, &[_]f32{ 0.1, 0.1, 0.1, 1.0 });
        gl.clear(gl.COLOR);

        shader.use();
        buffers.use();
        camera.use();

        const speed = 0.1;
        const w = window.getKey(.w) == .press;
        const s = window.getKey(.s) == .press;
        const a = window.getKey(.a) == .press;
        const d = window.getKey(.d) == .press;
        if (w or s or a or d) {
            if (w) player.translate(0, speed);
            if (s) player.translate(0, -speed);
            if (a) player.translate(-speed, 0);
            if (d) player.translate(speed, 0);

            player.setRotation(if (w) 0 else if (s) 180 else if (a) 270 else if (d) 90 else 0);
        }

        for (objects.items) |obj| {
            obj.use();
            buffers.draw();
        }

        window.swapBuffers();
    }
}
