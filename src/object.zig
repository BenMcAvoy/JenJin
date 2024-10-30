const std = @import("std");
const log = std.log.scoped(.utils);

const zglfw = @import("zglfw");
const zmath = @import("zmath");

const gl = @import("zopengl").bindings;

const utils = @import("utils.zig");

const Shader = @import("shader.zig").Shader;

pub const Object = struct {
    position: @Vector(2, f32) = .{ 0.0, 0.0 },
    scale: @Vector(2, f32) = .{ 1.0, 1.0 },
    rotation: f32 = 0.0,

    colour: @Vector(3, f32) = .{ 1.0, 1.0, 1.0 },

    has_texture: bool = false,
    texture_id: c_uint = 0,

    shader: *Shader,

    cached_model: ?zmath.Mat = null,

    pub fn init(shader: *Shader) Object {
        return Object{
            .shader = shader,
        };
    }

    pub fn use(self: *Object) void {
        if (self.cached_model == null) {
            var model = zmath.identity();
            model = zmath.mul(zmath.translation(self.position[0], self.position[1], 0.0), model);
            const rotationRad = -self.rotation * std.math.pi / 180.0;
            model = zmath.mul(zmath.rotationZ(rotationRad), model);
            model = zmath.mul(zmath.scaling(self.scale[0], self.scale[1], 1.0), model);

            self.cached_model = model;
        }

        self.shader.setMat4("model", self.cached_model.?);
        self.shader.setVec3("colour", self.colour);
    }
};
