const std = @import("std");
const log = std.log.scoped(.utils);

const zglfw = @import("zglfw");
const zmath = @import("zmath");

const gl = @import("zopengl").bindings;

const utils = @import("utils.zig");

const Texture = @import("texture.zig").Texture;
const Shader = @import("shader.zig").Shader;

// TODO: Is pulling the cached model out of cold memory better than just
// calculating it every frame?
pub const Object = struct {
    position: @Vector(2, f32) = .{ 0.0, 0.0 },
    scale: @Vector(2, f32) = .{ 1.0, 1.0 },
    rotation: f32 = 0.0,

    colour: @Vector(3, f32) = .{ 1.0, 1.0, 1.0 },

    texture: ?Texture = null,

    shader: *Shader,

    cached_model: ?zmath.Mat = null,

    pub fn init(shader: *Shader) Object {
        return Object{
            .shader = shader,
        };
    }

    pub fn setPosition(self: *Object, x: f32, y: f32) void {
        self.position[0] = x;
        self.position[1] = y;

        self.cached_model = null;
    }

    pub fn setScale(self: *Object, x: f32, y: f32) void {
        self.scale[0] = x;
        self.scale[1] = y;

        self.cached_model = null;
    }

    pub fn setRotation(self: *Object, rotation: f32) void {
        self.rotation = rotation;

        self.cached_model = null;
    }

    pub fn setColour(self: *Object, r: f32, g: f32, b: f32) void {
        self.colour[0] = r;
        self.colour[1] = g;
        self.colour[2] = b;
    }

    pub fn translate(self: *Object, x: f32, y: f32) void {
        self.position[0] += x;
        self.position[1] += y;

        self.cached_model = null;
    }

    pub fn scale(self: *Object, x: f32, y: f32) void {
        self.scale[0] *= x;
        self.scale[1] *= y;

        self.cached_model = null;
    }

    pub fn rotate(self: *Object, rotation: f32) void {
        self.rotation += rotation;

        self.cached_model = null;
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

        if (self.texture) |texture| {
            texture.use();
            self.shader.setBool("has_texture", true);
            self.shader.setInt("texture", 0);
        } else {
            self.shader.setBool("has_texture", false);
        }
    }

    pub fn setTexture(self: *Object, path: [:0]const u8) !void {
        self.texture = try Texture.init(path);
    }
};
