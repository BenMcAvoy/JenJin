const std = @import("std");
const log = std.log.scoped(.buffers);

const zglfw = @import("zglfw");

const gl = @import("zopengl").bindings;

const vertices = [_]f32{
    -0.5, 0.5, 0.0, // TL  0
    0.5, 0.5, 0.0, // TR   1
    -0.5, -0.5, 0.0, // BL 2
    0.5, -0.5, 0.0, // BR  3
};

const indices = [_]u32{
    0, 1, 3,
    2, 3, 0,
};

pub const Buffers = struct {
    vao: c_uint,
    vbo: c_uint,
    ebo: c_uint,

    // pub fn init(vertices: []const f32, indices: []const u32) !Buffers {
    pub fn init() Buffers {
        var vao: c_uint = undefined;
        var vbo: c_uint = undefined;
        var ebo: c_uint = undefined;

        gl.genBuffers(1, &ebo);
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, ebo);
        gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, @intCast(@sizeOf(u32) * indices.len), &indices, gl.STATIC_DRAW);

        gl.genBuffers(1, &vbo);
        gl.bindBuffer(gl.ARRAY_BUFFER, vbo);
        gl.bufferData(gl.ARRAY_BUFFER, @intCast(@sizeOf(f32) * vertices.len), &vertices, gl.STATIC_DRAW);

        gl.genVertexArrays(1, &vao);
        gl.bindVertexArray(vao);

        gl.vertexAttribPointer(0, 3, gl.FLOAT, gl.FALSE, 3 * @sizeOf(f32), @ptrFromInt(0));
        gl.enableVertexAttribArray(0);

        return Buffers{ .vbo = vbo, .vao = vao, .ebo = ebo };
    }

    pub fn use(self: *Buffers) void {
        gl.bindVertexArray(self.vao); // Binding the VAO tells the GPU to use the VBO and EBO
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, self.ebo);
        gl.bindBuffer(gl.ARRAY_BUFFER, self.vbo);
    }

    pub fn draw(self: *Buffers) void {
        _ = self;

        gl.drawElements(gl.TRIANGLES, 6, gl.UNSIGNED_INT, null);
    }

    pub fn deinit(self: *Buffers) void {
        gl.deleteBuffers(1, @ptrCast(&self.vbo));
        gl.deleteBuffers(1, @ptrCast(&self.ebo));
        gl.deleteVertexArrays(1, @ptrCast(&self.vao));

        log.info("Destroying buffers", .{});
    }
};
