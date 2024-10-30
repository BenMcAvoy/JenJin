const std = @import("std");
const log = std.log.scoped(.camera);

const zglfw = @import("zglfw");
const zmath = @import("zmath");

const gl = @import("zopengl").bindings;

const Shader = @import("shader.zig").Shader;

pub const Camera = struct {
    position: zmath.Vec = zmath.f32x4(0.0, 0.0, 0.0, 0.0),
    rotation: f32 = 0.0,
    zoom: f32 = 1.0,

    projection: zmath.Mat = zmath.identity(),
    view: zmath.Mat = zmath.identity(),

    width: f32 = 0,
    height: f32 = 0,

    shader: ?*Shader = null,

    pub fn init(width: f32, height: f32) Camera {
        return Camera{
            .width = width,
            .height = height,
        };
    }

    pub fn setShader(self: *Camera, shader: *Shader) void {
        self.shader = shader;
    }

    pub fn resize(self: *Camera, width: i32, height: i32) void {
        self.width = @floatFromInt(width);
        self.height = @floatFromInt(height);
    }

    pub fn update(self: *Camera) void {
        const aspect = self.width / self.height;
        self.projection = zmath.orthographicRh(aspect / self.zoom * 4, 1.0 / self.zoom * 4, -1.0, 100.0);
        self.view = zmath.lookAtRh(self.position, (self.position + zmath.f32x4(0.0, 0.0, -1.0, 0.0)), zmath.f32x4(0.0, 1.0, 0.0, 0.0));
    }

    pub fn use(self: *Camera) void {
        self.update();
        self.shader.?.setMat4("projection", self.projection);
        self.shader.?.setMat4("view", self.view);
    }
};
