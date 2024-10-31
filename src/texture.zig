const std = @import("std");
const log = std.log.scoped(.shader);

const zmath = @import("zmath");
const zglfw = @import("zglfw");
const gl = @import("zopengl").bindings;

const zstbi = @import("zstbi");

pub const Texture = struct {
    id: c_uint = 0,

    pub fn init(path: [:0]const u8) !Texture {
        var allocator = std.heap.GeneralPurposeAllocator(.{}){};
        defer _ = allocator.deinit();

        zstbi.init(allocator.allocator());
        defer zstbi.deinit();

        zstbi.setFlipVerticallyOnLoad(true);
        var image = try zstbi.Image.loadFromFile(path, 4);
        defer image.deinit();

        var id: c_uint = undefined;
        gl.genTextures(1, &id);
        gl.activeTexture(gl.TEXTURE0);
        gl.bindTexture(gl.TEXTURE_2D, id);

        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);

        // TODO: Allow user to choose between linear and nearest filtering
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);

        gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, @intCast(image.width), @intCast(image.height), 0, gl.RGBA, gl.UNSIGNED_BYTE, @ptrCast(image.data));

        gl.generateMipmap(gl.TEXTURE_2D);

        return Texture{
            .id = id,
        };
    }

    pub fn use(self: Texture) void {
        gl.bindTexture(gl.TEXTURE_2D, self.id);
    }

    pub fn deinit(self: Texture) void {
        gl.deleteTextures(1, @ptrCast(&self.id));
    }
};
