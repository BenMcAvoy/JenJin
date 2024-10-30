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

    var buffers = try Buffers.init();
    defer buffers.deinit();

    var object_1 = Object.init(&shader);
    object_1.colour = .{ 0.0, 1.0, 0.0 };
    var object_2 = Object.init(&shader);
    object_2.colour = .{ 1.0, 0.0, 0.0 };
    object_2.scale = .{ 2.5, 0.5 };

    // const objects: [2]*Object = .{ &object_1, &object_2 };
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    var objects = try std.ArrayList(*Object).initCapacity(allocator, 2);
    defer objects.deinit();

    try objects.append(&object_1);
    try objects.append(&object_2);

    _ = window.setFramebufferSizeCallback(resizeCallback);
    // gl.viewport(0, 0, size[0], size[1]);
    while (!window.shouldClose() and window.getKey(.escape) != .press) {
        frame_count += 1;

        zglfw.pollEvents();

        gl.clearBufferfv(gl.COLOR, 0, &[_]f32{ 0.1, 0.1, 0.1, 1.0 });
        gl.clear(gl.COLOR);

        shader.use();
        buffers.use();
        camera.use();

        // Shenanigans to test object creation/modification
        if (frame_count == 16 * 2) {
            var obj = Object.init(&shader);
            obj.colour = .{ 1.0, 0.0, 1.0 };
            obj.scale = .{ 2.5, 2.5 };
            obj.position = .{
                3.0,
                0.0,
            };
            try objects.append(&obj);
        } else if (frame_count == 16 * 4) {
            var obj = Object.init(&shader);
            obj.colour = .{ 0.0, 1.0, 1.0 };
            obj.scale = .{ 2.5, 2.5 };
            obj.position = .{
                -3.0,
                0.0,
            };
            try objects.append(&obj);
        } else if (frame_count >= 16 * 6 and frame_count < 16 * 8) {
            for (objects.items) |obj| {
                obj.rotation += 22.5;
                obj.cached_model = null;
            }
        }

        for (objects.items) |obj| {
            obj.use();
            buffers.draw();
        }

        window.swapBuffers();
    }
}
