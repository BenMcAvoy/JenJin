const std = @import("std");
const log = std.log.scoped(.shader);

const zmath = @import("zmath");
const zglfw = @import("zglfw");
const gl = @import("zopengl").bindings;

// NOTE: Technically a Shader Program
pub const Shader = struct {
    id: c_uint,

    pub fn init(vertex_shader: [:0]const u8, fragment_shader: [:0]const u8) !Shader {
        var success: c_int = undefined;
        var log_buf: [1024]u8 = undefined;
        var log_length: c_int = undefined;

        const vx_id: c_uint = gl.createShader(gl.VERTEX_SHADER);
        const fx_id: c_uint = gl.createShader(gl.FRAGMENT_SHADER);

        gl.shaderSource(vx_id, 1, @ptrCast(&vertex_shader), null);
        gl.compileShader(vx_id);
        gl.getShaderiv(vx_id, gl.COMPILE_STATUS, &success);
        if (success == gl.FALSE) {
            gl.getShaderInfoLog(vx_id, 1024, &log_length, log_buf[0..]);
            log.err("Vertex shader compilation failed: {s}", .{log_buf[0..@intCast(log_length)]});
            return error.VertexShaderCompilationFailed;
        }

        gl.shaderSource(fx_id, 1, @ptrCast(&fragment_shader), null);
        gl.compileShader(fx_id);
        gl.getShaderiv(fx_id, gl.COMPILE_STATUS, &success);
        if (success == gl.FALSE) {
            gl.getShaderInfoLog(fx_id, 1024, &log_length, log_buf[0..]);
            log.err("Fragment shader compilation failed: {s}", .{log_buf[0..@intCast(log_length)]});
            return error.FragmentShaderCompilationFailed;
        }

        const id = gl.createProgram();
        gl.attachShader(id, vx_id);
        gl.attachShader(id, fx_id);
        gl.linkProgram(id);

        gl.getProgramiv(id, gl.LINK_STATUS, &success);
        if (success == gl.FALSE) {
            gl.getProgramInfoLog(id, 1024, &log_length, log_buf[0..]);
            log.err("Shader program linking failed: {s}", .{log_buf[0..@intCast(log_length)]});
            return error.ShaderProgramLinkingFailed;
        }

        return Shader{
            .id = id,
        };
    }

    pub fn setMat4(self: Shader, name: [:0]const u8, value: zmath.Mat) void {
        gl.uniformMatrix4fv(gl.getUniformLocation(self.id, name), 1, gl.FALSE, @ptrCast(&value));
    }

    pub fn setVec3(self: Shader, name: [:0]const u8, value: @Vector(3, f32)) void {
        gl.uniform3fv(gl.getUniformLocation(self.id, name), 1, @ptrCast(&value));
    }

    pub fn use(self: Shader) void {
        gl.useProgram(self.id);
    }

    pub fn deinit(self: Shader) void {
        log.info("Destroying shader program", .{});
        gl.deleteProgram(self.id);
    }
};
