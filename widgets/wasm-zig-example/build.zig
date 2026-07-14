const std = @import("std");

pub fn build(b: *std.Build) void {
  const target = b.resolveTargetQuery(.{
    .cpu_arch = .wasm32,
    .os_tag = .freestanding,
  });

  const optimize = b.standardOptimizeOption(.{ .preferred_optimize_mode = .ReleaseSmall });
  const lib = b.addExecutable(.{
    .name = "vespa-zig-example",
    .root_module = b.createModule(.{
        .root_source_file = b.path("main.zig"),
        .target = target,
        .optimize = optimize,
    }),
  });
  lib.entry = .disabled;
  lib.rdynamic = true;

  const my_lib_dep = b.dependency("wasm_zig_sdk", .{
    .target = target,
    .optimize = optimize,
  });
  const my_module = my_lib_dep.module("vespa-wasm-zig-sdk");
  lib.root_module.addImport("vespa-wasm-zig-sdk", my_module);
  b.installArtifact(lib);
}
