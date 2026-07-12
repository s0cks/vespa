const std = @import("std");

pub fn build(b: *std.Build) void {
  const target = b.standardTargetOptions(.{});
  const optimize = b.standardOptimizeOption(.{});

  const exe = b.addExecutable(.{
    .name = "vespa-worker",
    .root_module = b.createModule(.{
      .root_source_file = b.path("main.zig"),
      .target = target,
      .optimize = optimize,
      .link_libc = true, 
    }),
  });

  exe.root_module.addIncludePath(b.path("include"));
  exe.root_module.addIncludePath(b.path("../../build/_deps/wamr-src/core/iwasm/include"));
  exe.root_module.addIncludePath(b.path("../../lib/core/include"));
  exe.root_module.addLibraryPath(b.path("../../build/lib"));
  exe.root_module.addCSourceFiles(.{
    .files = &.{
      "src/worker.c",
    },
    .flags = &.{
      "-std=c23",
      "-D_GNU_SOURCE"
    },
  });
  exe.root_module.linkSystemLibrary("z", .{});
  exe.root_module.linkSystemLibrary("pthread", .{});
  exe.root_module.linkSystemLibrary("uv", .{});
  exe.root_module.linkSystemLibrary("iwasm", .{});
  exe.root_module.linkSystemLibrary("vespa-wamr", .{});
  exe.root_module.linkSystemLibrary("vespa-core", .{});
  b.installArtifact(exe);
}
