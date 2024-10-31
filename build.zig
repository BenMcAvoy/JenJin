const std = @import("std");

pub fn build(b: *std.Build) void {
    // Allow overriding of target and optimization mode.
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const cwd = b.pathFromRoot(".");
    const dir = std.fs.path.basename(cwd);
    const exe = b.addExecutable(.{
        .name = dir,
        .root_source_file = b.path("src/main.zig"),
        .target = target,
        .optimize = optimize,
    });

    // Use system-sdk from zig-gamedev to setup the include path
    switch (target.result.os.tag) {
        .windows => {
            if (target.result.cpu.arch.isX86()) {
                if (target.result.abi.isGnu() or target.result.abi.isMusl()) {
                    const system_sdk = b.lazyDependency("system_sdk", .{}).?;
                    exe.addLibraryPath(system_sdk.path("windows/lib/x86_64-windows-gnu"));
                }
            }
        },
        .macos => {
            const system_sdk = b.lazyDependency("system_sdk", .{}).?;
            exe.addLibraryPath(system_sdk.path("macos12/usr/lib"));
            exe.addFrameworkPath(system_sdk.path("macos12/System/Library/Frameworks"));
        },
        .linux => {
            if (target.result.cpu.arch.isX86()) {
                const system_sdk = b.lazyDependency("system_sdk", .{}).?;
                exe.addLibraryPath(system_sdk.path("linux/lib/x86_64-linux-gnu"));
            } else if (target.result.cpu.arch == .aarch64) {
                const system_sdk = b.lazyDependency("system_sdk", .{}).?;
                exe.addLibraryPath(system_sdk.path("linux/lib/aarch64-linux-gnu"));
            }
        },
        else => {},
    }

    const zglfw = b.dependency("zglfw", .{ .target = target });
    exe.root_module.addImport("zglfw", zglfw.module("root"));
    exe.linkLibrary(zglfw.artifact("glfw"));

    const zopengl = b.dependency("zopengl", .{ .target = target });
    exe.root_module.addImport("zopengl", zopengl.module("root"));

    const zmath = b.dependency("zmath", .{ .target = target });
    exe.root_module.addImport("zmath", zmath.module("root"));

    const zstbi = b.dependency("zstbi", .{});
    exe.root_module.addImport("zstbi", zstbi.module("root"));
    exe.linkLibrary(zstbi.artifact("zstbi"));

    // This declares intent for the executable to be installed into the
    // standard location when the user invokes the "install" step (the default
    // step when running `zig build`).
    b.installArtifact(exe);

    // This *creates* a Run step in the build graph, to be executed when another
    // step is evaluated that depends on it. The next line below will establish
    // such a dependency.
    const run_cmd = b.addRunArtifact(exe);

    // By making the run step depend on the install step, it will be run from the
    // installation directory rather than directly from within the cache directory.
    // This is not necessary, however, if the application depends on other installed
    // files, this ensures they will be present and in the expected location.
    run_cmd.step.dependOn(b.getInstallStep());

    // This allows the user to pass arguments to the application in the build
    // command itself, like this: `zig build run -- arg1 arg2 etc`
    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    // This creates a build step. It will be visible in the `zig build --help` menu,
    // and can be selected like this: `zig build run`
    // This will evaluate the `run` step rather than the default, which is "install".
    const run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);
}
